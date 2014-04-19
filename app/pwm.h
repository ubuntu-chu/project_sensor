#ifndef _APP_PWM_
#define _APP_PWM_

#include "../includes/includes.h"
#include "devices.h"



class CDevice_pwm:public CDevice_base{
public:
    CDevice_pwm(const char *pname, uint16 oflag);
    virtual ~CDevice_pwm();
private:
    CDevice_pwm(const CDevice_pwm &other);
    CDevice_pwm &operator =(const CDevice_pwm &other);
};    
    
#endif


