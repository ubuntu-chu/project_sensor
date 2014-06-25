#include    "main.h"
#include    "devices.h"
#include    "ad.h"
#include    "pwm.h"
#include    "pin.h"
#include    "commu.h"
#include    "storage.h"
#include    "modbus.h"
#include    "algorithm.h"
#include    "../api/log/log.h"
#include 	"../bsp/driver/drv_interface.h"

#ifdef LOGGER
void debug_output(const char* msg, int len)
{
	uart_tx(DBG_UART, (uint8 *)msg, len);
}
#endif

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
            class buffer   &t_buffer 	= t_event.buffer_get();

            //将此次操作的寄存器起始位置及长度添加到event参数列表中
            t_buffer.appendInt16(reg);
            t_buffer.appendInt16(reg_no);

            papplication->m_peventloop->run_inloop(&t_event);
        }
            break;

        default:
            break;
        }
    }

	return rt;
}

enum datum_load_stat application::_datum_load(enum datum_load_ctrl ctrl)
{
    portSSIZE_TYPE      ssize;
    struct storage_info storage_info_modbus;
    device_storage *pdevice_storage    =
            static_cast<device_storage *>(m_app_runinfo.m_pdevice_storage);
    enum datum_load_stat 	rt 						= enum_DATUM_LOAD_STAT_SUCCESS;

    m_modelinfo.storage_info_query(enum_REG_TYPE_HOLD, &storage_info_modbus);
	//load modbus regs
	ssize   = pdevice_storage->read(storage_info_modbus.m_storage_addr,
							reinterpret_cast<char *>(storage_info_modbus.m_pdata),
							storage_info_modbus.m_len);
	if ((ssize <= 0) || (ssize != storage_info_modbus.m_len)){
#ifdef LOGGER
		LOG_ERROR << "modbus regs read failed!";
#endif
		rt 											= enum_DATUM_LOAD_STAT_FAILED;
		goto quit;
	}
	//判断校验和是否正确
	if (m_modelinfo.storage_param_verify(enum_REG_TYPE_HOLD)){
		//校验码错误  写入默认参数
		m_modelinfo.storage_param_chksum(enum_REG_TYPE_HOLD);
		if (ctrl == enum_DATUM_LOAD_CTRL_RECOVER){
			ssize   = pdevice_storage->write(storage_info_modbus.m_storage_addr,
									reinterpret_cast<char *>(storage_info_modbus.m_pdata),
									storage_info_modbus.m_len);
			if ((ssize <= 0) || (ssize != storage_info_modbus.m_len)){
#ifdef LOGGER
				LOG_ERROR << "modbus regs write failed!";
#endif
				rt 									= enum_DATUM_LOAD_STAT_FAILED;
				goto quit;
			}
			//为了确保写入正确 再此进行读取验证
			rt 										= enum_DATUM_LOAD_STAT_RETRY;
		}else {
#ifdef LOGGER
			LOG_ERROR << "modbus regs load/write failed!";
#endif
			rt 										= enum_DATUM_LOAD_STAT_FAILED;
			goto quit;
		}
	}
quit:
    return rt;
}

portBASE_TYPE application::datum_load(void)
{
	portBASE_TYPE 	rt								= 0;
    enum datum_load_stat	load_stat;

    load_stat 										= _datum_load(enum_DATUM_LOAD_CTRL_RECOVER);
    if (enum_DATUM_LOAD_STAT_SUCCESS != load_stat){
    	//再次尝试读取验证
		if ((enum_DATUM_LOAD_STAT_RETRY == load_stat)
			//此时若读取发生错误 则不再尝试对存贮器进行写入操作
			&& (enum_DATUM_LOAD_STAT_SUCCESS != _datum_load(enum_DATUM_LOAD_CTRL_NONE))){
			rt                                      = -1;

		}else {
			rt                                  	= -1;
		}
    }

    return rt;
}

