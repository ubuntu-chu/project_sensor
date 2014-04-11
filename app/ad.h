#ifndef _APP_AD_
#define _APP_AD_

#include "../includes/includes.h"
#include "devices.h"



class CDevice_ad:public CDevice_base{
public:
    CDevice_ad(const char *pname, uint16 oflag);
    virtual ~CDevice_ad();
private:
    CDevice_ad(const CDevice_ad &other);
    CDevice_ad &operator =(const CDevice_ad &other);
	virtual portBASE_TYPE process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size);

    uint8           m_data[2];
};    
    
#endif


