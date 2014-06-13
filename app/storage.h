#ifndef _APP_STORAGE_
#define _APP_STORAGE_

#include "../includes/includes.h"
#include "devices.h"



class device_storage:public device{
public:
    device_storage(const char *pname, uint16 oflag);
    virtual ~device_storage();
private:
    device_storage(const device_storage &other);
    device_storage &operator =(const device_storage &other);
};    
    
    













#endif