portBASE_TYPE application::datum_save(void *pvoid, class event *pevent)
{
#define     _def_MODIFY_VALID_IMMEDIATELY
	application 	*papplication	= static_cast<application *> (pvoid);
    struct          storage_info storage_info;
    portSSIZE_TYPE      ssize;
#ifdef _def_MODIFY_VALID_IMMEDIATELY
	class buffer 	&t_buffer 		= pevent->buffer_get();
	uint16 			reg				= t_buffer.readInt16();
	uint16 			reg_no			= t_buffer.readInt16();
#endif

	//write hold regs to storage device
	device_storage *pdevice_storage    =
		static_cast<device_storage *>(papplication->m_app_runinfo.m_pdevice_storage);

	//hold regs
	papplication->m_modelinfo.storage_info_query(enum_REG_TYPE_HOLD, &storage_info);
	papplication->m_modelinfo.storage_param_chksum(enum_REG_TYPE_HOLD);
	ssize = pdevice_storage->write(storage_info.m_storage_addr,
							reinterpret_cast<char *>(storage_info.m_pdata), storage_info.m_len);
	if ((ssize <= 0) || (ssize != storage_info.m_len)){
#ifdef LOGGER
		LOG_ERROR << "modbus regs read failed!";
#endif
		return -1;
	}
    
#ifdef _def_MODIFY_VALID_IMMEDIATELY
	if (IN_RANGES(0, reg, reg+reg_no))
	{
		dynamic_cast<protocol_modbus_rtu *>(papplication->m_app_runinfo.m_pprotocol)->slave_set(
            papplication->m_modelinfo.m_stuSensorPara.ucLinkAddr);
	}
	if (IN_RANGES(1, reg, reg+reg_no))
    {
		 papplication->m_app_runinfo.m_pdevice_commu->ioctl(COMMU_IOC_BAUD,
                        reinterpret_cast<void *>(papplication->m_modelinfo.m_stuSensorPara.ucCommBaud));
    }
#endif    
	return 0;
}

portBASE_TYPE application::init(void)
{
	static protocol_modbus_rtu 	    	t_protocol_modbus_rtu(application::package_event_handler, this);
    static device_ad 		    		t_device_ad;
    static device_pin 				    t_device_pin;
    static device_storage 				t_device_storage;
    static device_pwm		    		t_device_pwm;
    static device_commu   				t_device_commu( &t_protocol_modbus_rtu);
    portBASE_TYPE   rt  = 0;
    
#ifdef LOGGER
    {
        struct tm                      tm_time;
        time_t                          time;
#if defined(__CC_ARM)
        //获取编译程序时的系统时间 
        strptime(__DATE__" "__TIME__, "%b %d %Y %H:%M:%S", &tm_time);
#endif
        time                                = mktime(&tm_time);
        sv_xtime_set(time);
#if 0
        {
            struct tm                      tm_t;
            volatile char time_mm[100];
            tm_t = *localtime(&time);
            strcpy((char *)time_mm, asctime(&tm_t));
        }
#endif
        //log setting
        Logger::setOutput(debug_output);
    }
#endif
    //device setting
    m_app_runinfo.m_pdevice_pwm             = &t_device_pwm;
    m_app_runinfo.m_pdevice_ad 		        = &t_device_ad;
    m_app_runinfo.m_pdevice_commu 			= &t_device_commu;
    m_app_runinfo.m_pdevice_storage 	    = &t_device_storage;
    m_app_runinfo.m_pdevice_pin 			= &t_device_pin;
    m_app_runinfo.m_pprotocol 				= &t_protocol_modbus_rtu;
	m_app_runinfo.m_status 					= enum_STAT_OK;

	m_modelinfo.name_set(def_MODEL_NAME);
	//注册通讯事件响应函数
	cpu_pendsv_register(pendsv_handle, this);

    m_app_runinfo.m_pdevice_pin->open(DEVICE_FLAG_RDWR);
    m_app_runinfo.m_pdevice_pwm->open(DEVICE_FLAG_RDWR);
    m_app_runinfo.m_pdevice_ad->open(DEVICE_FLAG_RDONLY);
    m_app_runinfo.m_pdevice_storage->open(DEVICE_FLAG_RDWR);
    m_app_runinfo.m_pdevice_commu->open(DEVICE_FLAG_RDWR);
    //设置通讯波特率
    m_app_runinfo.m_pdevice_commu->ioctl(COMMU_IOC_BAUD, 
                        reinterpret_cast<void *>(m_modelinfo.m_stuSensorPara.ucCommBaud));
    //设置modbus协议地址
    t_protocol_modbus_rtu.slave_set(m_modelinfo.m_stuSensorPara.ucLinkAddr);
    t_protocol_modbus_rtu.reg_max_no_set(def_INPUT_REG_MAX_NO, def_HOLD_REG_MAX_NO);

    //load conf_datum from storage device
    if (datum_load()){
		rt                                      = -1;
		m_app_runinfo.m_status 				    = enum_STAT_DATUM_FAILED;
    }
    
    return rt;
}

