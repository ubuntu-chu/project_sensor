#ifndef _DATUM_H
#define _DATUM_H
#include "../includes/includes.h"

enum hold_reg_index{
    enum_REG_BREAK_SECTION_START_1 = 0,
    enum_REG_MODBUS_ADDR  = enum_REG_BREAK_SECTION_START_1,                      //�豸��ַ��ȡֵ��Χ��1-247�����㲥��ַ0
    enum_REG_COMMU_BPS,                             //ͨ�����ʣ�0��:2400��1��4800��2��9600��3��19200��4��38400��
    enum_REG_GAS_MIX_RATIO,                         //�����ϱ��ʣ�0~100%��SF6��                    
    enum_REG_P_CORRECTION_FACTOR,                   //ѹ�������¶ȣ�Ĭ��20�棬��λ0.1�棩
    enum_REG_H_SENSOR_AUTO_CLR,                     //ʪ�ȴ������Զ���ϴ���أ�1��on��0��off��
    enum_REG_H_SENSOR_POWER_ON_CLR,                 //ʪ�ȴ�����������ϴ���أ�1��on��0��off��
    enum_REG_H_SENSOR_MANUAL_CLR,                   //ʪ�ȴ������ֶ���ϴ���أ�1��on��0��off��
    enum_REG_H_SENSOR_CLR_POWER_SET,                //ʪ�ȴ�������ϴ�������ã�0~0xFFFF, PWMռ�ձȣ�
    enum_REG_H_SENSOR_ZERO_CAL_SET,                 //ʪ�ȴ��������У׼���ã�0~0xFFFF, PWMռ�ձȣ�    -Heat
    enum_REG_H_SENSOR_ZERO_REF_SET,                 //ʪ�ȴ��������ο����ã�0~0xFFFF, PWMռ�ձȣ�    -RHref
    enum_REG_BREAK_SECTION_END_1,
    
    
    enum_REG_BREAK_SECTION_START_2 = 257,
    enum_REG_P_CAL_0    = enum_REG_BREAK_SECTION_START_2,  //ѹ��У׼��0 ѹ������λhPa����ѹ���ο�1000hPa
    enum_REG_P_CAL_1,
    enum_REG_P_CAL_2,
    enum_REG_P_CAL_3,
    enum_REG_P_CAL_ADC_0,                           //ѹ��У׼��0��ѹ������������ֵPadc
    enum_REG_P_CAL_ADC_1,
    enum_REG_P_CAL_ADC_2,
    enum_REG_P_CAL_ADC_3,
    
    enum_REG_H_CAL_0,                               //ʪ��У׼��0��ʪ�ȵ�λ��0.01%rh���ο� 0%RH
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
    enum_REG_H_CAL_ADC_0,                           //ʪ��У׼��0��ʪ�ȴ���������ֵ
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
    enum_BIT_PA         = 0,                        //ѹ��������1������ 0���ޣ�
    enum_BIT_PB,                                    //ѹ��������1������ 0���ޣ�
    enum_BIT_HA,                                    //΢ˮ������1������ 0���ޣ�
    enum_BIT_HR,                                    //΢ˮУ׼��1��У׼�У���ֵδ���� 0�������У���ֵ���£�
    enum_BIT_EE,                                    //EEPROM���ϣ�1��EEPROM���� 0��������
    enum_BIT_RE,                                    //RAM���ϣ�1��RAM���ϣ�0��������
    enum_BIT_TE,                                    //�¶ȴ��������ϣ�1���¶ȴ��������ϣ�0��������
    enum_BIT_PE,                                    //ѹ�����������ϣ�1��ѹ�����������ϣ�0��������
    enum_BIT_HE,                                    //ʪ�ȴ��������ϣ�1��ʪ�ȴ��������ϣ�0��������
};

enum input_reg_index{
    enum_REG_STATUS  = 0,                           //״̬
    enum_REG_H2O,                                   //΢ˮ        ��ѹ��΢ˮ������ UI16
    enum_REG_P,                                     //ѹ��        SF6����ѹ��ֵ��I16
    enum_REG_P20,                                   //ѹ��20      SF6����ѹ��������20��ѹ����I16
    enum_REG_RH,                                    //�������������ʪ�ȣ� UI16
    enum_REG_RH2O,                                  //20�����������ʪ�ȣ� UI16
    enum_REG_DENSITY,                               //�ܶ�        SF6�����ܶ�ֵ �ѣ� UI16
    enum_REG_T,                                     //SF6�����¶�ֵ��I16
    enum_REG_DEW_POINT_1,                           //��ǰ�¶ȳ�ѹ��SF6����¶��ֵ��I16
    enum_REG_DEW_POINT_2,                           //��ǰ�¶�ѹ����SF6����¶��ֵ��I16
    enum_REG_T_2,                                   //MCU�ڲ��¶ȣ�I16
    enum_REG_T_ADC,                                 //�¶ȴ���������ֵ
    enum_REG_TR_ADC,                                //�¶Ȳο�����ֵ
    enum_REG_P_ADC,                                 //ѹ������������ֵ���ڲ��ο���
    enum_REG_PER_ADC,                               //ѹ������������ֵ���ⲿ�ο���
    enum_REG_HS_ADC,                                //ʪ�ȴ���������ֵ�����ˣ�GND REF��
    enum_REG_HZS_ADC,                               //ʪ�����ο�ֵ������, GND REF��
    enum_REG_H_ADC,                                 //ʪ�ȴ���������ֵ����֣����ο���
    enum_REG_RES,                                   //������δ��
    enum_REG_INPUT_REG_MAX,

};    

enum reg_type{
	enum_REG_TYPE_INPUT	= 0,
	enum_REG_TYPE_HOLD,
    enum_REG_TYPE_MAGIC,

	enum_REG_TYPE_MAX,
};

//�Ĵ������� 
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
    unsigned short   chksum;          //��������ʵ�ʱ���ʱ ���ܶԴ˽ṹ���е������������Ϊ
                                        //��˽�chksum�����ײ���β����Ӱ�첻ͬ
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
	//����ֵ
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
	uint16 					m_storage_addr;      	//�������豸��ַ
	uint8					*m_pdata;				//src data
	uint16 					m_len;					//src data len
};

typedef 	uint32 				portMODBUS_REG_VALUE;
typedef 	uint16 				portMODBUS_REG_INDEX;

#define 	_def_SPLIT_HOLD_R_INDEX_1 				(7)				//usWashPowerSet
#define 	_def_SPLIT_HOLD_R_INDEX_2 				(31)			//ulHumiADC
#define 	_def_SPLIT_HOLD_R_INDEX_3 				(57)			//���ּĴ����Ľ�β
//���ּĴ��������Ŀ  ����ʵ��������Ƿ����Ĵ��������ж�
#define     def_HOLD_REG_MAX_NO                    _def_SPLIT_HOLD_R_INDEX_3


#define 	_def_SPLIT_INPUT_R_INDEX_1 				(12)
#define 	_def_SPLIT_INPUT_R_INDEX_2 				(18)			//����Ĵ����Ľ�β
//����Ĵ��������Ŀ
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


