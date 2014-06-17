#ifndef _APP_PWM_
#define _APP_PWM_

#include "../includes/includes.h"
#include "devices.h"



class device_pwm:public device{
public:
	device_pwm():device(DEVICE_NAME_PWM)
	{

	}
    virtual ~device_pwm();
};    
    
#endif