//intertup context ; interrupt disable   通讯处理函数
void application::pendsv_handle(void *pvoid)
{
	application *papplication      		= static_cast<application *> (pvoid);
	device_commu *pdevice_commu    		= static_cast<device_commu *>(papplication->m_app_runinfo.m_pdevice_commu);

	pdevice_commu->package_event_fetch();
}

int application::event_handle_ad(void *pvoid, int event_type, class buffer &buf, class Timestamp &ts)
{
	application *papplication      		= static_cast<application *> (pvoid);
	device_ad 	*pdevice_ad 			= static_cast<device_ad *>(papplication->m_app_runinfo.m_pdevice_ad);
    device_pwm	*pdevice_pwm 			= static_cast<device_pwm *>(papplication->m_app_runinfo.m_pdevice_pwm);
	portBASE_TYPE 	buf_readablebytes 	= buf.readableBytes();
    float       value					= 0.0;
    struct _app_runinfo_	*pruninfo 	= &(papplication->m_app_runinfo);
    static float fTemperature;
    
#ifdef LOGGER
    const char *run_status_str_dbg[] 	= {
    	"ADC_T", "CALC_T", "ADC_RHS", "CALC_RH", "ADC_PRS", "CALC_P", "CALC_D", "CALC_PPM", "CALC_DEW",
	};
#endif

    //只运行在参数测量模式下
    //ad通道只能出现只读事件
    if ((event_type == POLLOUT) || (pruninfo->ucSensorRunMode != MODE_MEASURE_PARA)){
    	return 0;
    }
#ifdef LOGGER
	//LOG_TRACE << "adc stat:" << run_status_str_dbg[pruninfo->ucADC1RunStus];
#endif

	//由于AD_IOC_FAKE_READABLE命令而引发的可读时， buf_readablebytes() = 0
	if ((0 == buf_readablebytes) && (pruninfo->ucADC1RunStus == ADC_T)){
        //由初始状态(此状态在系统运行期间只会执行一次)进入时， 进入到温度校准状态  
		pruninfo->ucADC1RunStus					= CALC_T;
        //初始化硬件相关状态
    	pdevice_pwm->ioctl(PWM_IOC_MEASURE_ON, NULL);
        //此引脚电平影响ADC结果
        pdevice_pwm->ioctl(PWM_IOC_HEAT_FORCE_H, NULL);
		pdevice_ad->ioctl(AD_IOC_START_RTD_ADC, NULL);
	}else {
		pSENSOR_DATA pSensorData 				= &papplication->m_modelinfo.m_stuSensorData;
		pSENSOR_PARA pSensorPara 				= &papplication->m_modelinfo.m_stuSensorPara;

		if (0 != buf_readablebytes){
            value       						= buf.readFloat();
		}
		switch (pruninfo->ucADC1RunStus){
		case CALC_T:
		{
            pSensorData->fADCTemprature         = value;
            fTemperature 						= CalculateRTDTemp(value);
			pSensorData->ssTemprature 			= (signed short)(fTemperature*100);
            pruninfo->ucADC1RunStus				= CALC_RH;
			pdevice_ad->ioctl(AD_IOC_START_RHS_ADC, NULL);
		}
			break;

		case CALC_RH:
		{
            pSensorData->fADCHumiSgl 			= value;
			pSensorData->fADCCapcitance 		= value/0.003063808;

			pruninfo->ucADC1RunStus				= CALC_P;
			pdevice_ad->ioctl(AD_IOC_START_PRS_ADC, NULL);
		}
			break;

		case CALC_P:
		{
			float fPRSValue;

            fPRSValue = CalculatePRSVal(value, pSensorPara->usPADC, pSensorPara->usPValue);
			pSensorData->usADCPrs 				= (unsigned short)value;
			pSensorData->ssPressure 			= (signed short)fPRSValue;

			pruninfo->ucADC1RunStus				= CALC_D;
			pdevice_ad->ioctl(AD_IOC_FAKE_READABLE, NULL);
		}
			break;

		case CALC_D:
		{
			float fDensity;
			float fPRS20Value;

			fDensity = CalculateDensity((float*) &fPRS20Value, pSensorData->ssPressure, fTemperature);
			pSensorData->ssPressure20 			= (signed short) fPRS20Value;
			pSensorData->usDensity 				= (unsigned short) (fDensity * 100);

			papplication->m_app_runinfo.ucADC1RunStus				= CALC_PPM;
			pdevice_ad->ioctl(AD_IOC_FAKE_READABLE, NULL);
		}
			break;

		case CALC_PPM:
		{

			pruninfo->ucADC1RunStus				= CALC_DEW;
			pdevice_ad->ioctl(AD_IOC_FAKE_READABLE, NULL);
		}
			break;

		case CALC_DEW:
		{

			pruninfo->ucADC1RunStus				= CALC_T;
			pdevice_ad->ioctl(AD_IOC_START_RTD_ADC, NULL);
		}
			break;

		default:
		{
			//出现了异常状态  重新初始化为CALC_T状态
			pruninfo->ucADC1RunStus				= CALC_T;
			pdevice_ad->ioctl(AD_IOC_START_RTD_ADC, NULL);
		}
			break;
		}
	}

	return 0;
}

