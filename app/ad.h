#ifndef _APP_AD_
#define _APP_AD_

#include    "../includes/includes.h"
#include    "devices.h"
#include 	"../bsp/driver/drv_interface.h"


class device_ad:public device{
public:
	device_ad();
    virtual ~device_ad();
	virtual enum PROC_CMD_STAT process_command(enum PROC_CMD dir, enum PROC_PHASE phase, struct device_buffer &device_buffer);
private:
	uint8 						m_ioc;
#ifndef		def_AVARAGE_IN_READ
    int32                       m_buffer[def_AD_SAMPLE_LEN];
#endif
};    
    
#endif


