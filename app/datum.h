#ifndef _DATUM_H
#define _DATUM_H
#include "../includes/includes.h"

enum hold_reg_index{
    enum_REG_BREAK_SECTION_START_1 = 0,
    enum_REG_MODBUS_ADDR  = enum_REG_BREAK_SECTION_START_1,                      //设备地址，取值范围（1-247）；广播地址0
    enum_REG_COMMU_BPS,                             //通信速率（0：:2400，1：4800，2：9600，3：19200，4：38400）
    enum_REG_GAS_MIX_RATIO,                         //气体混合比率（0~100%，SF6）                    
    enum_REG_P_CORRECTION_FACTOR,                   //压力修正温度（默认20℃，单位0.1℃）
    enum_REG_H_SENSOR_AUTO_CLR,                     //湿度传感器自动清洗开关（1：on，0：off）
    enum_REG_H_SENSOR_POWER_ON_CLR,                 //湿度传感器开机清洗开关（1：on，0：off）
    enum_REG_H_SENSOR_MANUAL_CLR,                   //湿度传感器手动清洗开关（1：on，0：off）
    enum_REG_H_SENSOR_CLR_POWER_SET,                //湿度传感器清洗功率设置（0~0xFFFF, PWM占空比）
    enum_REG_H_SENSOR_ZERO_CAL_SET,                 //湿度传感器零点校准设置（0~0xFFFF, PWM占空比）    -Heat
    enum_REG_H_SENSOR_ZERO_REF_SET,                 //湿度传感器零点参考设置（0~0xFFFF, PWM占空比）    -RHref
    enum_REG_BREAK_SECTION_END_1,
    
    
    enum_REG_BREAK_SECTION_START_2 = 257,
    enum_REG_P_CAL_0    = enum_REG_BREAK_SECTION_START_2,  //压力校准点0 压力，单位hPa，绝压，参考1000hPa
    enum_REG_P_CAL_1,
    enum_REG_P_CAL_2,
    enum_REG_P_CAL_3,
    enum_REG_P_CAL_ADC_0,                           //压力校准点0，压力传感器采样值Padc
    enum_REG_P_CAL_ADC_1,
    enum_REG_P_CAL_ADC_2,
    enum_REG_P_CAL_ADC_3,
    
    enum_REG_H_CAL_0,                               //湿度校准点0，湿度单位：0.01%rh，参考 0%RH
    enum_REG_H_CAL_1,
    enum_REG_H_CAL_2,
    enum_REG_H_CAL_3,
    enum_REG_H_CAL_4,
    enum_REG_H_CAL_5,
    enum_REG_H_CAL_6,
    enum_REG_H_CAL_7,
    enum_REG_H_CAL_8,
    enum_REG_H_CAL_9,
    enum_REG_H_CAL_10,
    enum_REG_H_CAL_11,
    enum_REG_H_CAL_12,
    enum_REG_H_CAL_ADC_0,                           //湿度校准点0，湿度传感器采样值
    enum_REG_H_CAL_ADC_1,
    enum_REG_H_CAL_ADC_2,
    enum_REG_H_CAL_ADC_3,
    enum_REG_H_CAL_ADC_4,
    enum_REG_H_CAL_ADC_5,
    enum_REG_H_CAL_ADC_6,
    enum_REG_H_CAL_ADC_7,
    enum_REG_H_CAL_ADC_8,
    enum_REG_H_CAL_ADC_9,
    enum_REG_H_CAL_ADC_10,
    enum_REG_H_CAL_ADC_11,
    enum_REG_H_CAL_ADC_12,
    enum_REG_BREAK_SECTION_END_2,
    
    enum_REG_HOLD_REG_MAX   = 0x0122,
    
    enum_REG_RHREF_RREQ = 3,
    enum_REG_RHREF_DUTY_CYCLE,
    enum_REG_HEAT_RREQ,
    enum_REG_HEAT_DUTY_CYCLE,
};

