/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "devices.h"

#include    "../bsp/hal/hal.h"
#include    "../includes/Macro.h"
#include    "../api/log/log.h"

CDevice_base::CDevice_base(const char *pname, uint16 oflag)
{
    m_pdevice   = NULL;
    if ((NULL == pname) || (strlen(pname) > DEVICE_NAME_MAX)){
        return;
    }
    strcpy(m_name, pname);
    m_oflag     = oflag;
	m_len_recv  = 0;
	m_len_send  = 0;
	m_pbuf_recv		= NULL;
	m_pbuf_send		= NULL;
	m_offline_cnt	= 0;
}

CDevice_base::~CDevice_base()
{
    close();
    memset(m_name, 0, sizeof(m_name));
    m_pdevice   = NULL;
}

portBASE_TYPE CDevice_base::open(void)
{
    //查找设备
    m_pdevice   = hal_devicefind(m_name);
    if (NULL == m_pdevice){
	    LOG_WARN << "device:" << m_name << "find fail!";
        return (portBASE_TYPE)-1;
    }
    //打开设备
    if (DEVICE_OK != hal_deviceopen(m_pdevice, m_oflag)){
    	LOG_WARN << "device:" << m_name << "open fail!";
        m_pdevice 		= NULL;
        return (portBASE_TYPE)-2;
    }
    return 0;
}

portBASE_TYPE CDevice_base::close(void)
{
    if (NULL != m_pdevice){
        hal_deviceclose(m_pdevice);
    }
    
    return 0;
}

uint8	CDevice_base::device_stat_get(void)
{
	//对于应答式设备 设备连续离线5次后  才认为设备离线
	return (m_offline_cnt > 5)?(0):(1);
}

uint8	CDevice_base::device_is_valid(void)
{
	return (m_pdevice == NULL)?(0):(1);
}

portSSIZE_TYPE CDevice_base::read(portOFFSET_TYPE pos, char *buffer, portSIZE_TYPE size)
{
    if ((NULL == m_pdevice)){
        return -1;
    }
    if (this->process_read(PROC_PREPARE, buffer, size)){
    	return -1;
    }
    if (m_pbuf_recv == NULL){
        return -1;
    }
    size  = (portSIZE_TYPE)API_DeviceRead(m_pdevice, pos, m_pbuf_recv, m_buf_recv_len);
    if (size == ((portSIZE_TYPE)-1)){
		if (API_DeviceErrorInfoGet() == DEVICE_ETIMEOUT){
			m_offline_cnt++;
		}else {
			m_offline_cnt	= 0;
		}
        return -1;
    }
    m_offline_cnt 			= 0;

	m_len_recv				= size;
    //call virtual process_read to done
    if (this->process_read(PROC_DONE, buffer, size)){
		return -1;
	}
	if ((buffer != NULL) && (NULL != m_pbuf_data) 
        && (buffer != reinterpret_cast<char *>(m_pbuf_data))){
		memcpy(buffer, m_pbuf_data, m_len_data);
	}
    
    return m_len_data;
}

//Note: read return m_len_data  if the cmd rsp no data, then read return 0!
portSSIZE_TYPE CDevice_base::read(char *buffer, portSIZE_TYPE size)
{
	return read(0, buffer, size);
}

portSSIZE_TYPE CDevice_base::write(char *buffer, portSIZE_TYPE size)
{
	return write(0, buffer, size);
}

portSSIZE_TYPE CDevice_base::write(char *buffer)
{
	return write(0, buffer, strlen(buffer));
}

portSSIZE_TYPE CDevice_base::write(portOFFSET_TYPE pos, char *buffer, portSIZE_TYPE size)
{
    if (NULL == m_pdevice){
        return -1;
    }
    if (this->process_write(PROC_PREPARE, buffer, size)){
    	return -1;
    }
    size = API_DeviceWrite(m_pdevice, pos, m_pbuf_send, m_len_send);
    if (this->process_write(PROC_DONE, buffer, size)){
    	return -1;
    }
    
    return size;
}

DeviceStatus_TYPE CDevice_base::ioctl(uint8 cmd, void *args)
{
    if (NULL == m_pdevice){
        return DEVICE_ENULL;
    }
    return API_DeviceControl(m_pdevice, cmd, args);
}

portBASE_TYPE CDevice_base::process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size)
{
	switch (phase){
	case PROC_PREPARE:
    	m_pbuf_recv 			= (uint8 *)pbuf;
    	m_pbuf_data 			= m_pbuf_recv;
    	m_buf_recv_len 			= size;
    	m_len_data 				= m_buf_recv_len;
		break;

	case PROC_DONE:
		break;

	default:
		break;
	}

	return 0;
}

portBASE_TYPE CDevice_base::process_write(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size)
{
	switch (phase){
	case PROC_PREPARE:
		m_pbuf_send								= reinterpret_cast<uint8 *>(pbuf);
		m_len_send								= size;
		break;

	case PROC_DONE:
		break;

	default:
		break;
	}

	return 0;
}

#if 0
portBASE_TYPE CDeive_::process_read(enum PROC_PHASE phase, char *pbuf, uint16 size);
{
	switch (phase){
	case PROC_PREPARE:
		break;

	case PROC_DONE:
		break;

	default:
		break;
	}

	return 0;
}
#endif

