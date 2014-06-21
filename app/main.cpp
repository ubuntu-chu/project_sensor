#include    "main.h"
#include    "devices.h"
#include    "ad.h"
#include    "pwm.h"
#include    "pin.h"
#include    "commu.h"
#include    "storage.h"
#include    "modbus.h"
#include    "../api/log/log.h"
#include 	"../bsp/driver/drv_interface.h"

void debug_output(const char* msg, int len)
{
	//uart_tx(DBG_UART, (uint8 *)msg, len);
}

portBASE_TYPE application::package_event_handler(void *pvoid, enum protocol_phase phase, class protocol_info *pinfo)
{
    portBASE_TYPE    rt                             = 0;
    application  	 *papplication    		        = static_cast<application *>(pvoid);
    class modbus_rtu_info *pmodbus_rtu_info;
    uint16 	         reg, reg_no;
    uint8           *buffer;
    int             function;
	uint32 			i;

	pmodbus_rtu_info = static_cast<class modbus_rtu_info *>(pinfo);

    if (pmodbus_rtu_info->type_get() != enum_MODBUS_RTU){
		return -1;
	}
    function                                        = pmodbus_rtu_info->function_get();
    reg                                             = pmodbus_rtu_info->reg_get();
    reg_no                                          = pmodbus_rtu_info->reg_no_get();
    buffer                                          = pmodbus_rtu_info->param_addr_get();
    //准备协议所需的数据
    if (enum_PROTOCOL_PREPARE == phase){
        switch (function){
        case _FC_READ_HOLDING_REGISTERS:
        case _FC_READ_INPUT_REGISTERS:
        {
        	portMODBUS_REG_VALUE 	value;

			//高字节在前   大端格式
			for (i = 0; i < reg_no; i++) {
				value			= papplication->m_modelinfo.modbus_reg_get(
										(function == _FC_READ_HOLDING_REGISTERS)?(enum_REG_TYPE_HOLD):(enum_REG_TYPE_INPUT),
										reg + i);
				out16(buffer[1], buffer[0], value);
				buffer 			+= sizeof(uint16);
			}
            //每个寄存器sizeof(uint16)长度
            pmodbus_rtu_info->param_len_set(reg_no*sizeof(uint16));
        }
            break;

        case _FC_WRITE_SINGLE_REGISTER:
        case _FC_WRITE_MULTIPLE_REGISTERS:
        {
			portMODBUS_REG_VALUE 	value;

			for (i = 0; i < reg_no; i++) {
				in16(value, buffer[1], buffer[0]);
				if (0 != (rt = papplication->m_modelinfo.modbus_reg_set(enum_REG_TYPE_HOLD, reg + i, value))){
                    break;
                }
				buffer 			+= sizeof(uint16);
			}
        }
            break;
        default:
            break;
        }
    //对于写寄存器操作  要把更新后的寄存器写入到存储设备中
    }else if (enum_PROTOCOL_DONE == phase){
        switch (function){
        case _FC_WRITE_SINGLE_REGISTER:
        case _FC_WRITE_MULTIPLE_REGISTERS:
        {
            class event 	t_event(&application::datum_save, papplication);

            papplication->m_peventloop->run_inloop(&t_event);
        }
            break;

        default:
            break;
        }
    }

	return rt;
}

portBASE_TYPE application::datum_load(void)
{
    portSSIZE_TYPE      ssize;
    struct storage_info storage_info_modbus;
    device_storage *pdevice_storage    =
            static_cast<device_storage *>(m_app_runinfo.m_pdevice_storage);

    m_modelinfo.storage_info_query(enum_REG_TYPE_HOLD, &storage_info_modbus);
	//load modbus regs
	ssize   = pdevice_storage->read(storage_info_modbus.m_storage_addr,
							reinterpret_cast<char *>(storage_info_modbus.m_pdata),
							storage_info_modbus.m_len);
	if ((ssize <= 0) || (ssize != storage_info_modbus.m_len)){
	#ifdef LOGGER
		LOG_FATAL << "modbus regs load/write failed!";
	#endif
		return -1;
	}
	//判断校验和是否正确
	if (m_modelinfo.storage_param_verify(enum_REG_TYPE_HOLD)){
		//校验码错误  写入默认参数
		m_modelinfo.storage_param_chksum(enum_REG_TYPE_HOLD);
        ssize   = pdevice_storage->write(storage_info_modbus.m_storage_addr,
                                reinterpret_cast<char *>(storage_info_modbus.m_pdata),
                                storage_info_modbus.m_len);
		if ((ssize <= 0) || (ssize != storage_info_modbus.m_len)){
		#ifdef LOGGER
			LOG_FATAL << "modbus regs write failed!";
		#endif
			return -1;
		}
	}

    return 0;
}

