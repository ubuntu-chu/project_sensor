#ifndef _APP_STORAGE_
#define _APP_STORAGE_

#include "../includes/includes.h"
#include "devices.h"



class device_storage:public device{
public:
	device_storage():device(DEVICE_NAME_STORAGE)
	{

	}

    virtual ~device_storage();
};    
    
    













#endif


