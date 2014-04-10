#include "../../includes/Macro.h"
#include "cpu_gpio.h"
#include "cpu_interrupt.h"
#include    "cpu_uart.h"


#define _MONITOR_INVALID_VALUE          (0xff)

static time_t 		sys_tick		= 0;
static portBASE_TYPE 	time_trig_1s = 0;

time_t cpu_sys_time_get(void)
{
	return sys_tick;
}
void cpu_sys_time_set(time_t val)
{
	portCPSR_TYPE	level 		= cpu_interruptDisable();
	sys_tick    = val;
	cpu_interruptEnable(level);
}

//time: 500 ms  interrupt
portBASE_TYPE cpu_sys_tick_run(void)
{
	
    //t_monitor_manage.precision_set(1000);     //unit: ms

	return 0;
}

portBASE_TYPE cpu_timetrig_1s(void)
{
	portBASE_TYPE rt		= time_trig_1s;

	time_trig_1s 			= 0;

	return (rt)?(1):(0);
}

#if 0  
// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
	static portBASE_TYPE s_cnt 	= 0;
    
	s_cnt++;
	if (s_cnt >= 1){
		s_cnt				= 0;
		sys_tick++;
		time_trig_1s 		= 1;
		//t_monitor_manage.run();
//        cpu_led_toggle();
		if (cpu_sleep_status_pend()){
			__bic_SR_register_on_exit(LPM4_bits);   // Exit active CPU
		}
	}
}
#endif



/*********************************************************************************
**                            End Of File
*********************************************************************************/
