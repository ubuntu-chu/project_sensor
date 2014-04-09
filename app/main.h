#ifndef _MAIN_H_
#define _MAIN_H_

#include "../includes/includes.h"
#include "devices.h"
#include "protocol.h"

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
    CDevice_base        *m_pdevice_rf231;
    CDevice_base        *m_pdevice_battery;
    CDevice_base        *m_pdevice_log;
    CDevice_base        *m_pdevice_pin;
    CDevice_base        *m_pdevice_24c02;
	uint8				m_status;
	uint8 				m_mode;

	monitor_handle_type m_handle_period;

};
typedef struct _app_runinfo_ app_runinfo_t;

class CApplication{
public:
    static CApplication *GetInstance(void);  
    portBASE_TYPE run(void);
    portBASE_TYPE init(void);
    
    void mode_set(uint8 mode){m_app_runinfo.m_mode = mode;}
    uint8 mode_get(void){return m_app_runinfo.m_mode;}

private:
    CApplication(){}
    ~CApplication(){}
    CApplication(const CApplication &other);
    CApplication &operator =(const CApplication &other);
	portBASE_TYPE card_status_send(void);
	static portBASE_TYPE package_event_handler(frame_ctl_t *pframe_ctl, uint8 func_code, uint8 *pbuf, uint16 len);

	static void period_handle(void *pdata);


    static CApplication 	*m_pcapplicaiton;
    struct _app_runinfo_    m_app_runinfo;
};
    













#endif


