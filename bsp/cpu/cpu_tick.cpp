#include    "cpu_tick.h"
#include    "../../api/utils.h"


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
	
    t_monitor_manage.precision_set(1000);     //unit: ms
#if 0
	//Timer 0 setup to re-start every 64mS
	GptLd(pADI_TM0, 0x1000);// Time-out period of 256 clock pulese
	GptCfg(pADI_TM0, TCON_CLK_UCLK, TCON_PRE_DIV256,
			TCON_MOD | TCON_RLD | TCON_ENABLE);  // T0 config, Uclk/256,
#endif
	//Timer 1 setup
	   GptLd(pADI_TM1,0xFFFA);                                  // Set timeout period for 5 seconds
	   GptCfg(pADI_TM1,TCON_CLK_LFOSC,TCON_PRE_DIV32768,TCON_MOD_PERIODIC|TCON_UP|TCON_RLD|TCON_ENABLE);
	   NVIC_EnableIRQ(TIMER1_IRQn);                          // Enable Timer1 IRQ

	return 0;
}

portBASE_TYPE cpu_timetrig_1s(void)
{
	portBASE_TYPE rt		= time_trig_1s;

	time_trig_1s 			= 0;

	return (rt)?(1):(0);
}

extern "C" void GP_Tmr1_Int_Handler(void)
{
    static portBASE_TYPE s_loop 	= 0;
    
    GptClrInt(pADI_TM1,TSTA_TMOUT);  // Clear T1 interrupt
   
	s_loop++;
	if (s_loop >= 1){
		s_loop				= 0;
		sys_tick++;
		time_trig_1s 		= 1;
		t_monitor_manage.run();
        //cpu_led_toggle();
		if (cpu_sleep_status_pend()){
			cpu_sleep_exit();
		}
	}
}


void GP_Tmr0_Int_Handler(void)
{
}


void WDog_Tmr_Int_Handler(void)
{
}

#if 0  
// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
	
}
#endif



/*********************************************************************************
**                            End Of File
*********************************************************************************/
