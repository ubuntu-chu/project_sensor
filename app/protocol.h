#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "../includes/includes.h"

enum protocol_type{
	enum_RAW_MAC	= 0,
	enum_MODBUS_RTU,

	enum_TYPE_MAX,
};

class protocol_info{
public:
	protocol_info(){}
	~protocol_info(){}
public:
	void type_set(enum protocol_type type){m_type = type;}
	enum protocol_type type_get(void){return m_type;}

protected:
    enum protocol_type   m_type;             //protocol type
};


class protocol{
public:
	protocol(){};
	virtual ~protocol(){};

	virtual void init(void) = 0;
    virtual uint16 pack(uint8_t*dst, uint8 *src, uint16 len){return 0;}
	virtual int8   unpack(uint8_t* pbuf, uint16 len){return 0;}
	virtual portBASE_TYPE  info(const uint8_t*package, uint16 len, class protocol_info *pinfo){return 0;}

protected:
    enum protocol_type   m_type;             //protocol type
};

#ifdef RAW_MAC


// BASE FILE
#define def_FRAME_PHY_LEN_LEN		1
#define def_FRAME_DELIMITER_LEN     2

enum{
	RSP_OK		= 0,
	RSP_INVALID_CMD,
	RSP_INVALID_PARAM,
	RSP_EXEC_FAILURE,
	RSP_INVALID_PARAM_LEN,
	RSP_ABILITY_ERR,
	//主动模式丿收到应答帿	RSP_ACK_IN_ACTIVE_MODE,
	RSP_TYPE_ERR,
};

typedef enum{
	FRAME_TYPE_CTRL = 0x00,		//
	FRAME_TYPE_HEARTBEAT,		//
	FRAME_TYPE_STATUS,			// status out
	FRAME_TYPE_DATA,			// sensor data out
}FRAME_TYPE_T;

enum{
	DOWNSTREAM		= 0,
	UPSTREAM,
};

enum{
	NO_ACK_REQUEST		= 0,
	ACK_REQUEST,
};

enum{
	NO_ACK_FRAME		= 0,
	ACK_FRAME,
};

typedef struct{
	uint16_t frm_type	:	4,             	// frame type;
	direction			:	1,				// direction: 1 sensor -> sink, 0 sink -> sensor;
	frm_pending			:	1,				// frame pending; 	data to send for expand
	ack_req	        	:	1,				// ACK request; 	this frame need to ack
    ack_mask            :   1,              // ACK bit. 1 for ack frame
	reserved           	:	2,             	//reseverd
	des_addr_mode      	:	2,             	// dest addressing mode;
	frm_version        	:	2,             	//frame version
	src_addr_mode		:	2;             	//soure addressing mode
} mac_frm_ctrl_t;



typedef struct{
	uint16_t	delimiter_start;
	uint16_t	len;
	uint16_t	seq_id;
	mac_frm_ctrl_t	ctl;
	sys_time_t	time;
	dev_adr_t	dev_adr;
	sen_adr_t	sen_adr;
	uint8_t		type;
}mac_frame_t;

typedef struct{
	uint8_t		fun;
	uint8_t		sum;
	uint8_t		idex;
	uint16_t	len;
}app_frame_t;

typedef struct{
	mac_frame_t mac_frm_ptr;
	app_frame_t app_frm_ptr;
	uint16_t	app_data_len;
	uint8_t*	data_ptr;
}frame_ctl_t;

//function code
enum{
	def_FUNC_CODE_HEARBEAT	= 1,
	def_FUNC_CODE_SET,
};


#define def_FRAME_LEN_LEN			2
#define def_FRAME_SEQ_LEN			2
#define def_FRAME_CTL_LEN			2

#define def_ACK_MASK			(1<<7)
#define def_ACK_NEED_MASK		(1<<6)
#define def_DATA_SEND_MASK		(1<<5)
#define def_UP_DIR_MASK			(1<<4)


#define def_FRAME_TIME_LEN			4
#define def_FRAME_DST_LEN			4
#define def_FRAME_SRC_LEN			4

#define def_FRAME_TYPE_LEN			1
#define def_FRAME_FUN_LEN			1
#define def_FRAME_SUM_PACK_LEN		1
#define def_FRAME_PACK_IDEX_LEN		1
#define def_FRAME_DATA_LEN			2

#define def_FRAME_CK_LEN			2


#define def_FRAME_MAC_LEN 			def_FRAME_SEQ_LEN+def_FRAME_CTL_LEN  \
									+def_FRAME_TIME_LEN+def_FRAME_DST_LEN \
									+def_FRAME_SRC_LEN+def_FRAME_TYPE_LEN

#define def_FRAME_APP_HEAD_LEN		def_FRAME_FUN_LEN + def_FRAME_SUM_PACK_LEN


class protocol_mac:public protocol{
public:
	protocol_mac();
	virtual ~protocol_mac();

	virtual void init(void);
	virtual uint16 pack(mac_frame_t* mac_frm_ptr, app_frame_t* app_frm_ptr, uint8_t* ptr, uint8_t* pData);
	virtual int8	unpack(uint8_t* pbuf, uint16 len, frame_ctl_t *pfrm_ctl);
	virtual void frm_ctl_init(frame_ctl_t *pfrm_ctl, mac_frm_ctrl_t frm_ctl, uint8 total, uint8 index, uint8 func_code, uint8 *pbuf, uint16 len);
	virtual void mac_frm_ctrl_init(mac_frm_ctrl_t *pfrm_ctrl, uint8 ack, uint8 dir, uint8 ack_req, uint8 frm_type);
};

extern class protocol_mac 	t_protocol_mac;

#endif

#endif


