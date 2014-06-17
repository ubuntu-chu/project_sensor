#ifndef _APP_EVENT_H
#define _APP_EVENT_H

#include "../includes/includes.h"
#include "devices.h"



class device_event:public device{
public:
	device_event():device(DEVICE_NAME_EVENT)
	{

	}

    virtual ~device_event();
};    
    
    













#endif


