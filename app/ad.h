#ifndef _APP_AD_
#define _APP_AD_

#include "../includes/includes.h"
#include "devices.h"



class device_ad:public device{
public:
	device_ad():device(DEVICE_NAME_AD)
	{

	}
    virtual ~device_ad();
	virtual portBASE_TYPE process_readwrite(enum PROC_DIR dir, enum PROC_PHASE phase, struct device_buffer &device_buffer);
};    
    
#endif


