#ifndef _APP_PIN_
#define _APP_PIN_

#include "../includes/includes.h"
#include "devices.h"



class device_pin:public device{
public:
    device_pin(const char *pname, uint16 oflag);
    virtual ~device_pin();
private:
    device_pin(const device_pin &other);
    device_pin &operator =(const device_pin &other);
	virtual portBASE_TYPE process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size);
};    
    
    













#endif


