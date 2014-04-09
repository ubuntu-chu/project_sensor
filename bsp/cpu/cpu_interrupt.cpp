/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "cpu_interrupt.h"
#include "../../includes/macro.h"

/******************************************************************************
 *                       本文件所定义的静态全局变量
******************************************************************************/ 


/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                          
 *                                                                             
 *  出口参数 :                                                              
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 : 系统中的中端应该由其一套中断管理体系进行管理                                                            
 *                                                                            
 *                                                                            
******************************************************************************/ 

static uint8 nesting_level 	= 0;

void cpu_enter_critical(void)
{
#ifndef		__LINUX_OS__
    _BIC_SR(GIE);
#endif
    nesting_level++;
}

void cpu_exit_critical(void)
{
   	nesting_level--;
	if (0 == nesting_level){
#ifndef		__LINUX_OS__
		_BIS_SR(GIE);
#endif
	} 
}

//max nesting level is 255
portCPSR_TYPE cpu_interruptDisable(void)
{
#ifndef		__LINUX_OS__
    _BIC_SR(GIE);
#endif
          
    return TRUE;
}

void cpu_interruptEnable(portCPSR_TYPE level)
{
#ifndef		__LINUX_OS__
    _BIS_SR(GIE);
#endif
}

void cpu_interruptEnableNow(void)
{
	nesting_level 					= 0;
#ifndef		__LINUX_OS__
    _BIS_SR(GIE);
#endif
}

static volatile uint8 cpu_sleep_status = 0;

void cpu_sleep_enter(void)
{
	cpu_sleep_status 	= 1; 				   //atomic ops
    __bis_SR_register(LPM4_bits + GIE);       // Enter LPM0, enable interrupts
    __no_operation();                         // For debugger
}

uint8 cpu_sleep_status_pend(void)
{
   uint8 tmp 	= cpu_sleep_status;

   cpu_sleep_status 			= 0;
   return tmp;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
