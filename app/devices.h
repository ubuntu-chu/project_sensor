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
    char               *m_pbuf_recv;                //�豸�����ڽ��յĻ�����
    //ָ���豸�ڽ��ջ�������ʵ������λ��   �豸���ܶԽ��յ������ݽ��д����������µ����� ��ָ��ָ�����µ�����
    char               *m_pbuf_recv_actual;
    char               *m_pbuf_send;                //�豸�����ڷ��͵Ļ�����
    char               *m_pbuf_send_actual;

    portSIZE_TYPE       m_buf_recv_size;            //�豸�����ڽ��յĻ�������С
    portSIZE_TYPE       m_buf_send_size;            //�豸�����ڷ��͵Ļ�������С
    portSSIZE_TYPE      m_recv_actual_size;         //�豸��ʵ�ʽ������ݵĴ�С  Ҳ��read�����ķ���ֵ
    portSSIZE_TYPE      m_send_actual_size;         //�豸��ʵ�ʷ������ݵĴ�С  Ҳ��write�����ķ���ֵ
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


