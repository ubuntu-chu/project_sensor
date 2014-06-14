#ifndef _MAIN_H_
#define _MAIN_H_

#include "../includes/includes.h"
#if 1
#include "devices.h"
#include "protocol.h"
#include "modbus.h"
#include "../app/env_datum.h"

enum{
	STAT_OK				= 0,
	STAT_RF231_OPEN,
	STAT_RF231_ACCESS,
	STAT_BATTERY_OPEN,
	STAT_BATTERY_ACCESS,
	STAT_PIN_OPEN,
	STAT_PIN_ACCESS,
	STAT_24C02_OPEN,
	STAT_24C02_ACCESS,
};

enum{
	MODE_IDLE_ENTER 	= 0,
	MODE_IDLE_EXIT,
};

struct _app_runinfo_{
    device        *m_pdevice_commu;
    device        *m_pdevice_ad;
    device        *m_pdevice_log;
    device        *m_pdevice_pin;
    device        *m_pdevice_storage;
    device        *m_pdevice_pwm;
	uint8				m_status;
	uint8 				m_mode;

	timer_handle_type m_handle_period;

};
typedef struct _app_runinfo_ app_runinfo_t;

class application{
    DEFINE_SINGLETON_CLASS(application)
public:
    portBASE_TYPE run(void);
    portBASE_TYPE init(void);
    
    void mode_set(uint8 mode){m_app_runinfo.m_mode = mode;}
    uint8 mode_get(void){return m_app_runinfo.m_mode;}

private:
    application(){}
    ~application(){}
    application(const application &other);
    application &operator =(const application &other);
	static portBASE_TYPE package_event_handler(void *pvoid, class protocol_info *pinfo);
	static void pendsv_handle(void *pdata);
	static void period_handle(void *pdata);
        
    portBASE_TYPE load_app_datum(void);
    uint16  hold_reg_get(enum hold_reg_index index);
    void hold_reg_set(enum hold_reg_index index, uint16 value);

    uint16  input_reg_get(enum input_reg_index index);
    void input_reg_set(enum input_reg_index index, uint16 value);

    struct _app_runinfo_    m_app_runinfo;
	class CModelInfo 	     m_modeinfo;
};
    
#endif












#endif


