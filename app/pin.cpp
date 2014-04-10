/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 
#include "pin.h"

CDevice_pin::CDevice_pin(const char *pname, uint16 oflag):CDevice_base(pname, oflag)
{
}

CDevice_pin::~CDevice_pin()
{
}

portBASE_TYPE CDevice_pin::process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size)
{
	switch (phase){
	case PROC_PREPARE:
		CDevice_base::process_read(phase, pbuf, size);
		break;

	case PROC_DONE:
	#if(DEBUG_SWITCH > 0)
		{
		uint8			buffer[50];
		//sprintf((char *)buffer, "pin value:%d\n", m_pbuf_data[0]);
		SYS_LOG_LEV_TINY(SYS_LOG_LEV_NOTICE, buffer);
		}
	#endif
		break;

	default:
		break;
	}

	return 0;
}

