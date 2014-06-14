#ifndef _APP_EVENT_H
#define _APP_EVENT_H

#include "../includes/includes.h"
#include "devices.h"



class device_event:public device{
public:
    device_event(const char *pname, uint16 oflag);
    virtual ~device_event();
private:
    device_event(const device_event &other);
    device_event &operator =(const device_event &other);
};    
    
    













#endif


