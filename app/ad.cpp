/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 
#include "ad.h"
//#include "stdio.h"

CDevice_battery::CDevice_battery(const char *pname, uint16 oflag):CDevice_base(pname, oflag)
{
//    memset(m_data, 0, sizeof(m_data));
    m_len_recv        	 = sizeof(m_data);
	m_pbuf_recv		  	 = m_data;
	m_buf_recv_len 		= m_len_recv;
}

CDevice_battery::~CDevice_battery()
{
    memset(m_data, 0, sizeof(m_data));
}

portBASE_TYPE CDevice_battery::process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size)
{
	switch (phase){
	case PROC_PREPARE:
		break;

	case PROC_DONE:
		{
			uint32 			volt;

			m_pbuf_data        = m_pbuf_recv;
			volt               = ((uint16)m_pbuf_data[0])<<8;
			volt               += m_pbuf_data[1];
		#if(DEBUG_SWITCH > 0)
			{
			uint8			buffer[50];
			sprintf((char *)buffer, "ad value:%d\n", volt);
			SYS_LOG_LEV_TINY(SYS_LOG_LEV_NOTICE, buffer);
			}
		#endif
			volt               = (volt*100*c_modeinfo.battery_volt_get()/4095)/100;
			m_pbuf_data[0]     = volt;
			m_len_data         = 1;
		#if(DEBUG_SWITCH > 0)
			{
			uint8			buffer[50];
			sprintf((char *)buffer, "volt value:%d\n", volt);
			SYS_LOG_LEV_TINY(SYS_LOG_LEV_NOTICE, buffer);
			}
		#endif
		}
		break;

	default:
		break;
	}
    
    return 0;
}

