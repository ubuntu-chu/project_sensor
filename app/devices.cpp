/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "devices.h"

#include    "../bsp/hal/hal.h"
#include    "../includes/Macro.h"
#include    "../api/log/log.h"

device::~device()
{
    close();
    m_pdevice   = NULL;
}

portBASE_TYPE device::open(uint16 oflag)
{
    //查找设备
    m_pdevice   = hal_devicefind(m_name);
    if (NULL == m_pdevice){
    #ifdef LOGGER
	    LOG_WARN << "device:" << m_name << "find fail!";
    #endif
    	API_DeviceErrorInfoSet(DEVICE_ENOEXSITED);
        return -1;
    }
    //打开设备
    if (DEVICE_OK != hal_deviceopen(m_pdevice, oflag)){
    #ifdef LOGGER
    	LOG_WARN << "device:" << m_name << "open fail!";
    #endif
        m_pdevice 		= NULL;
    	API_DeviceErrorInfoSet(DEVICE_EOPEN);
        return -1;
    }
    return 0;
}

portBASE_TYPE device::close(void)
{
    if (NULL != m_pdevice){
        hal_deviceclose(m_pdevice);
    }
    
    return 0;
}

uint8	device::device_stat_get(void)
{
	//对于应答式设备 设备连续离线5次后  才认为设备离线
	return (m_offline_cnt > 5)?(0):(1);
}

uint8	device::device_is_valid(void)
{
	return (m_pdevice == NULL)?(0):(1);
}

portSSIZE_TYPE device::read(portOFFSET_TYPE pos, char *buffer, portSIZE_TYPE size)
{
    struct device_buffer    t_device_buffer;
    portSSIZE_TYPE 			actual_size;
    
    if ((NULL == m_pdevice) || (NULL == buffer)){
    	API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
        return -1;
    }
    if (0 == size){
    	return 0;
    }
    t_device_buffer.m_pbuf_recv 		= buffer;
    t_device_buffer.m_buf_recv_size 	= size;
    if (this->process_readwrite(DIR_READ, PHASE_PREPARE, t_device_buffer)){
    	API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
    	return -1;
    }
    if (t_device_buffer.m_pbuf_recv == NULL){
    	API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
        return -1;
    }
    actual_size  = API_DeviceRead(m_pdevice, pos, t_device_buffer.m_pbuf_recv, t_device_buffer.m_buf_recv_size);
    if (actual_size == ((portSSIZE_TYPE)-1)){
		if (API_DeviceErrorInfoGet() == DEVICE_ETIMEOUT){
			m_offline_cnt++;
		}else {
			m_offline_cnt	= 0;
		}
        return -1;
    }
    m_offline_cnt 			= 0;

    t_device_buffer.m_pbuf_recv_actual		= t_device_buffer.m_pbuf_recv;
    t_device_buffer.m_recv_actual_size		= actual_size;
    //call virtual process_read to done
    if (this->process_readwrite(DIR_READ, PHASE_DONE, t_device_buffer)){
    	API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
		return -1;
	}
	if (buffer != reinterpret_cast<char *>(t_device_buffer.m_pbuf_recv_actual)){
		memcpy(buffer, t_device_buffer.m_pbuf_recv_actual, t_device_buffer.m_recv_actual_size);
	}
    
    return t_device_buffer.m_recv_actual_size;
}

portSSIZE_TYPE device::write(portOFFSET_TYPE pos, char *buffer, portSIZE_TYPE size)
{
    struct device_buffer    t_device_buffer;
    portSSIZE_TYPE 			actual_size;

    if ((NULL == m_pdevice) || (NULL == buffer)){
    	API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
        return -1;
    }
    if (0 == size){
    	return 0;
    }
    t_device_buffer.m_pbuf_send 		    = buffer;
    t_device_buffer.m_pbuf_send_actual      = buffer;
    t_device_buffer.m_buf_send_size 	    = size;
    t_device_buffer.m_send_actual_size      = size;
    if (this->process_readwrite(DIR_WRITE, PHASE_PREPARE, t_device_buffer)){
    	API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
    	return -1;
    }
    if (t_device_buffer.m_pbuf_send_actual == NULL){
    	API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
        return -1;
    }
    actual_size = API_DeviceWrite(m_pdevice, pos, t_device_buffer.m_pbuf_send_actual, t_device_buffer.m_send_actual_size);
    if (this->process_readwrite(DIR_WRITE, PHASE_DONE, t_device_buffer)){
    	API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
    	return -1;
    }
    
    return actual_size;
}

//Note: read return m_len_data  if the cmd rsp no data, then read return 0!
portSSIZE_TYPE device::read(char *buffer, portSIZE_TYPE size)
{
	return read(0, buffer, size);
}

portSSIZE_TYPE device::write(char *buffer, portSIZE_TYPE size)
{
	return write(0, buffer, size);
}

portSSIZE_TYPE device::write(char *buffer)
{
	return write(0, buffer, strlen(buffer));
}

DeviceStatus_TYPE device::ioctl(uint8 cmd, void *args)
{
    if (NULL == m_pdevice){
        return DEVICE_ENULL;
    }
    return API_DeviceControl(m_pdevice, cmd, args);
}

DevicePoll_TYPE device::poll(void)
{
    if (NULL == m_pdevice){
        return DEVICE_POLL_ENULL;
    }
    return hal_poll(m_pdevice);
}

portBASE_TYPE device::process_readwrite(enum PROC_DIR dir, enum PROC_PHASE phase, struct device_buffer &device_buffer)
{
	if (DIR_READ == dir){
		switch (phase){
		case PHASE_PREPARE:

			break;
		case PHASE_DONE:

			break;

		default:
			break;
		}
	}else {
		switch (phase){
		case PHASE_PREPARE:

			break;
		case PHASE_DONE:

			break;

		default:
			break;
		}
	}

	return 0;
}


