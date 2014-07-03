#ifndef _MAIN_H_
#define _MAIN_H_

#include "../includes/includes.h"
#if 1
#include "devices.h"
#include "protocol.h"
#include "modbus.h"
#include "datum.h"
#include "../reactor/reactor.h"

enum app_status{
	enum_STAT_OK				= 0,
	enum_STAT_DATUM_FAILED,
};

enum datum_load_stat{
	enum_DATUM_LOAD_STAT_FAILED		= 0,
	enum_DATUM_LOAD_STAT_SUCCESS,
	enum_DATUM_LOAD_STAT_RETRY,
};

enum datum_load_ctrl{
	enum_DATUM_LOAD_CTRL_RECOVER		= 0,
	enum_DATUM_LOAD_CTRL_NONE,
};


enum{
	MODE_IDLE_ENTER 	= 0,
	MODE_IDLE_EXIT,
};

enum RUNMODE{ MODE_MEASURE_PARA=0,
    MODE_SENSOR_WASH,
    MODE_CALIB_HUMI,
};

enum ADC1_RUN_STATUS {
    ADC_T					= 0,
    CALC_T,
    ADC_RHS,
    CALC_RH,
    ADC_PRS,
    CALC_P,
    CALC_D,
    CALC_PPM,
    CALC_DEW,
};

struct _app_runinfo_{
    device        				*m_pdevice_commu;
    device        				*m_pdevice_ad;
    device        				*m_pdevice_log;
    device        				*m_pdevice_pin;
    device        				*m_pdevice_storage;
    device        				*m_pdevice_pwm;
    protocol					*m_pprotocol;
	timer_handle_type 			m_handle_period;
	enum app_status				m_status;
	uint8 						m_mode;
	bool						m_startupcalib;
	uint8						m_ucCalibRecordTimes;
	uint32						m_uiCalibRecTimeGap;


	enum RUNMODE 				ucSensorRunMode;
	enum ADC1_RUN_STATUS 		ucADC1RunStus;

	int 						iADCTemp[def_RECORD_LEN];
	int 						iADCRHS[def_RECORD_LEN];


};
typedef struct _app_runinfo_ app_runinfo_t;

class application{
    DEFINE_SINGLETON_CLASS(application)
public:
    portBASE_TYPE run(void);
    portBASE_TYPE init(void);
    
    void mode_set(uint8 mode){m_app_runinfo.m_mode = mode;}
    uint8 mode_get(void){return m_app_runinfo.m_mode;}

private:
    application(){}
    ~application(){}
    application(const application &other);
    application &operator =(const application &other);
	static portBASE_TYPE package_event_handler(void *pvoid, enum protocol_phase phase, class protocol_info *pinfo);
	static void pendsv_handle(void *pdata);
	static void self_calib_handle(void *pdata);
    static void period_handle(void *pdata);

	static int event_handle_ad(void *pvoid, int event_type, class buffer &buf, class Timestamp &ts);

	portBASE_TYPE datum_load(void);
	enum datum_load_stat 	_datum_load(enum datum_load_ctrl ctrl);
	static portBASE_TYPE   datum_save(void *pvoid, class event *pevent);

    struct _app_runinfo_    m_app_runinfo;
	class model_datum 	     m_modelinfo;

	eventloop 				*m_peventloop;
};
    
#endif












#endif


