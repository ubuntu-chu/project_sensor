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

#if 0
		if (0 != m_pprotocol->unpack(m_pbuf_recv, m_len_recv, &m_frm_ctl)) {
			return -1;
		}
		m_pbuf_data 		= m_frm_ctl.data_ptr;
		m_len_data 	        = m_frm_ctl.app_frm_ptr.len;
#endif
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
#if 0
		{
			frame_ctl_t *pframe_ctl  	= (frame_ctl_t *)pbuf;
			//format  send buff
			m_len_send = m_pprotocol->pack(&(pframe_ctl->mac_frm_ptr), &(pframe_ctl->app_frm_ptr),
							m_pbuf_send, pframe_ctl->data_ptr);
		}
#endif
		CDevice_base::process_write(phase, pbuf, size);
		break;

	case PROC_DONE:
		break;

	default:
		break;
	}

	return 0;
}

portBASE_TYPE CDevice_commu::package_send(uint8 func_code, mac_frm_ctrl_t frm_ctl, char *pbuf, uint16 len)
{
	frame_ctl_t t_frm_ctl;

	m_pprotocol->frm_ctl_init(&t_frm_ctl, 
                              frm_ctl, 
                              1, 
                              0, 
                              func_code, 
                              (uint8 *)pbuf, 
                              len);
	return write((char *)&t_frm_ctl, sizeof(frame_ctl_t));
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
	frame_ctl_t		*pframe_ctl;
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
        pframe_ctl						= &m_frm_ctl;
        SYS_LOG_LEV_TINY(SYS_LOG_LEV_DEBUG, "Net recv data:------------------------------------\n");
		SYS_LOG_LEV_TINY_LEN(SYS_LOG_LEV_DEBUG, pframe_ctl->data_ptr, pframe_ctl->app_frm_ptr.len);
		SYS_LOG_LEV_TINY(SYS_LOG_LEV_DEBUG, "\n");
		if (event == EVENT_ACK){
			//pbuf[0] result
			if ((pbuf[0] != 0) || (pframe_ctl->app_frm_ptr.fun != func_code)
				|| (pframe_ctl->mac_frm_ptr.ctl.ack_mask != 1)){
				rt						= (portBASE_TYPE)-2;
				if (pbuf[0] != 0){
					SYS_LOG_LEV_TINY(SYS_LOG_LEV_DEBUG, "Net recv:ack request not equal zero\n");
				}else if (pframe_ctl->app_frm_ptr.fun != func_code) {
					SYS_LOG_LEV_TINY(SYS_LOG_LEV_DEBUG, "Net recv:func code not equal\n");
				}else if (pframe_ctl->mac_frm_ptr.ctl.ack_mask != 1) {
					SYS_LOG_LEV_TINY(SYS_LOG_LEV_DEBUG, "Net recv:not ack frame\n");
				}
			}
		}else {
		//handle net evnet
        	if (pframe_ctl->mac_frm_ptr.type != c_modeinfo.dev_type_get()){
        		buf_recv[0]					= RSP_TYPE_ERR;
        		package_send_rsp(pframe_ctl->app_frm_ptr.fun, buf_recv, 1);
				SYS_LOG_LEV_TINY(SYS_LOG_LEV_DEBUG, "net pakage:dev type error\n");
        		rt						= (portBASE_TYPE)-3;
        	}else if (m_handler != NULL){
				m_handler(pframe_ctl, pframe_ctl->app_frm_ptr.fun, pbuf, buf_recv_len);
			}
		}
	}
    if (NULL != pcontinue){
        *pcontinue                             = to_continue;
    }

	return rt;
}

//respond frame
portBASE_TYPE CDevice_commu::package_send_rsp(uint8 func_code, uint8 *prsp, uint16 len)
{
	mac_frm_ctrl_t  t_mac_frm_ctrl;
    
    m_pprotocol->mac_frm_ctrl_init(&t_mac_frm_ctrl, ACK_FRAME, 
                                        UPSTREAM, 
                                        NO_ACK_REQUEST, 
                                        FRAME_TYPE_DATA);
    return package_send(func_code, 
                        t_mac_frm_ctrl, 
                        (char *)prsp, 
                        len);
}

portBASE_TYPE CDevice_commu::package_send_status(char *pbuf, uint16 len)
{
	uint8	func_code			= def_FUNC_CODE_HEARBEAT;
    mac_frm_ctrl_t  t_mac_frm_ctrl;
    
    m_pprotocol->mac_frm_ctrl_init(&t_mac_frm_ctrl, ACK_FRAME, 
                                        UPSTREAM, 
                                        ACK_REQUEST, 
                                        FRAME_TYPE_DATA); 

	return package_send(func_code, 
                        t_mac_frm_ctrl, 
                        (char *)pbuf, 
                        len);
	//wait for answer
	//return package_recv_handle(EVENT_ACK, func_code);
}



