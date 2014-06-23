#include "datum.h"

const struct param_range<uint8> t_hold_reg_0_6[] = {

	//ucLinkAddr
	{1, 247, enum_REG_ATTR_RDWR},
	//ucCommBaud
	{0, 4, enum_REG_ATTR_RDWR},
	//ucGasMixingRatio
	{0, 100, enum_REG_ATTR_RDWR},
	//ucNormalTemperature
	{0, 255, enum_REG_ATTR_RDWR},
	//ucAutoWashON
	{0, 255, enum_REG_ATTR_RDWR},
	//ucBootWashON
	{0, 255, enum_REG_ATTR_RDWR},
	//ucMannulWashON
	{0, 255, enum_REG_ATTR_RDWR},
};


model_datum::model_datum(uint8 *pname):m_name(pname)
{
	hold_reg_set(enum_REG_MODBUS_ADDR, 1);
	m_magic_setup           = "magic setup string";
	modbus_hold_reg_def_init();
}

portBASE_TYPE model_datum::storage_info_query(enum reg_type type, struct storage_info *pinfo)
{
	if (pinfo == NULL){
		return -1;
	}
	if (type == enum_REG_TYPE_HOLD){
		pinfo->m_storage_addr 						= ROUND_UP((strlen(reinterpret_cast<const char *>(m_magic_setup))), 16);
		pinfo->m_pdata 								= reinterpret_cast<uint8 *>(&m_stuSensorPara);
		pinfo->m_len 								= sizeof(m_stuSensorPara);
	}else if (type == enum_REG_TYPE_MAGIC){
		pinfo->m_storage_addr 						= 0;
		pinfo->m_pdata 								= const_cast<uint8 *>(m_magic_setup);
		pinfo->m_len 								= strlen(reinterpret_cast<const char *>(m_magic_setup)) + 1;   //contain end code
	}else {
		return -1;
	}

	return 0;
}

portBASE_TYPE model_datum::storage_param_verify(enum reg_type type)
{
	portBASE_TYPE 	 rt				= 0;
    uint16          crc;

	crc = crc16((const uint8*)&m_stuSensorPara.ucLinkAddr, 
                        sizeof(m_stuSensorPara) - sizeof(m_stuSensorPara.chksum));
    if (m_stuSensorPara.chksum != crc){
    	modbus_hold_reg_def_init();
		rt 							= -1;
	}

	return rt;
}

void model_datum::storage_param_chksum(enum reg_type type)
{
	m_stuSensorPara.chksum = crc16((const uint8*)&m_stuSensorPara.ucLinkAddr, 
                        sizeof(m_stuSensorPara) - sizeof(m_stuSensorPara.chksum));
}

portMODBUS_REG_VALUE model_datum::modbus_reg_get(enum reg_type type, portMODBUS_REG_INDEX reg)
{
	portMODBUS_REG_VALUE	value;

	if (type == enum_REG_TYPE_INPUT){
		value 						= input_reg_get(reg);
	}else if (type == enum_REG_TYPE_HOLD){
		value 						= hold_reg_get(reg);
	}else {
		value 						= (portMODBUS_REG_VALUE)-1;
	}

	return value;
}

portBASE_TYPE model_datum::modbus_reg_set(enum reg_type type, portMODBUS_REG_INDEX reg, portMODBUS_REG_VALUE value)
{
	portBASE_TYPE   rt;
    
    if (type == enum_REG_TYPE_INPUT){
		rt                          = input_reg_set(reg, value);
	}else if (type == enum_REG_TYPE_HOLD){
		rt                          = hold_reg_set(reg, value);
	}
    
    return rt;
}

