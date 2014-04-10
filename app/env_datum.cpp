/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 
#include "env_datum.h"


portBASE_TYPE 	errno	= 0;


#define		_MODEL_NAME 			("card_term")
//volt:3.0 V
#define     _BATTERY_VOLT           (30)
//type  code
#define def_RFID_TYPE 				(0x0d)
#define def_SENSOR_TYPE 			(def_RFID_TYPE)
//follow the device
#define def_DEV_ID					(0x0d000003)
//fixed
#define def_SENSOR_ID			    (0x0e000001)

//global model info
class CModelInfo 	c_modeinfo((uint8 *)_MODEL_NAME, _BATTERY_VOLT,
                               def_SENSOR_TYPE, def_DEV_ID, def_SENSOR_ID);

