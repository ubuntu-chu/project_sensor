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
    uint16_t crc = crc16(const_cast<const uint8 *>(req), req_length);
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


int modbus_reply(protocol_modbus_rtu *rtu, uint8 *rsp, const uint8_t *req,
                 int req_length)
{
    int offset          = _MODBUS_RTU_HEADER_LENGTH;
    int slave           = req[offset - 1];
    int function        = req[offset];
    int rsp_length = 0;
    sft_t sft;

    sft.slave           = slave;
    sft.function        = function;
    //从接收到的长度中 取出crc校验的两个字节
    sft.t_id            = _modbus_rtu_prepare_response_tid(req, &req_length);

    if ((_FC_READ_HOLDING_REGISTERS == function)
    	|| (_FC_READ_INPUT_REGISTERS == function)
    	|| (_FC_WRITE_SINGLE_REGISTER == function)
    	|| (_FC_WRITE_MULTIPLE_REGISTERS == function)){

        bool   read_operation               = false;
        uint16	max_reg_no;

		rtu->info(req, req_length, &rtu->m_info);
        rsp_length                          = _modbus_rtu_build_response_basis(&sft, rsp);
		if ((_FC_READ_HOLDING_REGISTERS == function)
			|| (_FC_READ_INPUT_REGISTERS == function)){
            rsp[rsp_length++]               = (rtu->m_info.reg_no_get()) << 1;
            rtu->m_info.param_addr_set(const_cast<uint8 *>(&(rsp[rsp_length])));
            read_operation                  = true;
		}
		//获取modbus寄存器最大数目
		if (_FC_READ_INPUT_REGISTERS == function){
			max_reg_no						= rtu->m_modbus.input_reg_max_no;
		}else {
			max_reg_no						= rtu->m_modbus.hold_reg_max_no;
		}
		if ((rtu->m_info.reg_get() + rtu->m_info.reg_no_get() < max_reg_no)
			&& (0 == rtu->handle(enum_PROTOCOL_PREPARE))){
            if (true == read_operation){
                rsp_length                  += rtu->m_info.param_len_get();
            }else {
                if (_FC_WRITE_SINGLE_REGISTER == function){
                    memcpy(rsp, req, req_length);
                    rsp_length = req_length;
                }else {
                    rsp_length = _modbus_rtu_build_response_basis(&sft, rsp);
                    /* 4 to copy the address (2) and the no. of registers */
                    memcpy(rsp + rsp_length, req + rsp_length, 4);
                    rsp_length += 4;
                }
            }

        }else {
            rsp_length = response_exception(
                &rtu->m_modbus, &sft,
                MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
        }
        
    }else if (_FC_REPORT_SLAVE_ID == function) {

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
    }else {
        rsp_length = response_exception(&rtu->m_modbus, &sft,
                                        MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
                                        rsp);
    }
    rsp_length = _modbus_rtu_send_msg_pre(rsp, rsp_length);
    
    return rsp_length;
}

protocol_modbus_rtu::protocol_modbus_rtu(fp_protocol_handle *handle, void *pvoid):protocol(handle, pvoid)
{
    m_type 			                = enum_MODBUS_RTU;
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
    return modbus_reply(this, dst, src, len);
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

portBASE_TYPE  protocol_modbus_rtu::info(const uint8_t*package, uint16 len, class protocol_info *pinfo)

{
    int offset          = _MODBUS_RTU_HEADER_LENGTH;
    int function;
    class modbus_rtu_info *pmodbus_rtu_info;

    pmodbus_rtu_info = static_cast<class modbus_rtu_info *>(pinfo);
    function        	= package[offset];
    pinfo->type_set(m_type);
	pmodbus_rtu_info->function_set(function);
	if ((function == _FC_READ_HOLDING_REGISTERS)
			|| (function == _FC_READ_INPUT_REGISTERS)
			|| (function == _FC_WRITE_SINGLE_REGISTER)
			|| (function == _FC_WRITE_MULTIPLE_REGISTERS)){
		pmodbus_rtu_info->reg_set((package[offset + 1] << 8) + package[offset + 2]);
		if (function == _FC_WRITE_SINGLE_REGISTER){
            pmodbus_rtu_info->reg_no_set(1);
            pmodbus_rtu_info->param_addr_set(const_cast<uint8 *>(&(package[offset + 3])));
        }else {
            pmodbus_rtu_info->reg_no_set((package[offset + 3] << 8) + package[offset + 4]);
            pmodbus_rtu_info->param_addr_set(const_cast<uint8 *>(&(package[offset + 5])));
        }
		pinfo->package_addr_set(reinterpret_cast<char *>(const_cast<uint8_t *>(package)));
		pinfo->package_len_set(len);
	}else{
		pmodbus_rtu_info->reg_set(-1);
		pmodbus_rtu_info->reg_no_set(-1);
	}
	return 0;
}

















#endif

