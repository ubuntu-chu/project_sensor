#ifndef _ENV_DATUM_H
#define _ENV_DATUM_H

#include "../includes/includes.h"

#define     def_NR_TAB_BITS                 1
#define     def_NR_TAB_INPUT_BITS           1
#define     def_NR_TAB_INPUT_REGS           10
#define     def_NR_TAB_REGS                 10


class 	CModelInfo{
public:
	CModelInfo(uint8 *pname):m_name(pname){}
    ~CModelInfo(){}
    

private:
    CModelInfo( CModelInfo &other);
    CModelInfo &operator =( CModelInfo &other);

public:
    uint8                   *m_name;
    uint8 			        m_tab_bits[def_NR_TAB_BITS];
    uint8 			        m_tab_input_bits[def_NR_TAB_INPUT_BITS];
    uint16 			        m_tab_input_registers[def_NR_TAB_INPUT_REGS];
    uint16			        m_tab_registers[def_NR_TAB_REGS];
};

extern class CModelInfo 	t_modeinfo;


extern portBASE_TYPE 	errno;







#endif