enum{
    enum_BIT_PA         = 0,                        //压力报警（1：报警 0：无）
    enum_BIT_PB,                                    //压力闭锁（1：闭锁 0：无）
    enum_BIT_HA,                                    //微水报警（1：报警 0：无）
    enum_BIT_HR,                                    //微水校准（1：校准中，数值未更新 0：测量中，数值更新）
    enum_BIT_EE,                                    //EEPROM故障（1：EEPROM故障 0：正常）
    enum_BIT_RE,                                    //RAM故障（1：RAM故障，0：正常）
    enum_BIT_TE,                                    //温度传感器故障（1：温度传感器故障，0：正常）
    enum_BIT_PE,                                    //压力传感器故障（1：压力传感器故障，0：正常）
    enum_BIT_HE,                                    //湿度传感器故障（1：湿度传感器故障，0：正常）
};

enum input_reg_index{
    enum_REG_STATUS  = 0,                           //状态
    enum_REG_H2O,                                   //微水        常压下微水含量， UI16
    enum_REG_P,                                     //压力        SF6气体压力值，I16
    enum_REG_P20,                                   //压力20      SF6气体压力修正至20℃压力，I16
    enum_REG_RH,                                    //常温下气体相对湿度， UI16
    enum_REG_RH2O,                                  //20℃下气体相对湿度， UI16
    enum_REG_DENSITY,                               //密度        SF6气体密度值 ρ， UI16
    enum_REG_T,                                     //SF6气体温度值，I16
    enum_REG_DEW_POINT_1,                           //当前温度常压下SF6气体露点值，I16
    enum_REG_DEW_POINT_2,                           //当前温度压力下SF6气体露点值，I16
    enum_REG_T_2,                                   //MCU内部温度，I16
    enum_REG_T_ADC,                                 //温度传感器采用值
    enum_REG_TR_ADC,                                //温度参考采用值
    enum_REG_P_ADC,                                 //压力传感器采样值（内部参考）
    enum_REG_PER_ADC,                               //压力传感器采样值（外部参考）
    enum_REG_HS_ADC,                                //湿度传感器采样值（单端，GND REF）
    enum_REG_HZS_ADC,                               //湿度零点参考值（单端, GND REF）
    enum_REG_H_ADC,                                 //湿度传感器采样值（差分，零点参考）
    enum_REG_RES,                                   //保留，未用
    enum_REG_INPUT_REG_MAX,

};    

enum reg_type{
	enum_REG_TYPE_INPUT	= 0,
	enum_REG_TYPE_HOLD,
    enum_REG_TYPE_MAGIC,

	enum_REG_TYPE_MAX,
};

//寄存器属性 
enum reg_attr{
    enum_REG_ATTR_WRONLY	= 0,
    enum_REG_ATTR_RDONLY,
    enum_REG_ATTR_RDWR,
};

template <class T>
struct param_range{
    T       	        m_min;
    T 			        m_max;
    enum reg_attr       m_attr;
};

typedef struct __SENSOR_INFO
{
    char   strSensorType[24];
    char   strSensorSerialNo[24];
    char   strSensorHardwareVer[24];
    char   strSensorSoftwareVer[24];
    char   chksum;
} SENSOR_INFO;

typedef struct __SENSOR_PARA
{
    //chech sum
    unsigned short   chksum;          //编译器在实际编译时 可能对此结构体中的数据有填充行为
                                        //因此将chksum放在首部和尾部的影响不同
//Configure Infomation
    //0-6
    unsigned char   ucLinkAddr;	  //1-247
    unsigned char   ucCommBaud;   //0:2400 1:4800 2:9600 3:19200 4:38400
    unsigned char   ucGasMixingRatio;    //Gas mixing ratio:0~100% SF6
    unsigned char   ucNormalTemperature; //Pressure normalization Temperature
    unsigned char   ucAutoWashON;   //Auto wash
    unsigned char   ucBootWashON;   //Boot wash
    unsigned char   ucMannulWashON; //Mannul wash
    //7-9
    unsigned short  usWashPowerSet; //Wash power
    unsigned short  usHeatPowerSet; //Heat for getting humi-zero voltage
    unsigned short  usHumiZeroSet;  //Humi Zero Setting
//Calibration Setting
    //pressure: 10-13;14-17
    unsigned short  usPValue[4];
    unsigned short  usPADC[4];
    //humidity: 18-30;31-43,44-56
    unsigned short usRH[13];
    unsigned long  ulHumiADC[13];

}SENSOR_PARA,*pSENSOR_PARA;

