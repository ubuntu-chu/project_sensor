#include "modbus.h"

#ifdef MODBUS

/* Define the slave ID of the remote device to talk in master mode or set the
 * internal slave ID in slave mode */
static int _modbus_set_slave(modbus_t *ctx, int slave)
{
    /* Broadcast address is 0 (MODBUS_BROADCAST_ADDRESS) */
    if (slave >= 0 && slave <= 247) {
        ctx->slave = slave;
    } else {
        return -1;
    }

    return 0;
}

/* Builds a RTU response header */
static int _modbus_rtu_build_response_basis(sft_t *sft, uint8_t *rsp)
{
    /* In this case, the slave is certainly valid because a check is already
     * done in _modbus_rtu_listen */
    rsp[0] = sft->slave;
    rsp[1] = sft->function;

    return _MODBUS_RTU_PRESET_RSP_LENGTH;
}

static int _modbus_rtu_prepare_response_tid(const uint8_t *req, int *req_length)
{
    (*req_length) -= _MODBUS_RTU_CHECKSUM_LENGTH;
    /* No TID */
    return 0;
}

static int _modbus_rtu_send_msg_pre(uint8_t *req, int req_length)
{
    uint16_t crc = crc16(reinterpret_cast<const uint8 *>(req), req_length);
    req[req_length++] = crc >> 8;
    req[req_length++] = crc & 0x00FF;

    return req_length;
}

/* Build the exception response */
static int response_exception(modbus_t *ctx, sft_t *sft,
                              int exception_code, uint8_t *rsp)
{
    int rsp_length;

    sft->function   = sft->function + 0x80;
    rsp_length      = _modbus_rtu_build_response_basis(sft, rsp);

    /* Positive exception code */
    rsp[rsp_length++] = exception_code;

    return rsp_length;
}

int modbus_reply(modbus_t *ctx, uint8 *rsp, const uint8_t *req,
                 int req_length, modbus_mapping_t *mb_mapping)
{
    int offset          = _MODBUS_RTU_HEADER_LENGTH;
    int slave           = req[offset - 1];
    int function        = req[offset];
    uint16_t address    = (req[offset + 1] << 8) + req[offset + 2];
    int rsp_length = 0;
    sft_t sft;

    sft.slave           = slave;
    sft.function        = function;
    sft.t_id            = _modbus_rtu_prepare_response_tid(req, &req_length);

    switch (function) {
    case _FC_READ_HOLDING_REGISTERS: {
        int nb = (req[offset + 3] << 8) + req[offset + 4];

        if ((address + nb) > mb_mapping->nb_registers) {
            if (ctx->debug) {
                
            }
            rsp_length = response_exception(
                ctx, &sft,
                MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
        } else {
            int i;

            rsp_length = _modbus_rtu_build_response_basis(&sft, rsp);
            rsp[rsp_length++] = nb << 1;
            for (i = address; i < address + nb; i++) {
                rsp[rsp_length++] = mb_mapping->tab_registers[i] >> 8;
                rsp[rsp_length++] = mb_mapping->tab_registers[i] & 0xFF;
            }
        }
    }
        break;
    case _FC_READ_INPUT_REGISTERS: {
        /* Similar to holding registers (but too many arguments to use a
         * function) */
        int nb = (req[offset + 3] << 8) + req[offset + 4];

        if ((address + nb) > mb_mapping->nb_input_registers) {
            if (ctx->debug) {
                
            }
            rsp_length = response_exception(
                ctx, &sft,
                MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
        } else {
            int i;

            rsp_length = _modbus_rtu_build_response_basis(&sft, rsp);
            rsp[rsp_length++] = nb << 1;
            for (i = address; i < address + nb; i++) {
                rsp[rsp_length++] = mb_mapping->tab_input_registers[i] >> 8;
                rsp[rsp_length++] = mb_mapping->tab_input_registers[i] & 0xFF;
            }
        }
    }
        break;
    case _FC_WRITE_SINGLE_REGISTER:
        if (address >= mb_mapping->nb_registers) {
            if (ctx->debug) {
                
            }
            rsp_length = response_exception(
                ctx, &sft,
                MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
        } else {
            int data = (req[offset + 3] << 8) + req[offset + 4];

            mb_mapping->tab_registers[address] = data;
            memcpy(rsp, req, req_length);
            rsp_length = req_length;
        }
        break;
    case _FC_WRITE_MULTIPLE_REGISTERS: {
        int nb = (req[offset + 3] << 8) + req[offset + 4];

        if ((address + nb) > mb_mapping->nb_registers) {
            if (ctx->debug) {
                
            }
            rsp_length = response_exception(
                ctx, &sft,
                MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
        } else {
            int i, j;
            for (i = address, j = 6; i < address + nb; i++, j += 2) {
                /* 6 and 7 = first value */
                mb_mapping->tab_registers[i] =
                    (req[offset + j] << 8) + req[offset + j + 1];
            }

            rsp_length = _modbus_rtu_build_response_basis(&sft, rsp);
            /* 4 to copy the address (2) and the no. of registers */
            memcpy(rsp + rsp_length, req + rsp_length, 4);
            rsp_length += 4;
        }
    }
        break;
    case _FC_REPORT_SLAVE_ID: {
        int str_len;
        int byte_count_pos;

        rsp_length = _modbus_rtu_build_response_basis(&sft, rsp);
        /* Skip byte count for now */
        byte_count_pos = rsp_length++;
        rsp[rsp_length++] = _REPORT_SLAVE_ID;
        /* Run indicator status to ON */
        rsp[rsp_length++] = 0xFF;
        /* LMB + length of LIBMODBUS_VERSION_STRING */
        str_len = 3 + strlen(LIBMODBUS_VERSION_STRING);
        memcpy(rsp + rsp_length, "LMB" LIBMODBUS_VERSION_STRING, str_len);
        rsp_length += str_len;
        rsp[byte_count_pos] = rsp_length - byte_count_pos - 1;
    }
        break;
    default:
        rsp_length = response_exception(ctx, &sft,
                                        MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
                                        rsp);
        break;
    }
    rsp_length = _modbus_rtu_send_msg_pre(rsp, rsp_length);
    
    return rsp_length;
}






protocol_modbus_rtu::protocol_modbus_rtu()
{
    
}

protocol_modbus_rtu::~protocol_modbus_rtu()
{
    
}

portBASE_TYPE protocol_modbus_rtu::slave_set(uint8 addr)
{
    return _modbus_set_slave(&m_modbus, addr);
}


void protocol_modbus_rtu::init(void)
{
    
}

uint16 protocol_modbus_rtu::pack(uint8_t*dst, uint8 *src, uint16 len)
{
    return modbus_reply(&m_modbus, dst, src, len, &m_mapping);
}

int8   protocol_modbus_rtu::unpack(uint8_t* pbuf, uint16 len)
{
    uint16_t crc_calculated;
    uint16_t crc_received;
    uint8    slave  = pbuf[0];

    if (len > MAX_MESSAGE_LENGTH){
        return -3;
    }
    
    /* Filter on the Modbus unit identifier (slave) in RTU mode */
    if (slave != m_modbus.slave && slave != MODBUS_BROADCAST_ADDRESS) {
        return -1;
    }

    crc_calculated = crc16(reinterpret_cast<const uint8 *>(pbuf), len - 2);
    crc_received = (pbuf[len - 2] << 8) | pbuf[len - 1];

    /* Check CRC of msg */
    if (crc_calculated != crc_received) {
        return -2;
    }
    
    return 0;
}





class protocol_modbus_rtu 	t_protocol_modbus_rtu;
















#endif

