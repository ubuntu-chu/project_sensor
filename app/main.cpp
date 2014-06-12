#include    "main.h"
#include    "devices.h"
#include    "ad.h"
#include    "pwm.h"
#include    "pin.h"
#include    "commu.h"
#include    "storage.h"
#include    "../api/log/log.h"
#include	"../reactor/reactor.h"


#if 1
void debug_output(const char* msg, int len)
{
	//uart_tx(DBG_UART, (uint8 *)msg, len);
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

portBASE_TYPE CApplication::package_event_handler(void *pvoid, class protocol_info *pinfo)
{
    CApplication  	*pcapplication    		= static_cast<CApplication *>(pvoid);
    class modbus_rtu_info *pmodbus_rtu_info;
    struct storage_info storage_info;
    uint16 	offset;

	pmodbus_rtu_info = static_cast<class modbus_rtu_info *>(pinfo);

	if (pmodbus_rtu_info->type_get() != enum_MODBUS_RTU){
		return -1;
	}

    switch (pmodbus_rtu_info->function_get()){
    case _FC_WRITE_SINGLE_REGISTER: 
    case _FC_WRITE_MULTIPLE_REGISTERS:
    {
        //write hold regs to storage device
        CDevice_storage *pdevice_storage    = 
            static_cast<CDevice_storage *>(pcapplication->m_app_runinfo.m_pdevice_storage);
        
        //hold regs type is uint16
        offset 								= pmodbus_rtu_info->reg_get()<<1;
        pcapplication->m_modeinfo.storage_info_query(enum_REG_TYPE_HOLD, &storage_info);
        pdevice_storage->write(storage_info.m_storage_addr+offset,
        		(char *)(storage_info.m_pdata + offset),
        		storage_info.m_len<<1);
    }
        break;
    
    default:
        break;
    }
    
	return 0;
}

volatile CDevice_commu *pCDevice_commu;

portBASE_TYPE CApplication::load_app_datum(void)
{
    uint8               magic_string[100];
    portSSIZE_TYPE      ssize;
    struct storage_info storage_info_magic;
    struct storage_info storage_info_modbus;
    CDevice_storage *pdevice_storage    = 
            static_cast<CDevice_storage *>(m_app_runinfo.m_pdevice_storage);
    
    m_modeinfo.storage_info_query(enum_REG_TYPE_MAGIC, &storage_info_magic);
while (1){  
#if 0    
    ssize    = pdevice_storage->read(storage_info_magic.m_storage_addr, 
                    reinterpret_cast<char *>(magic_string), 
                    storage_info_magic.m_len);
    
#endif    
    ssize    = pdevice_storage->write(storage_info_magic.m_storage_addr, 
                    reinterpret_cast<char *>(storage_info_magic.m_pdata), 
                    storage_info_magic.m_len);

    ssize    = pdevice_storage->read(storage_info_magic.m_storage_addr, 
                    reinterpret_cast<char *>(magic_string), 
                    storage_info_magic.m_len);
    delay_ms(1000);
}    
    if ((ssize <= 0) || (ssize != storage_info_magic.m_len)){
        LOG_FATAL << "magic load failed!";
        return -1;
    }
    m_modeinfo.storage_info_query(enum_REG_TYPE_HOLD, &storage_info_modbus);
    //first load   do init - modbus regs
    if (strcmp(reinterpret_cast<const char *>(magic_string), 
                reinterpret_cast<const char *>(storage_info_magic.m_pdata))){
        
        ssize   = pdevice_storage->write(storage_info_modbus.m_storage_addr,
                                reinterpret_cast<char *>(storage_info_modbus.m_pdata), 
                                storage_info_modbus.m_len);
    }else {
        //load modbus regs
        ssize   = pdevice_storage->read(storage_info_modbus.m_storage_addr,
                                reinterpret_cast<char *>(storage_info_modbus.m_pdata), 
                                storage_info_modbus.m_len);
    }
    if ((ssize <= 0) || (ssize != storage_info_modbus.m_len)){
        LOG_FATAL << "modbus regs load/write failed!";
        return -1;
    }
    
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

uint16  CApplication::input_reg_get(enum input_reg_index index)
{
    return m_modeinfo.input_reg_get(index);
}
void CApplication::input_reg_set(enum input_reg_index index, uint16 value)
{
    m_modeinfo.input_reg_set(index, value);
}

portBASE_TYPE CApplication::init(void)
{
    static CDevice_ad 		    t_device_ad(DEVICE_NAME_AD, DEVICE_FLAG_RDONLY);
    static CDevice_pin 			t_device_pin(DEVICE_NAME_PIN, DEVICE_FLAG_RDONLY);   
    static CDevice_storage 		t_device_storage(DEVICE_NAME_STORAGE, DEVICE_FLAG_RDWR);
    static CDevice_pwm		    t_device_pwm(DEVICE_NAME_PWM, DEVICE_FLAG_RDONLY);
    static CDevice_commu   		t_device_commu(DEVICE_NAME_COMMU, DEVICE_FLAG_RDWR, 
                                             &t_protocol_modbus_rtu,
                                             CApplication::package_event_handler,
                                             this);
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
	cpu_pendsv_register(pendsv_handle, this);
    
    m_app_runinfo.m_pdevice_commu->open();
    m_app_runinfo.m_pdevice_pin->open();
    m_app_runinfo.m_pdevice_ad->open();
    m_app_runinfo.m_pdevice_pwm->open();
    m_app_runinfo.m_pdevice_storage->open();
    
    //load conf_datum from storage device
#if 0
    if (load_app_datum()){
    }
#endif
    t_protocol_modbus_rtu.slave_set(hold_reg_get(enum_REG_MODBUS_ADDR));
    t_protocol_modbus_rtu.modbus_mapping_set(ARRAY_SIZE(m_modeinfo.m_regs.m_tab_bits),
                                        ARRAY_SIZE(m_modeinfo.m_regs.m_tab_input_bits),
                                        ARRAY_SIZE(m_modeinfo.m_regs.m_tab_input_registers),
                                        ARRAY_SIZE(m_modeinfo.m_regs.m_tab_registers),
                                        m_modeinfo.m_regs.m_tab_bits,
                                        m_modeinfo.m_regs.m_tab_input_bits,
                                        m_modeinfo.m_regs.m_tab_input_registers,
                                        m_modeinfo.m_regs.m_tab_registers);
    
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
	CDevice_commu *pdevice_commu =
			(CDevice_commu *) m_app_runinfo.m_pdevice_commu;
	CDevice_pin *pdevice_pin = (CDevice_pin *) m_app_runinfo.m_pdevice_pin;
	CDevice_ad *pdevice_ad =
			static_cast<CDevice_ad *>(m_app_runinfo.m_pdevice_ad);
	portDEVHANDLE_TYPE handle_ad = pdevice_ad->handle_get();

	eventloop t_loop;
	channel t_channel_ad(&t_loop, handle_ad);
	channel t_channel_pin(&t_loop, pdevice_pin->handle_get());

	t_channel_ad.enableReading();
	t_channel_pin.enableReading();
	t_channel_pin.disableReading();
	t_loop.loop();

	return 0;
}

//intertup context 
void CApplication::pendsv_handle(void *pdata)
{
	CApplication *papplication      = static_cast<CApplication *> (pdata);
	CDevice_commu *pdevice_commu    = static_cast<CDevice_commu *>(papplication->m_app_runinfo.m_pdevice_commu);

	pdevice_commu->package_event_fetch();
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



