#ifndef _DEVICES_H_
#define _DEVICES_H_

#include "../bsp/hal/hal.h"
#include "../includes/macro.h"

enum PROC_PHASE{

	PHASE_PREPARE 		= 0,
	PHASE_DONE,
};

enum PROC_DIR{
	DIR_READ 		= 0,
	DIR_WRITE,
};

struct device_buffer{
    char               *m_pbuf_recv;                //设备内用于接收的缓冲区
    //指向设备内接收缓冲区的实际数据位置   设备可能对接收到的数据进行处理生成了新的数据 此指针指向最新的数据
    char               *m_pbuf_recv_actual;
    char               *m_pbuf_send;                //设备内用于发送的缓冲区
    char               *m_pbuf_send_actual;

    portSIZE_TYPE       m_buf_recv_size;            //设备内用于接收的缓冲区大小
    portSIZE_TYPE       m_buf_send_size;            //设备内用于发送的缓冲区大小
    portSSIZE_TYPE      m_recv_actual_size;         //设备内实际接收数据的大小  也即read函数的返回值
    portSSIZE_TYPE      m_send_actual_size;         //设备内实际发送数据的大小  也即write函数的返回值
};

class device:noncopyable{
public:
    portBASE_TYPE open(uint16 oflag);
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
	virtual portBASE_TYPE process_readwrite(enum PROC_DIR dir, enum PROC_PHASE phase, struct device_buffer &device_buffer);
    //portBASE_TYPE   name_set(const char *pname);
    portDEVHANDLE_TYPE handle_get(void)
    {
    	return m_pdevice;
    }

protected:
    device(const char *pname):
		m_name(pname)
	{
		m_pdevice = NULL;
		m_offline_cnt = 0;
	}
    virtual ~device();
    device_t        *m_pdevice;

private:
    const char 	*const m_name;
    uint8			m_offline_cnt;
};

    
    
    












#endif


