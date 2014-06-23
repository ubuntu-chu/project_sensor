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
    uint8 					slave;
    uint8 					debug;

    uint16					input_reg_max_no;
    uint16 					hold_reg_max_no;
};

class modbus_rtu_info:public protocol_info{
public:
	modbus_rtu_info():m_reg(0),m_reg_no(0),m_function(0){}
	~modbus_rtu_info(){}

	void function_set(int function){m_function = function;}
	int function_get(void){return m_function;}

	void reg_set(int reg){m_reg = reg;}
	int reg_get(void){return m_reg;}

	void reg_no_set(int len){m_reg_no = len;}
	int reg_no_get(void){return m_reg_no;}
	
	void param_addr_set(uint8 *param){m_pparam = param;}
	uint8 *param_addr_get(void){return m_pparam;}

	void param_len_set(uint16 len){m_param_len 	= len;}
	uint16 param_len_get(void){return m_param_len;}
private:
	uint16		m_reg;
	uint16 	m_reg_no;
	uint16  m_param_len;
	uint8 	*m_pparam;
    uint8 	m_function;
};

typedef struct _modbus modbus_t;

class protocol_modbus_rtu:public protocol{
public:
	protocol_modbus_rtu(fp_protocol_handle *handle, void *pvoid);
	virtual ~protocol_modbus_rtu();

	virtual void init(void);
	virtual uint16 pack(uint8_t*dst, uint8 *src, uint16 len);
	virtual int8   unpack(uint8_t* pbuf, uint16 len);
    virtual portBASE_TYPE  handle(enum protocol_phase phase)
	{
		if ((NULL != m_fp_handle) && (0 == m_fp_handle(m_pvoid, phase, &m_info))){
			return 0;
		}
		return -1;
	}
    portBASE_TYPE slave_set(uint8 addr);
    void reg_max_no_set(uint16 input_reg, uint16 hold_reg)
    {
    	m_modbus.input_reg_max_no					= input_reg;
    	m_modbus.hold_reg_max_no					= hold_reg;
    }
	portBASE_TYPE  info(const uint8_t*package, uint16 len, class protocol_info *pinfo);

public:
    modbus_t                    m_modbus;            /* Slave address */
    modbus_rtu_info				m_info;
};


#endif








#endif


