#ifndef _ENV_DATUM_H
#define _ENV_DATUM_H

#include "../includes/includes.h"



class 	CModelInfo{
public:
	CModelInfo(uint8 *pname, uint8 battery_volt, uint8 dev_type, 
               uint32 dev_addr, uint32 sensor_addr):m_name(pname),
                m_battery_volt(battery_volt), m_dev_type(dev_type), 
                m_dev_addr(dev_addr), m_sensor_addr(sensor_addr){}
    ~CModelInfo(){}
    
    uint8	battery_volt_get(void) {return m_battery_volt;}
    uint8	dev_type_get(void) {return m_dev_type;}
    uint32	dev_addr_get(void) {return m_dev_addr;}
    uint32	sensor_addr_get(void) {return m_sensor_addr;}

private:
    CModelInfo( CModelInfo &other);
    CModelInfo &operator =( CModelInfo &other);

     uint8 			    *m_name;

     uint8              m_battery_volt;
     uint8				m_dev_type;
     uint32				m_dev_addr;
     uint32				m_sensor_addr;
};

extern class CModelInfo 	c_modeinfo;


extern portBASE_TYPE 	errno;







#endif