typedef struct __SENSOR_DATA
{
    unsigned short usStatus;
    unsigned short usPPM;
    signed short   ssPressure;
    signed short   ssPressure20;
    unsigned short usRH;
    unsigned short usRH20;
    unsigned short usDensity;
	signed short   ssTemprature;
	signed short   ssDew;
	signed short   ssDewUnderPress;
	//采样值
	unsigned short usADCPrs;
    unsigned short usADCPrsVs;
    float  fADCTemprature;
    float  fADCHumiSgl;
    float  fADCCapcitance;
} SENSOR_DATA,*pSENSOR_DATA;

//input & output pressure unit
#define PRESS_ABS_INPUT    FALSE
#define PRESS_ABS_OUTPUT   FALSE


enum reg_operate{
	enum_REG_READ 	= 0,
	enum_REG_WRITE,
};

enum reg_access_type{
	enum_REG_UINT8 	= 0,
	enum_REG_UINT16,
	enum_REG_UINT32,
	enum_REG_UINT32_HIGH_16,
	enum_REG_UINT32_LOW_16,
};


//---------------------------------------------------------------------------------------------------------

struct storage_info{
	uint16 					m_storage_addr;      	//存贮器设备地址
	uint8					*m_pdata;				//src data
	uint16 					m_len;					//src data len
};

typedef 	uint32 				portMODBUS_REG_VALUE;
typedef 	uint16 				portMODBUS_REG_INDEX;

#define 	_def_SPLIT_HOLD_R_INDEX_1 				(7)				//usWashPowerSet
#define 	_def_SPLIT_HOLD_R_INDEX_2 				(31)			//ulHumiADC
#define 	_def_SPLIT_HOLD_R_INDEX_3 				(57)			//保持寄存器的结尾
//保持寄存器最大数目  依据实际情况看是否做寄存器超限判定
#define     def_HOLD_REG_MAX_NO                    _def_SPLIT_HOLD_R_INDEX_3


#define 	_def_SPLIT_INPUT_R_INDEX_1 				(12)
#define 	_def_SPLIT_INPUT_R_INDEX_2 				(18)			//输入寄存器的结尾
//输入寄存器最大数目
#define     def_INPUT_REG_MAX_NO                   _def_SPLIT_INPUT_R_INDEX_2



class 	model_datum:noncopyable{
public:
	model_datum(uint8 *pname = NULL);
    ~model_datum(){}

    portBASE_TYPE storage_info_query(enum reg_type type, struct storage_info *pinfo);
    portBASE_TYPE storage_param_verify(enum reg_type type);
    void storage_param_chksum(enum reg_type type);
 
    const uint8 *name_get(void){ return m_name; }
    void name_set(const uint8 *pname){ m_name 	= pname; }

    portMODBUS_REG_VALUE modbus_reg_get(enum reg_type type, portMODBUS_REG_INDEX reg);
    portBASE_TYPE modbus_reg_set(enum reg_type type, portMODBUS_REG_INDEX reg, portMODBUS_REG_VALUE value);

    void modbus_hold_reg_def_init(void);

private:
	portBASE_TYPE reg_operate(enum reg_access_type type, enum reg_operate operate, 
                void *preg_addr, portMODBUS_REG_VALUE &value, void *prange = NULL);
    
    portBASE_TYPE hold_reg_operate(enum reg_operate operate, portMODBUS_REG_INDEX reg, portMODBUS_REG_VALUE &value);
    portBASE_TYPE input_reg_operate(enum reg_operate operate, portMODBUS_REG_INDEX reg, portMODBUS_REG_VALUE &value);

    portMODBUS_REG_VALUE  hold_reg_get(portMODBUS_REG_INDEX reg);
    portBASE_TYPE hold_reg_set(portMODBUS_REG_INDEX reg, portMODBUS_REG_VALUE value);
    portMODBUS_REG_VALUE  input_reg_get(portMODBUS_REG_INDEX reg);
    portBASE_TYPE input_reg_set(portMODBUS_REG_INDEX reg, portMODBUS_REG_VALUE value);

public:
    const uint8             	*m_name;
    const uint8             	*m_magic_setup;
    SENSOR_INFO 				m_stuSensorInfo;
    SENSOR_PARA 				m_stuSensorPara;
    SENSOR_DATA 				m_stuSensorData;
};

#endif


