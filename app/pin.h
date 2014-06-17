#ifndef _APP_PIN_
#define _APP_PIN_

#include "../includes/includes.h"
#include "devices.h"



class device_pin:public device{
public:
	device_pin():device(DEVICE_NAME_PIN)
	{

	}

    virtual ~device_pin();
};    
    
    













#endif


