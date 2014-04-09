#ifndef _APP_PIN_
#define _APP_PIN_

#include "../includes/includes.h"
#include "devices.h"



class CDevice_pin:public CDevice_base{
public:
    CDevice_pin(const char *pname, uint16 oflag);
    virtual ~CDevice_pin();
private:
    CDevice_pin(const CDevice_pin &other);
    CDevice_pin &operator =(const CDevice_pin &other);
	virtual portBASE_TYPE process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size);
};    
    
    













#endif