void model_datum::modbus_hold_reg_def_init(void)
{
    memset(&m_stuSensorPara, 0, sizeof(m_stuSensorPara));
    
    m_stuSensorPara.ucLinkAddr = 1;
	m_stuSensorPara.ucCommBaud = 2;  //B9600
	m_stuSensorPara.usPADC[0] = 1120;
	m_stuSensorPara.usPValue[0] = 1000;
	m_stuSensorPara.usPADC[1] = 14140;
	m_stuSensorPara.usPValue[1] = 3500;
	m_stuSensorPara.usPADC[2] = 27000;
	m_stuSensorPara.usPValue[2] = 6000;
	m_stuSensorPara.usPADC[3] = 39770;
	m_stuSensorPara.usPValue[3] = 8500;

#if 0
    uint32 i;
    for (i = 0; i < sizeof(m_stuSensorPara.usRH); i++){
        m_stuSensorPara.usRH[i] = i + 1;
    }
    
    for (i = 0; i < sizeof(m_stuSensorPara.ulHumiADC); i++){
        m_stuSensorPara.ulHumiADC[i] = 100 - i;
    }
    
    m_stuSensorData.usStatus = 1;
    
    m_stuSensorData.usPPM = 2;
    m_stuSensorData.ssPressure = 3;
    m_stuSensorData.ssPressure20 = 4;
    m_stuSensorData.usRH = 5;
    m_stuSensorData.usRH20 = 6;
    m_stuSensorData.usDensity = 7;
	m_stuSensorData.ssTemprature = 8;
	m_stuSensorData.ssDew = 9;
	m_stuSensorData.ssDewUnderPress = 10;
	//采样值
	m_stuSensorData.usADCPrs = 11;
    m_stuSensorData.usADCPrsVs = 12;
    
    m_stuSensorData.fADCTemprature  = 13;
    m_stuSensorData.fADCHumiSgl = 14;
    m_stuSensorData.fADCCapcitance = 15;
#endif
}

//对寄存器的写操作 要增加值范围判断
portBASE_TYPE model_datum::reg_operate(enum reg_access_type type, enum reg_operate operate,
		void *preg_addr, portMODBUS_REG_VALUE &value, void *prange)
{
	portBASE_TYPE 			rt				= 0;

	switch (type){
	case enum_REG_UINT8:
    {
		struct param_range<uint8> *pparam_rang = reinterpret_cast<struct param_range<uint8> *>(prange);
        if (operate == enum_REG_READ){
			if ((NULL != pparam_rang) && (pparam_rang->m_attr == enum_REG_ATTR_WRONLY)){
                rt 						    = -1;
				break;
            }
            value  							= XBYTE(preg_addr);
		}else {
			
			if (NULL != pparam_rang){
				if ((value < pparam_rang->m_min) || (value > pparam_rang->m_max)
                    || (pparam_rang->m_attr == enum_REG_ATTR_RDONLY)){
					rt 						= -1;
					break;
				}
			}
			XBYTE(preg_addr)				= value;
		}
    }
		break;

	case enum_REG_UINT16:
    {
		if (operate == enum_REG_READ){
			value  							= XHWORD(preg_addr);
		}else {
			XHWORD(preg_addr)				= value;
		}
    }
		break;

	case enum_REG_UINT32:
    {
		if (operate == enum_REG_READ){
			value  							= XWORD(preg_addr);
		}else {
			XWORD(preg_addr)				= value;
		}
    }
		break;

	case enum_REG_UINT32_HIGH_16:
	{
		portMODBUS_REG_VALUE	tmp			= XWORD(preg_addr);
		if (operate == enum_REG_READ){
			value  							= tmp >> 16;
		}else {
			tmp 							= (tmp & (portMODBUS_REG_VALUE)0xffff) | ((value & (portMODBUS_REG_VALUE)0xffff) << 16);
			XWORD(preg_addr)				= tmp;
		}
	}
		break;

	case enum_REG_UINT32_LOW_16:
	{
		portMODBUS_REG_VALUE	tmp			= XWORD(preg_addr);
		if (operate == enum_REG_READ){
			value 							= tmp & (portMODBUS_REG_VALUE)0xffff;
		}else {
			tmp 							= (tmp & (portMODBUS_REG_VALUE)0xffff0000) | ((value & (portMODBUS_REG_VALUE)0xffff));
			XWORD(preg_addr)				= tmp;
		}
	}
		break;
	default:
		break;
	}

	return rt;
}