portBASE_TYPE application::datum_save(void *pvoid, class event *pevent)
{
	application 	*papplication	= static_cast<application *> (pvoid);
    struct          storage_info storage_info;

	//write hold regs to storage device
	device_storage *pdevice_storage    =
		static_cast<device_storage *>(papplication->m_app_runinfo.m_pdevice_storage);

	//hold regs
	papplication->m_modelinfo.storage_info_query(enum_REG_TYPE_HOLD, &storage_info);
	papplication->m_modelinfo.storage_param_chksum(enum_REG_TYPE_HOLD);
	pdevice_storage->write(storage_info.m_storage_addr,
							reinterpret_cast<char *>(storage_info.m_pdata), storage_info.m_len);

	return 0;
}


uint16  application::hold_reg_get(enum hold_reg_index index)
{
    return m_modelinfo.modbus_reg_get(enum_REG_TYPE_HOLD, index);
}
void application::hold_reg_set(enum hold_reg_index index, uint16 value)
{
    m_modelinfo.modbus_reg_set(enum_REG_TYPE_HOLD, index, value);
}

uint16  application::input_reg_get(enum input_reg_index index)
{
    return m_modelinfo.modbus_reg_get(enum_REG_TYPE_INPUT, index);
}
void application::input_reg_set(enum input_reg_index index, uint16 value)
{
    m_modelinfo.modbus_reg_set(enum_REG_TYPE_INPUT, index, value);
}

portBASE_TYPE application::init(void)
{
	static protocol_modbus_rtu 	    t_protocol_modbus_rtu(application::package_event_handler, this);
    static device_ad 		    		t_device_ad;
    static device_pin 				    t_device_pin;
    static device_storage 				t_device_storage;
    static device_pwm		    		t_device_pwm;
    static device_commu   				t_device_commu( &t_protocol_modbus_rtu);
    
#ifdef LOGGER
    //log setting
    Logger::setOutput(debug_output);
#endif
#if 0
    {
        struct tm                      tm_time;
        time_t                          time;
        
        tm_time.tm_year                     = 2014-1900;
        tm_time.tm_mon                      = 4;
        tm_time.tm_mday                     = 1;
        tm_time.tm_hour                     = 17;
        tm_time.tm_min                      = 20;
        tm_time.tm_sec                      = 50;
        time                                = mktime(&tm_time);
        sv_tick_set(time*TICK_PER_SECOND);
    }
#endif
    //device setting
    m_app_runinfo.m_pdevice_pwm             = &t_device_pwm;
    m_app_runinfo.m_pdevice_ad 		        = &t_device_ad;
    m_app_runinfo.m_pdevice_commu 			= &t_device_commu;
    m_app_runinfo.m_pdevice_storage 	    = &t_device_storage;
    m_app_runinfo.m_pdevice_pin 			= &t_device_pin;
	m_app_runinfo.m_status 					= STAT_OK;

	m_modelinfo.name_set(def_MODEL_NAME);
	cpu_pendsv_register(pendsv_handle, this);

    m_app_runinfo.m_pdevice_commu->open(DEVICE_FLAG_RDWR);
    m_app_runinfo.m_pdevice_pin->open(DEVICE_FLAG_RDWR);
    m_app_runinfo.m_pdevice_ad->open(DEVICE_FLAG_RDONLY);
    m_app_runinfo.m_pdevice_pwm->open(DEVICE_FLAG_RDWR);
    m_app_runinfo.m_pdevice_storage->open(DEVICE_FLAG_RDWR);

    //load conf_datum from storage device
    if (datum_load()){
        datum_load();
        while (1);
    }

    t_protocol_modbus_rtu.slave_set(hold_reg_get(enum_REG_MODBUS_ADDR));
#if 0
    hold_reg_set(enum_REG_RHREF_RREQ, 200);
    hold_reg_set(enum_REG_RHREF_DUTY_CYCLE, 150);
    hold_reg_set(enum_REG_HEAT_RREQ, 400);
    hold_reg_set(enum_REG_HEAT_DUTY_CYCLE, 150);
#endif
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_RHREF_FORCE_H, NULL);
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_RHREF_FORCE_L, NULL);
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_HEAT_FORCE_H, NULL);
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_HEAT_FORCE_L, NULL);
    
