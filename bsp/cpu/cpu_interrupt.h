/******************************************************************************
 * 文件信息 : 
 * 
 * 创 建 者 :  
 *
 * 创建日期 : 
 * 
 * 原始版本 :  
 *     
 * 修改版本 :   
 *    
 * 修改日期 :  
 *
 * 修改内容 : 
 * 
 * 审 核 者 : 
 *
 * 附    注 : 
 *
 * 描    述 :   源代码
 *
 * 版    权 :   
 * 
******************************************************************************/

/******************************************************************************
 *                              头文件卫士
******************************************************************************/ 
 
#ifndef __CPU_INTERRUPT_H 
#define __CPU_INTERRUPT_H

/******************************************************************************
 *                             包含文件声明
******************************************************************************/

#ifndef INCLUDES_LOW_H
	#include    "../../includes/includes-low.h"
#endif

/******************************************************************************
 *                            文件接口函数声明
******************************************************************************/ 

portCPSR_TYPE cpu_interruptDisable(void);
void cpu_interruptEnable(portCPSR_TYPE level);
void cpu_interruptEnableNow(void);
void cpu_enter_critical(void);
void cpu_exit_critical(void);

void cpu_sleep_enter(void);
void cpu_sleep_exit(void);
uint8 cpu_sleep_status_pend(void);

void cpu_pendsv_init(void);
void cpu_pendsv_trig(void);

void cpu_pendsv_register(fp_void_pvoid *fp, void *data);
void cpu_pendsv_unregister(void);


/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif
