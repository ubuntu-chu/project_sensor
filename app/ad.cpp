/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 
#include "ad.h"
//#include "stdio.h"

device_ad::device_ad(const char *pname, uint16 oflag):device(pname, oflag)
{
//    memset(m_data, 0, sizeof(m_data));
    m_len_recv        	    = sizeof(m_data);
	m_pbuf_recv		  	    = m_data;
	m_buf_recv_len 		    = m_len_recv;
}

device_ad::~device_ad()
{
    memset(m_data, 0, sizeof(m_data));
}

portBASE_TYPE device_ad::process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size)
{
	switch (phase){
	case PROC_PREPARE:
        device::process_read(phase, pbuf, size);
		break;

	case PROC_DONE:
		{

		}
		break;

	default:
		break;
	}
    
    return 0;
}