void application::self_calib_handle(void *pdata)
{
	application *papplication  			= static_cast<application *> (pdata);
    device_pwm	*pdevice_pwm 			= static_cast<device_pwm *>(papplication->m_app_runinfo.m_pdevice_pwm);
	device_ad 	*pdevice_ad 			= static_cast<device_ad *>(papplication->m_app_runinfo.m_pdevice_ad);
    struct _app_runinfo_	*pruninfo 	= &(papplication->m_app_runinfo);
#ifdef LOGGER
    const char *mode_str_dbg[] 	= {
    	"MODE_MEASURE_PARA", "MODE_SENSOR_WASH", "MODE_CALIB_HUMI",
	};
#endif

#ifdef LOGGER
	LOG_TRACE << "run mode:" << mode_str_dbg[pruninfo->ucSensorRunMode];
#endif
    //运行参数校准功能
    switch (pruninfo->ucSensorRunMode){
    case MODE_MEASURE_PARA:
    {
    	pdevice_pwm->ioctl(PWM_IOC_MEASURE_OFF, NULL);
    	// 80kHz freq. PWM0 output for 50% high. PWM1 output 1% duty cycle.
		// HEAT : HIGH_OFF  ON:75%  0.1Wat
    	pdevice_pwm->ioctl(PWM_IOC_HEAT_FREQ, (void *)100);
    	pdevice_pwm->ioctl(PWM_IOC_HEAT_DUTY_CYCLE, (void *)25);
#ifdef LOGGER
        LOG_TRACE << "self-calib timer: 10s";
#endif
    	//更改定时器超时时间：10秒
    	papplication->m_peventloop->timer_ioctl(pruninfo->m_handle_period,
    			enum_TIMER_IOC_RESTART_WITH_TIME, (void *)(10*def_TIME_1_SECOND));

    	pruninfo->ucSensorRunMode                       = MODE_SENSOR_WASH;
    }
    	break;

    case MODE_SENSOR_WASH:
    {
    	pdevice_pwm->ioctl(PWM_IOC_HEAT_FORCE_H, NULL);
    	pdevice_pwm->ioctl(PWM_IOC_MEASURE_ON, NULL);

    	//更改定时器超时时间：1秒
    	pruninfo->m_uiCalibRecTimeGap 					= 1;
    	pruninfo->m_ucCalibRecordTimes 					= 0;
        //立即进行采样
        pdevice_ad->ioctl(AD_IOC_GET_SGL_RTD_ADC, (void *)&(pruninfo->iADCTemp[pruninfo->m_ucCalibRecordTimes]));
		pdevice_ad->ioctl(AD_IOC_GET_SGL_RHS_ADC, (void *)&(pruninfo->iADCRHS[pruninfo->m_ucCalibRecordTimes]));
        
        //重设定时器
    	papplication->m_peventloop->timer_ioctl(pruninfo->m_handle_period,
    			enum_TIMER_IOC_RESTART_WITH_TIME, (void *)(pruninfo->m_uiCalibRecTimeGap*def_TIME_1_SECOND));
#ifdef LOGGER
        LOG_TRACE << "self-calib timer - m_uiCalibRecTimeGap:" << pruninfo->m_uiCalibRecTimeGap << "s";
#endif
    	pruninfo->ucSensorRunMode                       = MODE_CALIB_HUMI;
    }
    	break;

    case MODE_CALIB_HUMI:
    {
        //定时间隔为2的指数幂
		pruninfo->m_uiCalibRecTimeGap					<<= 1;
#ifdef LOGGER
		LOG_TRACE << "m_ucCalibRecordTimes:" << pruninfo->m_ucCalibRecordTimes;
#endif
    	pruninfo->m_ucCalibRecordTimes++;
		if (pruninfo->m_ucCalibRecordTimes > 8) {
			pdevice_ad->ioctl(AD_IOC_GET_SGL_RTD_ADC, (void *)&(pruninfo->iADCTemp[pruninfo->m_ucCalibRecordTimes]));
            pdevice_ad->ioctl(AD_IOC_GET_SGL_RHS_ADC, (void *)&(pruninfo->iADCRHS[pruninfo->m_ucCalibRecordTimes]));
            
            //calc rh

            
			papplication->m_peventloop->timer_ioctl(pruninfo->m_handle_period,
					enum_TIMER_IOC_RESTART_WITH_TIME, (void *)(def_SELF_CALIB_TIME));
            
			//Switch to normal measure
			pruninfo->ucSensorRunMode                   = MODE_MEASURE_PARA;
#ifdef LOGGER
            LOG_TRACE << "MODE_CALIB_HUMI complete, now mode = MODE_MEASURE_PARA";
            LOG_TRACE << "self-calib timer: 8 hour";
#endif
		}else {
            papplication->m_peventloop->timer_ioctl(pruninfo->m_handle_period,
					enum_TIMER_IOC_RESTART_WITH_TIME, (void *)(pruninfo->m_uiCalibRecTimeGap*def_TIME_1_SECOND));
#ifdef LOGGER
            LOG_TRACE << "self-calib timer - m_uiCalibRecTimeGap:" << pruninfo->m_uiCalibRecTimeGap << "s";
#endif
		}
    }
    	break;

    default:
    {
    	//出现异常状态
		papplication->m_peventloop->timer_ioctl(pruninfo->m_handle_period,
				enum_TIMER_IOC_RESTART_WITH_TIME, (void *)(def_SELF_CALIB_TIME));
		//Switch to normal measure
		pruninfo->ucSensorRunMode                       = MODE_MEASURE_PARA;
    }
    	break;
    }
    cpu_led_toggle();
}

