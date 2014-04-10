/******************************************************************************
 *                              ͷ�ļ���ʿ
******************************************************************************/ 
 
#ifndef __CPU_TICK_H
#define __CPU_TICK_H

#include "../../includes/macro.h"

/******************************************************************************
 *                             ���ļ�����
******************************************************************************/

#ifndef    _INCLUDES_H_
	#include    "../../includes/includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *                            �ļ��ӿں�������
******************************************************************************/ 

time_t cpu_sys_time_get(void);
void cpu_sys_time_set(time_t val);
portBASE_TYPE cpu_sys_tick_run(void);
portBASE_TYPE cpu_timetrig_1s(void);


void uart_comm_monitor_set(void);
void uart_comm_monitor_clr(void);

portuBASE_TYPE cpu_sys_monitor_run(void);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