portBASE_TYPE model_datum::hold_reg_operate(enum reg_operate operate, portMODBUS_REG_INDEX reg, portMODBUS_REG_VALUE &value)
{
	void 			*pstart_addr;
	void 			*preg_addr;
	portBASE_TYPE 	rt 					= 0;

	//0-6: tyep.char 	//7-30:type.short 	//31-56:type.long
	if (reg < _def_SPLIT_HOLD_R_INDEX_2) {
		//type:char   字符型变量  高字节固定为0
		if (reg < _def_SPLIT_HOLD_R_INDEX_1) {
			pstart_addr 				= (void *)&m_stuSensorPara.ucLinkAddr;
			preg_addr 					= (void *)((unsigned char* )pstart_addr + reg);
			rt  = reg_operate(enum_REG_UINT8, operate, preg_addr, value, (void *)(&t_hold_reg_0_6[reg]));
		//type:short  短整型变量
		} else {
			pstart_addr 				= (void *) &m_stuSensorPara.usWashPowerSet;
			preg_addr 					= (void *)((unsigned short* )pstart_addr + reg - _def_SPLIT_HOLD_R_INDEX_1);
			rt  = reg_operate(enum_REG_UINT16, operate, preg_addr, value);
		}
	//type:long
	}else if (reg < _def_SPLIT_HOLD_R_INDEX_3){
		portMODBUS_REG_INDEX 	tmp		= (reg-_def_SPLIT_HOLD_R_INDEX_2)/2;
		pstart_addr 					= (void *) &m_stuSensorPara.ulHumiADC;
		preg_addr 						= (void *)((unsigned long* )pstart_addr + tmp);

		//2的奇数   高位    此处取决于_def_SPLIT_HOLD_R_INDEX_2的值
		rt  = reg_operate(((reg % 2) == 1)?(enum_REG_UINT32_HIGH_16):(enum_REG_UINT32_LOW_16), operate, preg_addr, value);
	//其他未定义寄存器
	}else {
		if (operate == enum_REG_READ){
			value  					= 0;
		}
	}

	return rt;
}

portBASE_TYPE model_datum::input_reg_operate(enum reg_operate operate, portMODBUS_REG_INDEX reg, portMODBUS_REG_VALUE &value)
{
	void 			*pstart_addr;
	void 			*preg_addr;
    portBASE_TYPE 	rt 					= 0;

	if (reg < _def_SPLIT_INPUT_R_INDEX_1) {
		//type:short  短整型变量
		pstart_addr 						= (void *) &m_stuSensorData.usStatus;
		preg_addr 							= (void *) ((unsigned short*) pstart_addr + reg);
		rt  = reg_operate(enum_REG_UINT8, operate, preg_addr, value);
	//type:long
	}else if (reg < _def_SPLIT_INPUT_R_INDEX_2) {
		portMODBUS_REG_INDEX 	tmp		= (reg-_def_SPLIT_INPUT_R_INDEX_1)/2;
		pstart_addr 						= (void *) &m_stuSensorData.fADCTemprature;
		preg_addr 							= (void *)((unsigned long* )pstart_addr + tmp);

		//2的偶数   高位    此处取决于_def_SPLIT_INPUT_R_INDEX_1的值
		rt  = reg_operate(((reg % 2) == 0)?(enum_REG_UINT32_HIGH_16):(enum_REG_UINT32_LOW_16), operate, preg_addr, value);
	//其他未定义寄存器
	}else {
		if (operate == enum_REG_READ){
			value  							= 0;
		}
	}

	return rt;
}

portMODBUS_REG_VALUE  model_datum::hold_reg_get(portMODBUS_REG_INDEX reg)
{
	portMODBUS_REG_VALUE	value;

	hold_reg_operate(enum_REG_READ, reg, value);
	return value;
}
portBASE_TYPE model_datum::hold_reg_set(portMODBUS_REG_INDEX reg, portMODBUS_REG_VALUE value)
{
	return hold_reg_operate(enum_REG_WRITE, reg, value);
}

portMODBUS_REG_VALUE  model_datum::input_reg_get(portMODBUS_REG_INDEX reg)
{
	portMODBUS_REG_VALUE	value;

	input_reg_operate(enum_REG_READ, reg, value);
	return value;
}

portBASE_TYPE model_datum::input_reg_set(portMODBUS_REG_INDEX reg, portMODBUS_REG_VALUE value)
{
	return input_reg_operate(enum_REG_WRITE, reg, value);
}

