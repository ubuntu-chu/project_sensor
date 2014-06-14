#ifndef _DEVICES_H_
#define _DEVICES_H_

#include "../bsp/hal/hal.h"
#include "../includes/macro.h"

enum PROC_PHASE{

	PROC_PREPARE 		= 0,
	PROC_DONE,
};

class device:noncopyable{
public:
    portBASE_TYPE open(void);
    portBASE_TYPE close(void);
    portSSIZE_TYPE read(char *buffer, portSIZE_TYPE size);
    portSSIZE_TYPE read(portOFFSET_TYPE pos, char *buffer, portSIZE_TYPE size);
    portSSIZE_TYPE write(char *buffer, portSIZE_TYPE size);
	portSSIZE_TYPE write(char *buffer);
    portSSIZE_TYPE write(portOFFSET_TYPE pos, char *buffer, portSIZE_TYPE size);
	DeviceStatus_TYPE ioctl(uint8 cmd, void *args);
	DevicePoll_TYPE poll(void);
	uint8	device_stat_get(void);
	uint8	device_is_valid(void);
	virtual portBASE_TYPE process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size);
    virtual portBASE_TYPE process_write(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size);
    //portBASE_TYPE   name_set(const char *pname);
    portDEVHANDLE_TYPE handle_get(void)
    {
    	return m_pdevice;
    }

protected:
    device(const char *pname, uint16 oflag);
    virtual ~device();
	//recv buffer
	uint8			*m_pbuf_recv;
	//recv data seg
	uint8			*m_pbuf_data;
	//send buffer
	uint8			*m_pbuf_send;
	//total recv len
	uint16			m_len_recv;
	uint16			m_len_send;
	//recv data len
	uint16			m_len_data;
	//recv buff len  
	uint16          m_buf_recv_len;
    device_t        *m_pdevice;

private:
    uint16          m_oflag;
    uint8			m_offline_cnt;
    const char 	*m_name;
};













#endif


