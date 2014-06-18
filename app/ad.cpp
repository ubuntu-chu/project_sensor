/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 
#include "ad.h"
//#include "stdio.h"


device_ad::~device_ad()
{
}


portBASE_TYPE device_ad::process_readwrite(enum PROC_DIR dir, enum PROC_PHASE phase, struct device_buffer &device_buffer)
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
		device::process_readwrite(dir, phase, device_buffer);
	}

	return 0;
}