#if 0
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_RHREF_FREQ,
            (void *)static_cast<uint32>(hold_reg_get(enum_REG_RHREF_RREQ)));
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_RHREF_DUTY_CYCLE,
            (void *)static_cast<uint32>(hold_reg_get(enum_REG_RHREF_DUTY_CYCLE)));

    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_HEAT_FREQ,
            (void *)static_cast<uint32>(hold_reg_get(enum_REG_HEAT_RREQ)));
    m_app_runinfo.m_pdevice_pwm->ioctl(PWM_IOC_HEAT_DUTY_CYCLE,
            (void *)static_cast<uint32>(hold_reg_get(enum_REG_HEAT_DUTY_CYCLE)));
#endif
    return 0;
}

int application::event_handle_ad(void *pvoid, int event_type, class buffer &buf, class Timestamp &ts)
{
	application *papplication      = static_cast<application *> (pvoid);

	char aa[100];

	buf.retrieveAllAsCharArray(reinterpret_cast<int8 *>(aa));

{
	class event 	t_event(&application::event_cb, papplication);
	class buffer   &t_buffer 	= t_event.buffer_get();

	papplication	= papplication;
	t_buffer.appendInt32(1234);
	t_buffer.appendInt16(222);
	papplication->m_peventloop->run_inloop(&t_event);
}

	return 0;
}

portBASE_TYPE   application::event_cb(void *pvoid, class event *pevent)
{
	application 	*papplication	= static_cast<application *> (pvoid);
	class buffer 	&t_buffer 		= pevent->buffer_get();
	volatile int32 			a				= t_buffer.readInt32();
	volatile int16 			b				= t_buffer.readInt16();

    papplication    = papplication;

	return 0;
}


portBASE_TYPE application::run()
{
	device_commu *pdevice_commu =
			(device_commu *) m_app_runinfo.m_pdevice_commu;
	device_ad *pdevice_ad =
			static_cast<device_ad *>(m_app_runinfo.m_pdevice_ad);
	eventloop t_loop;
	channel t_channel_ad(&t_loop, pdevice_ad);
    
    //消除未使用的变量警告
    pdevice_commu                           = pdevice_commu;
	m_peventloop 				            = &t_loop;
    //每隔4分钟执行一次
    m_app_runinfo.m_handle_period 			= t_loop.run_every(1*60*1000,
                                                self_calc_handle,
                                                this,
                                                "self calc handle");
    ASSERT(m_app_runinfo.m_handle_period != (timer_handle_type)-1);
    
	t_channel_ad.event_handle_register(&application::event_handle_ad, this);
	t_channel_ad.enableReading();
    
    //启动loop循环
	t_loop.loop();

	return 0;
}

//intertup context ; interrupt disable
void application::pendsv_handle(void *pvoid)
{
	application *papplication      = static_cast<application *> (pvoid);
	device_commu *pdevice_commu    = static_cast<device_commu *>(papplication->m_app_runinfo.m_pdevice_commu);

	pdevice_commu->package_event_fetch();
}

void application::self_calc_handle(void *pdata)
{
	application *papplication  = static_cast<application *> (pdata);
    papplication    = papplication;

    cpu_led_toggle();
}

application  *papplication_watch;

int main(void)
{
    application  *papplication      = singleton<application>::instance();
    papplication_watch              = papplication;
    
    //bsp startup
    bsp_startup();
    sv_startup();
    papplication->init();
    papplication->run();

    return 0;
}



