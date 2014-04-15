#include "commu.h"
#include "protocol.h"

CDevice_commu::CDevice_commu(const char *pname, 
                             uint16 oflag, 
                             class protocol *pprotocol, 
                             package_event_handler handler)
                            :CDevice_base(pname, oflag)
{
	m_pbuf_recv			= m_buffrecv;
	m_pbuf_send			= m_buffsend;
	//buf recv max len
    m_buf_recv_len      = sizeof(m_buffrecv);

    m_handler			= handler;

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
#if 1
		{
			//format  send buff
			m_len_send = m_pprotocol->pack(reinterpret_cast<uint8 *>(m_pbuf_send), 
                                reinterpret_cast<uint8 *>(pbuf),size );
		}
#endif
		//CDevice_base::process_write(phase, pbuf, size);
		break;

	case PROC_DONE:
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
	uint8			buf_recv[300];
	portBASE_TYPE 	rt			= 0;
	uint8			*pbuf		= buf_recv;
    uint8           to_continue = 0;

	API_DeviceControl(m_pdevice, (event == EVENT_ACK)?(DEVICE_IOC_BLOCK):(DEVICE_IOC_NONBLOCK), 0);
	buf_recv_len = read((char *)pbuf, m_buf_recv_len);
	if (event == EVENT_ACK){
		API_DeviceControl(m_pdevice, DEVICE_IOC_NONBLOCK, 0);
	}

	if (buf_recv_len == (portSIZE_TYPE)-1){
		rt								= (portBASE_TYPE)-1;
	}else {
        to_continue                     = 1;
		if (event == EVENT_ACK){
		}else {
        	write((char *)pbuf, m_buf_recv_len);
            if (m_handler != NULL){
				m_handler(0, pbuf, buf_recv_len);
			}
		}
	}
    if (NULL != pcontinue){
        *pcontinue                             = to_continue;
    }

	return rt;
}



