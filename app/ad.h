#ifndef _APP_AD_
#define _APP_AD_

#include "../includes/includes.h"
#include "devices.h"



class device_ad:public device{
public:
	device_ad();
    virtual ~device_ad();
	virtual portBASE_TYPE process_command(enum PROC_CMD dir, enum PROC_PHASE phase, struct device_buffer &device_buffer);
private:
	uint8 						m_stage;
    int32                       m_buffer[def_AD_SAMPLE_LEN];
};    
    
#endif


