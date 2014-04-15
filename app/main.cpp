#include "main.h"
#include "devices.h"
#include "ad.h"
#include "pin.h"
#include "commu.h"
#include "storage.h"
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
    static CDevice_ad 		    device_ad(DEVICE_NAME_AD, DEVICE_FLAG_RDONLY);
    static CDevice_pin 			device_pin(DEVICE_NAME_PIN, DEVICE_FLAG_RDONLY);   
    static CDevice_storage 		device_storage(DEVICE_NAME_STORAGE, DEVICE_FLAG_RDWR);
    static CDevice_commu   		device_commu(DEVICE_NAME_COMMU, DEVICE_FLAG_RDWR, 
                                             &t_protocol_mac, 
                                             CApplication::package_event_handler);  
    //log setting
    Logger::setOutput(debug_output);
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
    m_app_runinfo.m_pdevice_ad 		        = &device_ad;
    m_app_runinfo.m_pdevice_commu 			= &device_commu;
    m_app_runinfo.m_pdevice_storage 	    = &device_storage;
    m_app_runinfo.m_pdevice_pin 			= &device_pin;
	m_app_runinfo.m_status 					= STAT_OK;

	m_app_runinfo.m_handle_period 			= t_monitor_manage.monitor_register(1000, 
                                                enum_MODE_PERIODIC, 
                                                period_handle, 
                                                this,
                                                "period handle");
	t_monitor_manage.monitor_start(m_app_runinfo.m_handle_period);  
    if (m_app_runinfo.m_pdevice_commu->open()){
		//SYS_LOG("commu device open failed\n");
    }
    if (m_app_runinfo.m_pdevice_pin->open()){
		//SYS_LOG("pin device open failed\n");
    }
    if (m_app_runinfo.m_pdevice_ad->open()){
		//SYS_LOG("pin device open failed\n");
    }
    if (m_app_runinfo.m_pdevice_storage->open()){
		//SYS_LOG("storage device open failed\n");
    }
    
    return 0;
}

portBASE_TYPE CApplication::run()
{
	CDevice_commu 		*pdevice_commu		= (CDevice_commu *)m_app_runinfo.m_pdevice_commu;
	CDevice_pin 	    *pdevice_pin	    = (CDevice_pin *)m_app_runinfo.m_pdevice_pin;
    uint8               key = 1;
    
    while(1){
        //pdevice_pin->read((char *)&key, sizeof(key)); 
        //key pressed   rf231 in recv mode
        if (!key){
            pdevice_commu->package_event_fetch();
        }else {
            uint8 buffer[100];
            uint8 len_buf[10];
            portSIZE_TYPE len = 100;
            portSIZE_TYPE len_len;
            
            pdevice_commu->write((char *)("wait for recv\n"));
            if (-1 == (len = pdevice_commu->read(reinterpret_cast<char *>(buffer), len))){
                
                pdevice_commu->write((char *)("wait for recv timeout\n"));
            }else {
                len_len = sprintf((char *)len_buf, "len = %d\n", len);
                pdevice_commu->write((char *)len_buf, len_len);
                pdevice_commu->write((char *)("buf = "));
                pdevice_commu->write((char *)buffer, len);
                pdevice_commu->write((char *)("\n"));
            }
            
            if (cpu_timetrig_1s()){
                
                
                //LOG_INFO << "now time is:" << cpu_sys_time_get();
#undef DEBUG_SWITCH
#define DEBUG_SWITCH  0
#if(DEBUG_SWITCH > 0)       
                {
                    uint8 buffer[100];
                    uint16 len;
                    
                    len  = sprintf((char *)buffer, "now time is: %ld\n", cpu_sys_time_get());
                    pdevice_commu->write(reinterpret_cast<char *>(buffer), len);
                    //SYS_LOG_LEV_TINY(SYS_LOG_LEV_NOTICE, buffer);
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

