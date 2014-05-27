#ifndef _ENV_DATUM_H
#define _ENV_DATUM_H
#include "../includes/includes.h"

enum hold_reg_index{
    enum_REG_MODBUS_ADDR  = 0,                      //�豸��ַ��ȡֵ��Χ��1-247�����㲥��ַ0
    enum_REG_COMMU_BPS,                             //ͨ�����ʣ�0��:2400��1��4800��2��9600��3��19200��4��38400��
    enum_REG_GAS_MIX_RATIO,                         //�����ϱ��ʣ�0~100%��SF6��                    
    enum_REG_P_CORRECTION_FACTOR,                   //ѹ�������¶ȣ�Ĭ��20�棬��λ0.1�棩
    enum_REG_H_SENSOR_AUTO_CLR,                     //ʪ�ȴ������Զ���ϴ���أ�1��on��0��off��
    enum_REG_H_SENSOR_POWER_ON_CLR,                 //ʪ�ȴ�����������ϴ���أ�1��on��0��off��
    enum_REG_H_SENSOR_MANUAL_CLR,                   //ʪ�ȴ������ֶ���ϴ���أ�1��on��0��off��
    enum_REG_H_SENSOR_CLR_POWER_SET,                //ʪ�ȴ�������ϴ�������ã�0~0xFFFF, PWMռ�ձȣ�
    enum_REG_H_SENSOR_ZERO_CAL_SET,                 //ʪ�ȴ��������У׼���ã�0~0xFFFF, PWMռ�ձȣ�    -Heat
    enum_REG_H_SENSOR_ZERO_REF_SET,                 //ʪ�ȴ��������ο����ã�0~0xFFFF, PWMռ�ձȣ�    -RHref
    
    enum_REG_P_CAL_0    = 257,                      //ѹ��У׼��0 ѹ������λhPa����ѹ���ο�1000hPa
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

#define     def_NR_TAB_BITS                 (1)
#define     def_NR_TAB_INPUT_BITS           (1)


struct regs{
    uint8 			        m_tab_bits[def_NR_TAB_BITS];
    uint8 			        m_tab_input_bits[def_NR_TAB_INPUT_BITS];
    //input regs
    uint16 			        m_tab_input_registers[enum_REG_INPUT_REG_MAX];
    //hold regs
    uint16			        m_tab_registers[enum_REG_HOLD_REG_MAX];
};

enum reg_type{
	enum_REG_TYPE_INPUT	= 0,
	enum_REG_TYPE_HOLD,
    enum_REG_TYPE_MAGIC,

	enum_REG_TYPE_MAX,
};

struct storage_info{
	uint16 					m_storage_addr;      	//�������豸��ַ
	uint8					*m_pdata;				//src data
	uint16 					m_len;					//src data len
};

class 	CModelInfo{
public:
	CModelInfo(uint8 *pname):m_name(pname)
    {
        hold_reg_set(enum_REG_MODBUS_ADDR, 1);
        m_magic_setup           = "magic setup string";
    }
	CModelInfo()
    {
        hold_reg_set(enum_REG_MODBUS_ADDR, 1);
        m_magic_setup           = "magic setup string";
    }
    ~CModelInfo(){}
    
    uint16  hold_reg_get(enum hold_reg_index index)
    {
        return m_regs.m_tab_registers[index];
    }
    void hold_reg_set(enum hold_reg_index index, uint16 value)
    {
        m_regs.m_tab_registers[index] = value;
    }

    uint16  input_reg_get(enum input_reg_index index)
    {
        return m_regs.m_tab_input_registers[index];
    }
    void input_reg_set(enum input_reg_index index, uint16 value)
    {
        m_regs.m_tab_input_registers[index] = value;
    }

    portBASE_TYPE storage_info_query(enum reg_type type, struct storage_info *pinfo)
    {
    	if (pinfo == NULL){
    		return -1;
    	}
    	if (type == enum_REG_TYPE_HOLD){
    		pinfo->m_storage_addr 						= ROUND_UP((strlen(reinterpret_cast<const char *>(m_magic_setup))), 16);
    		pinfo->m_pdata 								= reinterpret_cast<uint8 *>(&m_regs.m_tab_registers[0]);
    		pinfo->m_len 								= sizeof(m_regs.m_tab_registers);
        }else if (type == enum_REG_TYPE_MAGIC){
            pinfo->m_storage_addr 						= 0;
            pinfo->m_pdata 								= const_cast<uint8 *>(m_magic_setup);
    		pinfo->m_len 								= strlen(reinterpret_cast<const char *>(m_magic_setup)) + 1;   //contain end code
    	}else {
    		return -1;
    	}

    	return 0;
    }

    const uint8 *name_get(void)
    {
        return m_name;
    }
    void name_set(const uint8 *pname)
    {
        m_name 				= pname;
    }

private:
    CModelInfo( CModelInfo &other);
    CModelInfo &operator =( CModelInfo &other);

public:
    const uint8             *m_name;
    const uint8             *m_magic_setup;
    struct regs             m_regs;


};

#endif


