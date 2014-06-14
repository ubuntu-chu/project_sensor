#include    "cpu_tick.h"
#include    "../../api/utils.h"
#include    "../../includes/includes.h"


void cpu_delay_ms(uint16 ms)
{
    
}

void cpu_delay_us(uint16 us)
{
    
}

static tick_t 		s_tick		= 0;
static portBASE_TYPE 	time_trig_1s = 0;

tick_t cpu_tick_get(void)
{
	return s_tick;
}
void cpu_tick_set(tick_t tick)
{
	portCPSR_TYPE	level 		= cpu_interruptDisable();
	s_tick    					= tick;
	cpu_interruptEnable(level);
}

void tick_handle(void *pvoid)
{
    time_trig_1s 		= 1;
}

portBASE_TYPE cpu_timetrig_1s(void)
{
	portBASE_TYPE rt		= time_trig_1s;

	time_trig_1s 			= 0;

	return (rt)?(1):(0);
}

void cpu_tick_increase(void)
{
	s_tick++;
//	timer_
}

//time: 500 ms  interrupt
portBASE_TYPE cpu_tick_run(void)
{
	timer_handle_type handle_tick;
    
    handle_tick   = t_timer_manage.hard_timer_register(1000, 
                                                SV_TIMER_FLAG_PERIODIC, 
                                                tick_handle, 
                                                NULL,
                                                "tick handle");
    t_timer_manage.timer_start(handle_tick);  

//    SysTick_Config(16*1000*50); // Time-out period of 50ms   FCLK = 16MHz
    SysTick_Config(16*1000*(1000/TICK_PER_SECOND)); // Time-out period of 10ms   FCLK = 16MHz
#if 0
	//Timer 0 setup to re-start every 64mS
	GptLd(pADI_TM0, 50000);// Time-out period of 50ms
	GptCfg(pADI_TM0, TCON_CLK_UCLK, TCON_PRE_DIV16,
			TCON_MOD | TCON_RLD | TCON_ENABLE);  // T0 config, Uclk/16, 1MHz
    NVIC_EnableIRQ(TIMER0_IRQn);
#endif
#if  0    
	//Timer 1 setup
	GptLd(pADI_TM1,0xFFFA);                                  // Set timeout period for 5 seconds
	GptCfg(pADI_TM1,TCON_CLK_LFOSC,TCON_PRE_DIV32768,TCON_MOD_FREERUN|TCON_UP|TCON_RLD|TCON_ENABLE);
	GptCfg(pADI_TM0, TCON_CLK_UCLK, TCON_PRE_DIV16,
			TCON_MOD | TCON_RLD | TCON_ENABLE);  // T0 config, Uclk/16, 1MHz
#endif

	return 0;
}


extern "C" void SysTick_Handler(void)
{
    //portCPSR_TYPE	level = cpu_interruptDisable();
    
	cpu_tick_increase();
    t_timer_manage.timer_check();
    if (cpu_sleep_status_pend()){
        cpu_sleep_exit();
    }
    //cpu_interruptEnable(level);
}

extern "C" void GP_Tmr0_Int_Handler(void)
{
#if 0
    portCPSR_TYPE	level = cpu_interruptDisable();
    
    GptClrInt(pADI_TM0,TSTA_TMOUT);  // Clear T0 interrupt
    t_timer_manage.timer_check();
    if (cpu_sleep_status_pend()){
        cpu_sleep_exit();
    }
    cpu_interruptEnable(level);
#endif
}

extern "C" void GP_Tmr1_Int_Handler(void)
{
}

extern "C" void WDog_Tmr_Int_Handler(void)
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
