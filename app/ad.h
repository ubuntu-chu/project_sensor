#ifndef _APP_AD_
#define _APP_AD_

#include "../includes/includes.h"
#include "devices.h"



class device_ad:public device{
public:
    device_ad(const char *pname, uint16 oflag);
    virtual ~device_ad();
private:
    device_ad(const device_ad &other);
    device_ad &operator =(const device_ad &other);
	virtual portBASE_TYPE process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size);

    uint8           m_data[2];
};    
    
#endif


