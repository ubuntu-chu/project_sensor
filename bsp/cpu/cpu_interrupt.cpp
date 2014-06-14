/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "cpu_interrupt.h"
#include "../../includes/macro.h"

#if 0
static uint8 nesting_level 	= 0;

void cpu_enter_critical(void)
{
#ifndef		__LINUX_OS__
    
#endif
    nesting_level++;
}

void cpu_exit_critical(void)
{
   	nesting_level--;
	if (0 == nesting_level){
#ifndef		__LINUX_OS__
		
#endif
	} 
}
#endif

//max nesting level is 255
portCPSR_TYPE cpu_interruptDisable(void)
{
#ifndef		__LINUX_OS__
    //return rt_hw_interrupt_disable();
    register uint32_t __regPriMask         __ASM("primask");
    register portCPSR_TYPE rt   = __regPriMask; 
    __regPriMask = 1;
    
    return rt;
#endif 
}

void cpu_interruptEnable(portCPSR_TYPE level)
{
#ifndef		__LINUX_OS__
    register uint32_t __regPriMask         __ASM("primask");
    __regPriMask = level;
    //hw_interrupt_enable(level);
#endif
}

void cpu_interruptEnableNow(void)
{
#ifndef		__LINUX_OS__
    register uint32_t __regPriMask         __ASM("primask");
    __regPriMask = 0;
#endif
}

static volatile uint8 cpu_sleep_status = 0;

void cpu_sleep_enter(void)
{
	cpu_sleep_status 	= 1; 				   //atomic ops
}

void cpu_sleep_exit(void)
{
	cpu_sleep_status 	= 0; 				   //atomic ops
}

uint8 cpu_sleep_status_pend(void)
{
   uint8 tmp 	= cpu_sleep_status;

   cpu_sleep_status 			= 0;
   return tmp;
}

void cpu_pendsv_init(void)
{ 
    //lowest pri
    NVIC_SetPriority(PendSV_IRQn, PEND_SV_PRIO);
}

void cpu_pendsv_trig(void)
{
    SCB->ICSR                     = SCB_ICSR_PENDSVSET_Msk;
}

static fp_void_pvoid *fp_pendsv = NULL;
static void *pendsv_data = NULL;

void cpu_pendsv_register(fp_void_pvoid *fp, void *pvoid)
{
    fp_pendsv                   = fp;
    pendsv_data                 = pvoid;
}

void cpu_pendsv_unregister(void)
{
    fp_pendsv                   = NULL;
}

extern "C" void PendSV_Handler(void)
{
    if (NULL != fp_pendsv){
        fp_pendsv(pendsv_data);
    }
}


/*********************************************************************************
**                            End Of File
*********************************************************************************/