portBASE_TYPE application::run()
{
	device_ad   *pdevice_ad 			                = static_cast<device_ad *>(m_app_runinfo.m_pdevice_ad);
	eventloop   t_loop;
	channel     t_channel_ad(&t_loop, pdevice_ad);

	//m_app_runinfo.m_startupcalib 		                = false;
    m_app_runinfo.m_startupcalib 		                = true;
    //消除未使用的变量警告
	m_peventloop 				                        = &t_loop;
#ifdef LOGGER
	LOG_TRACE << "register self calib timer";
#endif
	//注册定时器
    m_app_runinfo.m_handle_period 		                = 
            t_loop.run_every((true == m_app_runinfo.m_startupcalib)?(def_TIME_1_MINUTE):(def_SELF_CALIB_TIME),
                                                self_calib_handle,
                                                this,
                                                "self calib handle");
    ASSERT(m_app_runinfo.m_handle_period != (timer_handle_type)-1);

    //AD通道初始化
	t_channel_ad.event_handle_register(&application::event_handle_ad, this);
	t_channel_ad.enableReading();

	//初始化相关状态变量
	m_app_runinfo.ucSensorRunMode		                = MODE_MEASURE_PARA;
	m_app_runinfo.ucADC1RunStus			                = ADC_T;
	//控制AD设备产生虚假可读事件  让AD通道的回调函数被调用
	pdevice_ad->ioctl(AD_IOC_FAKE_READABLE, NULL);

#ifdef LOGGER
	LOG_TRACE << "start event loop";
#endif
    //启动loop循环
	t_loop.loop();

	return 0;
}

application  *papplication_watch;

int main(void)
{
    application  *papplication      	                = singleton<application>::instance();
    papplication_watch              	                = papplication;
    
    //bsp startup
    bsp_startup();
    sv_startup();
    papplication->init();
    papplication->run();

    return 0;
}



