/******************************************************************************
 *                        
******************************************************************************/ 
#include "pin.h"

device_pin::device_pin(const char *pname, uint16 oflag):device(pname, oflag)
{
}

device_pin::~device_pin()
{
}

portBASE_TYPE device_pin::process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size)
{
	switch (phase){
	case PROC_PREPARE:
		device::process_read(phase, pbuf, size);
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

