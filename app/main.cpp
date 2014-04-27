#include    "main.h"
#include    "devices.h"
#include    "ad.h"
#include    "pwm.h"
#include    "pin.h"
#include    "commu.h"
#include    "storage.h"
#include    "../api/log/log.h"


#if 1
void debug_output(const char* msg, int len)
{
	uart_tx(DBG_UART, (uint8 *)msg, len);
}

CApplication 	*CApplication::m_pcapplicaiton = NULL;

CApplication *CApplication::GetInstance(void)
{
	static CApplication 	t_application;
  
    if (NULL == m_pcapplicaiton){
        m_pcapplicaiton         = &t_application;
    }
    
    return (m_pcapplicaiton);
}

portBASE_TYPE CApplication::package_event_handler(uint8 func_code, uint8 *pbuf, uint16 len)
{
    CApplication  	*pcapplication    		= CApplication::GetInstance();

    switch (func_code){
    case _FC_WRITE_SINGLE_REGISTER: 
    case _FC_WRITE_MULTIPLE_REGISTERS:
    {
        //write hold regs to storage device
        CDevice_storage *pdevice_storage    = 
            (CDevice_storage *)(pcapplication->m_app_runinfo.m_pdevice_storage);
        
        pdevice_storage->write((char *)&(pcapplication->m_modeinfo.m_regs), sizeof(struct regs));
    }
        break;
    
    default:
        break;
    }
    
	return 0;
}

volatile CDevice_commu *pCDevice_commu;

portBASE_TYPE CApplication::load_env_datum(void)
{
    CDevice_storage *pdevice_storage    = 
            static_cast<CDevice_storage *>(m_app_runinfo.m_pdevice_storage);
    
    pdevice_storage->read((char *)&m_modeinfo.m_regs, sizeof(struct regs));
    
    
    
    return 0;
}

uint16  CApplication::hold_reg_get(enum hold_reg_index index)
{
    return m_modeinfo.hold_reg_get(index);
}
void CApplication::hold_reg_set(enum hold_reg_index index, uint16 value)
{
    m_modeinfo.hold_reg_set(index, value);
}

portBASE_TYPE CApplication::init(void)
{
    static CDevice_ad 		    t_device_ad(DEVICE_NAME_AD, DEVICE_FLAG_RDONLY);
    static CDevice_pin 			t_device_pin(DEVICE_NAME_PIN, DEVICE_FLAG_RDONLY);   
    static CDevice_storage 		t_device_storage(DEVICE_NAME_STORAGE, DEVICE_FLAG_RDWR);
    static CDevice_pwm		    t_device_pwm(DEVICE_NAME_PWM, DEVICE_FLAG_RDONLY);
    static CDevice_commu   		t_device_commu(DEVICE_NAME_COMMU, DEVICE_FLAG_RDWR, 
                                             &t_protocol_modbus_rtu,
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
    m_app_runinfo.m_pdevice_pwm             = &t_device_pwm;
    m_app_runinfo.m_pdevice_ad 		        = &t_device_ad;
    m_app_runinfo.m_pdevice_commu 			= &t_device_commu;
    m_app_runinfo.m_pdevice_storage 	    = &t_device_storage;
    m_app_runinfo.m_pdevice_pin 			= &t_device_pin;
	m_app_runinfo.m_status 					= STAT_OK;
	m_modeinfo.name_set(def_MODEL_NAME);
    pCDevice_commu= &t_device_commu;

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
    if (m_app_runinfo.m_pdevice_pwm->open()){
		//SYS_LOG("pin device open failed\n");
    }
    if (m_app_runinfo.m_pdevice_storage->open()){
		//SYS_LOG("storage device open failed\n");
    }
    //load env_datum from storage device
    if (load_env_datum()){
        //SYS_LOG("load_env_datum failed\n");
    }
    t_protocol_modbus_rtu.slave_set(hold_reg_get(enum_REG_MODBUS_ADDR));
    t_protocol_modbus_rtu.modbus_mapping_set(def_NR_TAB_BITS,
                                        def_NR_TAB_INPUT_BITS,
                                        def_NR_TAB_INPUT_REGS,
                                        def_NR_TAB_REGS,
                                        m_modeinfo.m_regs.m_tab_bits,
                                        m_modeinfo.m_regs.m_tab_input_bits,
                                        m_modeinfo.m_regs.m_tab_input_registers,
                                        m_modeinfo.m_regs.m_tab_registers);
    memset(m_modeinfo.m_regs.m_tab_input_registers, 0x55, def_NR_TAB_INPUT_REGS);
    memset(m_modeinfo.m_regs.m_tab_registers, 0x55, def_NR_TAB_REGS);
    
    hold_reg_set(enum_REG_RHREF_RREQ, 200);
    hold_reg_set(enum_REG_RHREF_DUTY_CYCLE, 150);
    hold_reg_set(enum_REG_HEAT_RREQ, 400);
    hold_reg_set(enum_REG_HEAT_DUTY_CYCLE, 150);
    
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_RHREF_FORCE_H, NULL);
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_RHREF_FORCE_L, NULL);
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_HEAT_FORCE_H, NULL);
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_HEAT_FORCE_L, NULL);
    
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_RHREF_FREQ, 
            (void *)static_cast<uint32>(hold_reg_get(enum_REG_RHREF_RREQ)));
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_RHREF_DUTY_CYCLE, 
            (void *)static_cast<uint32>(hold_reg_get(enum_REG_RHREF_DUTY_CYCLE)));
        
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_HEAT_FREQ, 
            (void *)static_cast<uint32>(hold_reg_get(enum_REG_HEAT_RREQ)));
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_HEAT_DUTY_CYCLE, 
            (void *)static_cast<uint32>(hold_reg_get(enum_REG_HEAT_DUTY_CYCLE)));
    
    return 0;
}

portBASE_TYPE CApplication::run()
{
	CDevice_commu 		*pdevice_commu		= (CDevice_commu *)m_app_runinfo.m_pdevice_commu;
	CDevice_pin 	    *pdevice_pin	    = (CDevice_pin *)m_app_runinfo.m_pdevice_pin;
    
    while(1){
        //pdevice_pin->read((char *)&key, sizeof(key)); 
        //key pressed   rf231 in recv mode
        pdevice_commu->package_event_fetch();
#if 0
        uint8 buffer[100];
        uint8 len_buf[10];
        portSIZE_TYPE len = 100;
        portSIZE_TYPE len_len;
        
        //pdevice_commu->write((char *)("wait for recv\n"));
        if (-1 == (len = pdevice_commu->read(reinterpret_cast<char *>(buffer), len))){
            
//                 pdevice_commu->write((char *)("wait for recv timeout\n"));
        }else {
               len_len = sprintf((char *)len_buf, "len = %d\n", len);
//                 pdevice_commu->write((char *)len_buf, len_len);
//                 pdevice_commu->write((char *)("buf = "));
//                 pdevice_commu->write((char *)buffer, len);
//                 pdevice_commu->write((char *)("\n"));
        }
  #endif      
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
    };
    
    return 0;
}

void CApplication::period_handle(void *pdata)
{
	CApplication *papplication  = static_cast<CApplication *> (pdata);
    (void)papplication;
    
    cpu_led_toggle();
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

