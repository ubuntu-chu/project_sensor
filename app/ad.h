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
};    
    
#endif


