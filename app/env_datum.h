#ifndef _ENV_DATUM_H
#define _ENV_DATUM_H
#include "../includes/includes.h"

enum hold_reg_index{
    enum_REG_MODBUS_ADDR  = 0,
    enum_REG_RHREF_RREQ,
    enum_REG_RHREF_DUTY_CYCLE,
    enum_REG_HEAT_RREQ,
    enum_REG_HEAT_DUTY_CYCLE,
    enum_REG_HOLD_REG_MAX,
};

#define REGS_MODBUS_ADDR()          (m_regs.m_tab_registers[enum_MODBUS_ADDR_INDEX])
#define REGS_RHREF_RREQ()           (m_regs.m_tab_registers[enum_RHREF_RREQ_INDEX])
#define REGS_RHREF_DUTY_CYCLE()     (m_regs.m_tab_registers[enum_RHREF_DUTY_CYCLE_INDEX])
#define REGS_HEAT_RREQ()            (m_regs.m_tab_registers[enum_HEAT_RREQ_INDEX])
#define REGS_HEAT_DUTY_CYCLE()      (m_regs.m_tab_registers[enum_HEAT_DUTY_CYCLE_INDEX])

#define     def_NR_TAB_BITS                 (1)
#define     def_NR_TAB_INPUT_BITS           (1)
#define     def_NR_TAB_INPUT_REGS           (10)
#define     def_NR_TAB_REGS                 (enum_REG_HOLD_REG_MAX)


struct regs{
    uint8 			        m_tab_bits[def_NR_TAB_BITS];
    uint8 			        m_tab_input_bits[def_NR_TAB_INPUT_BITS];
    uint16 			        m_tab_input_registers[def_NR_TAB_INPUT_REGS];
    uint16			        m_tab_registers[def_NR_TAB_REGS];
};

class 	CModelInfo{
public:
	CModelInfo(uint8 *pname):m_name(pname)
    {
        hold_reg_set(enum_REG_MODBUS_ADDR, 1);
    }
	CModelInfo()
    {
        hold_reg_set(enum_REG_MODBUS_ADDR, 1);
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
    struct regs             m_regs;
};

extern class CModelInfo 	t_modeinfo;










#endif


