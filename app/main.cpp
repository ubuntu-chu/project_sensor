#include "main.h"
#include "devices.h"
#include "ad.h"
#include "pin.h"
#include "../api/log/log.h"

#if 1
void debug_output(const char* msg, int len)
{
	uart_tx(DBG_UART, (uint8 *)msg, len);
}

CApplication 	*CApplication::m_pcapplicaiton = NULL;

CApplication *CApplication::GetInstance(void)
{
	static CApplication 	c_application;
  
    if (NULL == m_pcapplicaiton){
        m_pcapplicaiton         = &c_application;
    }
    
    return (m_pcapplicaiton);
}

portBASE_TYPE CApplication::package_event_handler(frame_ctl_t *pframe_ctl, uint8 func_code, uint8 *pbuf, uint16 len)
{
	uint8			rsp						= RSP_OK;
    uint8           fliter                  = 0;
    CApplication  	*pcapplication    		= CApplication::GetInstance();

#if 0    
    //key pressed   rf231 in recv mode
    if (pframe_ctl->mac_frm_ptr.ctl.ack_mask == 1){
		return 0;
	}else if(func_code != def_FUNC_CODE_SET){
		fliter              				= 1;
		rsp									= RSP_EXEC_FAILURE;
	}
	if (1 == fliter){
		pdevice_rf231->package_send_rsp(func_code, &rsp, sizeof(rsp));
		return 0;
	}
	switch (func_code) {
		case def_FUNC_CODE_SET:
			pdevice_rf231->package_send_rsp(pframe_ctl->app_frm_ptr.fun, &rsp, sizeof(rsp));
			break;
		default:
			rsp					= RSP_INVALID_CMD;
			pdevice_rf231->package_send_rsp(func_code, &rsp, sizeof(rsp));
			break;
	}
#endif
	return 0;
}

portBASE_TYPE CApplication::init()
{
    static CDevice_battery 		device_battery(DEVICE_NAME_AD, DEVICE_FLAG_RDONLY);
    static CDevice_pin 			device_pin(DEVICE_NAME_PIN, DEVICE_FLAG_RDONLY);
#if 0    
    static CDevice_commu   		device_commu(DEVICE_NAME_COMMU, DEVICE_FLAG_RDWR, 
                                             &t_protocol_mac, 
                                             CApplication::package_event_handler);
#endif    
    //log setting
   // Logger::setOutput(debug_output);
#if 0
    {
        struct tm   tm_time;
        time_t      time;
        
        tm_time.tm_year                     = 2014-1900;
        tm_time.tm_mon                      = 4; 
        tm_time.tm_mday                     = 1;
        tm_time.tm_hour                     = 17; 
        tm_time.tm_min                      = 20; 
        tm_time.tm_sec                      = 50;
        time        = mktime(&tm_time);
        cpu_sys_time_set(time);
    }
#endif
    //device setting
    m_app_runinfo.m_pdevice_battery 		= &device_battery;
  //  m_app_runinfo.m_pdevice_rf231 			= &device_rf231;
    m_app_runinfo.m_pdevice_pin 			= &device_pin;
	m_app_runinfo.m_status 					= STAT_OK;

	m_app_runinfo.m_handle_period 			= t_monitor_manage.monitor_register(1000, enum_MODE_PERIODIC, period_handle, this);
	t_monitor_manage.monitor_start(m_app_runinfo.m_handle_period);
#if 0    
    if (m_app_runinfo.m_pdevice_commu->open()){
		//SYS_LOG("commu device open failed\n");
    }
#endif
    if (m_app_runinfo.m_pdevice_pin->open()){
		//SYS_LOG("pin device open failed\n");
    }
#if 0 
    if (m_app_runinfo.m_pdevice_storage->open()){
		//SYS_LOG("storage device open failed\n");
    }
#endif
    
    return 0;
}

portBASE_TYPE CApplication::run()
{
	//CDevice_rf231 		*pdevice_commu		= (CDevice_rf231 *)m_app_runinfo.m_pdevice_commu;
	CDevice_pin 	    *pdevice_pin	    = (CDevice_pin *)m_app_runinfo.m_pdevice_pin;
    uint8               key;
    
    while(1){
        pdevice_pin->read((char *)&key, sizeof(key)); 
        //key pressed   rf231 in recv mode
        if (!key){
      //      pdevice_rf231->package_event_fetch();
        }else {
            if (cpu_timetrig_1s()){
                
                LOG_INFO << "now time is:" << cpu_sys_time_get();
#if(DEBUG_SWITCH > 0)       
                {
                    uint8 buffer[100];
                    sprintf((char *)buffer, "now time is: %ld\n", cpu_sys_time_get());
                    // SYS_LOG_LEV_TINY(SYS_LOG_LEV_NOTICE, buffer);
                }
#endif
            }
        }
    };
    
    return 0;
}

void CApplication::period_handle(void *pdata)
{
	CApplication *papplication  = static_cast<CApplication *> (pdata);
    
    cpu_led_toggle();
//	t_monitor_manage.monitor_start(papplication->m_app_runinfo.m_handle_period);
}
#endif

int main(void)
{
    CApplication  *pcapplication    = CApplication::GetInstance();
    
    //bsp startup
    bsp_startup();
    pcapplication->init();
    pcapplication->run();

    return 0;
}

