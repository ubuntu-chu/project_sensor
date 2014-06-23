#ifndef _DRV_COMMU_H_
#define _DRV_COMMU_H_

#ifndef    _INCLUDES_H_
	#include    "../../../includes/includes.h"
#endif


#define     DEVICE_NAME_COMMU                       ("COMMU")

#define		COMMU_IOC_RX_ENTER				        (DEVICE_IOC_USER+1)
#define		COMMU_IOC_BAUD				        	(DEVICE_IOC_USER+2)



extern portuBASE_TYPE drv_commuregister(void);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif
