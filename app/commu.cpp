#include "commu.h"
#include "protocol.h"
#include "modbus.h"

CDevice_commu::CDevice_commu(const char *pname, 
                             uint16 oflag, 
                             class protocol *pprotocol, 
                             package_event_handler handler,
                             void *pvoid)
                            :CDevice_base(pname, oflag)
{
    m_handler			= handler;
    m_pvoid 			= pvoid;

    //protocol init
    m_pprotocol			= pprotocol;
    m_pprotocol->init();
}

CDevice_commu::~CDevice_commu()
{

}

portBASE_TYPE CDevice_commu::process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size)
{
	switch (phase){
	case PROC_PREPARE:
		CDevice_base::process_read(phase, pbuf, size);
		break;

	case PROC_DONE:

		if (0 != m_pprotocol->unpack(reinterpret_cast<uint8 *>(pbuf), size)) {
			API_DeviceControl(m_pdevice, COMMU_IOC_RX_ENTER, NULL);
            return -1;
		}
        m_len_data      = size;
		break;

	default:
		break;
	}

	return 0;
}

portBASE_TYPE CDevice_commu::process_write(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size)
{
	switch (phase){
	case PROC_PREPARE:
		{
			//format  send buff
			m_len_send = m_pprotocol->pack(reinterpret_cast<uint8 *>(m_pbuf_send), 
                                reinterpret_cast<uint8 *>(pbuf),size );
		}
		break;

	case PROC_DONE:
        API_DeviceControl(m_pdevice, COMMU_IOC_RX_ENTER, NULL);
		break;

	default:
		break;
	}

	return 0;
}

portBASE_TYPE CDevice_commu::package_event_fetch(void)
{
	portBASE_TYPE   rt;
    uint8           to_continue;
    
    do {
        rt = package_recv_handle(EVENT_CMD, 0, &to_continue);
    }while (to_continue == 1);
    
    return rt;
}

portBASE_TYPE CDevice_commu::package_recv_handle(uint8 event, 
                                                 uint8 func_code, 
                                                 uint8 *pcontinue)
{
	portSIZE_TYPE	buf_recv_len;
	uint8			buf[700];
	portBASE_TYPE 	rt			= 0;
	uint8			*pbuf		= buf; 
    uint8           to_continue = 0;

	API_DeviceControl(m_pdevice, (event == EVENT_ACK)?(DEVICE_IOC_BLOCK):(DEVICE_IOC_NONBLOCK), 0);
    buf_recv_len = read((char *)pbuf, sizeof(buf));
	if (event == EVENT_ACK){
		API_DeviceControl(m_pdevice, DEVICE_IOC_NONBLOCK, 0);
	}

	if (buf_recv_len == (portSIZE_TYPE)-1){
		rt								= (portBASE_TYPE)-1;
	}else {
        to_continue                     = 1;
		if (event == EVENT_ACK){
		}else {
        	//redirect  send buf
            m_pbuf_send                 = buf;
            //format send data to buf
            write((char *)pbuf, buf_recv_len);
            if (m_handler != NULL){
            	class modbus_rtu_info info;
            	if (0 == m_pprotocol->info(static_cast<const uint8 *>(pbuf), buf_recv_len, 
                    static_cast<class protocol_info *>(&info))){
					m_handler(m_pvoid, static_cast<class protocol_info *>(&info));
            	}
			}
		}
	}
    if (NULL != pcontinue){
        *pcontinue                             = to_continue;
    }

	return rt;
}




