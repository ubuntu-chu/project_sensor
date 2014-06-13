#ifndef _APP_PWM_
#define _APP_PWM_

#include "../includes/includes.h"
#include "devices.h"



class device_pwm:public device{
public:
    device_pwm(const char *pname, uint16 oflag);
    virtual ~device_pwm();
private:
    device_pwm(const device_pwm &other);
    device_pwm &operator =(const device_pwm &other);
};    
    
#endif


