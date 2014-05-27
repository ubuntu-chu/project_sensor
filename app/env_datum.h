#ifndef _ENV_DATUM_H
#define _ENV_DATUM_H
#include "../includes/includes.h"

enum hold_reg_index{
    enum_REG_MODBUS_ADDR  = 0,                      //设备地址，取值范围（1-247）；广播地址0
    enum_REG_COMMU_BPS,                             //通信速率（0：:2400，1：4800，2：9600，3：19200，4：38400）
    enum_REG_GAS_MIX_RATIO,                         //气体混合比率（0~100%，SF6）                    
    enum_REG_P_CORRECTION_FACTOR,                   //压力修正温度（默认20℃，单位0.1℃）
    enum_REG_H_SENSOR_AUTO_CLR,                     //湿度传感器自动清洗开关（1：on，0：off）
    enum_REG_H_SENSOR_POWER_ON_CLR,                 //湿度传感器开机清洗开关（1：on，0：off）
    enum_REG_H_SENSOR_MANUAL_CLR,                   //湿度传感器手动清洗开关（1：on，0：off）
    enum_REG_H_SENSOR_CLR_POWER_SET,                //湿度传感器清洗功率设置（0~0xFFFF, PWM占空比）
    enum_REG_H_SENSOR_ZERO_CAL_SET,                 //湿度传感器零点校准设置（0~0xFFFF, PWM占空比）    -Heat
    enum_REG_H_SENSOR_ZERO_REF_SET,                 //湿度传感器零点参考设置（0~0xFFFF, PWM占空比）    -RHref
    
    enum_REG_P_CAL_0    = 257,                      //压力校准点0 压力，单位hPa，绝压，参考1000hPa
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
	uint16 					m_storage_addr;      	//存贮器设备地址
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


