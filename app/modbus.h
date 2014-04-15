#ifndef _MODBUS_H_
#define _MODBUS_H_

#include "../includes/includes.h"
#include "protocol.h"

#ifdef MODBUS

//HEADER_LENGTH_RTU (1) + function (1) + address (2) + number (2) + CRC (2)

#define _MIN_REQ_LENGTH 12
#define _REPORT_SLAVE_ID 180
#define _MODBUS_EXCEPTION_RSP_LENGTH 5

/* Function codes */
#define _FC_READ_COILS                0x01
#define _FC_READ_DISCRETE_INPUTS      0x02
#define _FC_READ_HOLDING_REGISTERS    0x03
#define _FC_READ_INPUT_REGISTERS      0x04
#define _FC_WRITE_SINGLE_COIL         0x05
#define _FC_WRITE_SINGLE_REGISTER     0x06
#define _FC_READ_EXCEPTION_STATUS     0x07
#define _FC_WRITE_MULTIPLE_COILS      0x0F
#define _FC_WRITE_MULTIPLE_REGISTERS  0x10
#define _FC_REPORT_SLAVE_ID           0x11
#define _FC_MASK_WRITE_REGISTER       0x16
#define _FC_WRITE_AND_READ_REGISTERS  0x17

#define _MODBUS_RTU_HEADER_LENGTH      1
#define _MODBUS_RTU_PRESET_REQ_LENGTH  6
#define _MODBUS_RTU_PRESET_RSP_LENGTH  2

#define _MODBUS_RTU_CHECKSUM_LENGTH    2

/* Max between RTU and TCP max adu length (so TCP) */
#define MAX_MESSAGE_LENGTH 260
/* Internal use */
#define MSG_LENGTH_UNDEFINED -1
#define MODBUS_BROADCAST_ADDRESS    0

#define LIBMODBUS_VERSION_STRING     "modbus-1.0"

/* Protocol exceptions */
enum {
    MODBUS_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
    MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
    MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
    MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE,
    MODBUS_EXCEPTION_ACKNOWLEDGE,
    MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY,
    MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE,
    MODBUS_EXCEPTION_MEMORY_PARITY,
    MODBUS_EXCEPTION_NOT_DEFINED,
    MODBUS_EXCEPTION_GATEWAY_PATH,
    MODBUS_EXCEPTION_GATEWAY_TARGET,
    MODBUS_EXCEPTION_MAX
};


/* 3 steps are used to parse the query */
typedef enum {
    _STEP_FUNCTION,
    _STEP_META,
    _STEP_DATA
} _step_t;




typedef struct _sft {
    int slave;
    int function;
    int t_id;
} sft_t;

struct _modbus {
    
    int slave;
    int debug;
};

typedef struct _modbus modbus_t;

typedef struct {
    int nb_bits;
    int nb_input_bits;
    int nb_input_registers;
    int nb_registers;
    uint8_t *tab_bits;
    uint8_t *tab_input_bits;
    uint16_t *tab_input_registers;
    uint16_t *tab_registers;
} modbus_mapping_t;


class protocol_modbus_rtu:public protocol{
public:
	protocol_modbus_rtu();
	virtual ~protocol_modbus_rtu();

	virtual void init(void);
	virtual uint16 pack(uint8_t*dst, uint8 *src, uint16 len);
	virtual int8   unpack(uint8_t* pbuf, uint16 len);
    portBASE_TYPE slave_set(uint8 addr);
    void modbus_mapping_set(int nb_bits,
                            int nb_input_bits,
                            int nb_input_registers, 
                            int nb_registers,
                            uint8_t *tab_bits,
                            uint8_t *tab_input_bits,
                            uint16_t *tab_input_registers,
                            uint16_t *tab_registers)
    {
        m_mapping.nb_bits                   = nb_bits;
        m_mapping.nb_input_bits             = nb_input_bits;
        m_mapping.nb_input_registers        = nb_input_registers;
        m_mapping.nb_registers              = nb_registers;
        m_mapping.tab_bits                  = tab_bits;
        m_mapping.tab_input_bits            = tab_input_bits;
        m_mapping.tab_input_registers       = tab_input_registers;
        m_mapping.tab_registers             = tab_registers;
        
    }

private:
    modbus_t                    m_modbus;            /* Slave address */
    modbus_mapping_t            m_mapping;
    
};

extern class protocol_modbus_rtu 	t_protocol_modbus_rtu;

#endif








#endif

