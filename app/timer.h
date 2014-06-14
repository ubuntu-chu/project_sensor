#ifndef _APP_TIMER_
#define _APP_TIMER_

#include "../includes/includes.h"
#include "devices.h"



class device_timer:public device{
public:
    device_timer(const char *pname, uint16 oflag);
    virtual ~device_timer();
private:
    device_timer(const device_timer &other);
    device_timer &operator =(const device_timer &other);
};    
    
    













#endif


