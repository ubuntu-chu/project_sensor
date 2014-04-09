/******************************************************************************
 *                          锟斤拷锟侥硷拷锟斤拷锟斤拷锟矫碉拷头锟侥硷拷
******************************************************************************/ 

//#include "cpu_tick.h"


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
//    TA0CTL = TASSEL_2 + MC_0 + TACLR;
//    TA1CCR0 = (uint16)(((uint32)def_MCLK*ms)/1000);
//    TA1CCR0 += (uint16)((uint32)TA1CCR0*5*ms/100);
//	TA1CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, contmode, clear TAR

    TA0CTL = TASSEL_1 + MC_0 + TACLR;
    //TA1CCR0 = 16384;
    TA1CCR0 = 32768;
	TA1CTL = TASSEL_1 + MC_1 + TACLR;         // SMCLK, contmode, clear TAR
	
    t_monitor_manage.precision_set(1000);     //unit: ms
    TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled

	return 0;
}

portBASE_TYPE cpu_timetrig_1s(void)
{
	portBASE_TYPE rt		= time_trig_1s;

	time_trig_1s 			= 0;

	return (rt)?(1):(0);
}

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
		t_monitor_manage.run();
//        cpu_led_toggle();
		if (cpu_sleep_status_pend()){
			__bic_SR_register_on_exit(LPM4_bits);   // Exit active CPU
		}
	}
}


//-------------------------------------------------------------------------------------------


void uart_comm_monitor_set(void)
{
#if 0
	uint8	ms		= 100;

	TBCTL 		= TBSSEL_1 + MC_0 + TBCLR;          // SMCLK, upmode, clear TBR
    TBCCR0 		 = (uint16)(((uint32)32768*ms)/1000);
	TBCTL 		= TBSSEL_1 + MC_1 + TBCLR;          // SMCLK, upmode, clear TBR
	TBCCTL0 	= CCIE;                           // TBCCR0 interrupt enabled
#endif
}

void uart_comm_monitor_clr(void)
{
#if 0
	TBCTL 		= TBSSEL_1 + MC_0 + TBCLR;          // SMCLK, upmode, clear TBR
	TBCCTL0 	= 0;                           // TBCCR0 interrupt disabled
#endif
}
#if 0

// Timer B0 interrupt service routine
#pragma vector=TIMERB0_VECTOR
__interrupt void TIMERB0_ISR (void)
{
	com_info_framedone_set(UART_0, UART_RECV_STATT_TIMEOUT);
	uart_comm_monitor_clr();
}
#endif

#if 1
//monitor_manage 	t_monitor_manage;

portuBASE_TYPE cpu_sys_monitor_run(void)
{
#if 0
	uint8	ms		= 500;							//ms  interrupt

	TBCTL 		= TBSSEL_1 + MC_0 + TBCLR;          // SMCLK, upmode, clear TBR
    TBCCR0 		 = (uint16)(((uint32)32768*ms)/1000);
	TBCTL 		= TBSSEL_1 + MC_1 + TBCLR;          // SMCLK, upmode, clear TBR
	TBCCTL0 	= CCIE;                           // TBCCR0 interrupt enabled

	t_monitor_manage.precision_set(1000/ms);
#endif
	return 0;
}

// Timer B0 interrupt service routine
#pragma vector=TIMERB0_VECTOR
__interrupt void TIMERB0_ISR (void)
{
	//t_monitor_manage.run();
}
#endif

/*********************************************************************************
**                            End Of File
*********************************************************************************/
