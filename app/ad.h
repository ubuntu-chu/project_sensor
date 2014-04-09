#ifndef _APP_BATTERY_
#define _APP_BATTERY_

#include "../includes/includes.h"
#include "devices.h"



class CDevice_battery:public CDevice_base{
public:
    CDevice_battery(const char *pname, uint16 oflag);
    virtual ~CDevice_battery();
private:
    CDevice_battery(const CDevice_battery &other);
    CDevice_battery &operator =(const CDevice_battery &other);
	virtual portBASE_TYPE process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size);

    uint8           m_data[2];
};    
    
#endif


