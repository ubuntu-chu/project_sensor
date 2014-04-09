#include    "bsp_startup.h"

void sys_clock_init(void)
{
	UCSCTL1 = DCORSEL_4;							  // 选择FLL参考源自REFO  12-28MHz
//  P7SEL &= ~(BIT1 + BIT0);
//	UCSCTL2 |= 0x1079; 								// Set DCO Multiplier for 8MHz
//	UCSCTL3 |= SELREF1;                             // select REFOCLK
    P7SEL |= (BIT1 + BIT0);
    UCSCTL2 |= 0x10F3; 								// Set DCO Multiplier for 16 MHz
    UCSCTL3 |= SELA__XT1CLK;                        // select XT1 for FLLREF


	UCSCTL4 = SELM__DCOCLK + SELA__XT1CLK + SELS__DCOCLK; 			 // 配置 MCLK = DCOC,SMCLK =DCOC,ACLK=
	UCSCTL6 = XT2OFF + XT1OFF;
	while (SFRIFG1 & OFIFG)						  //清除OFIFG,and  XT1OFFG ,DCOFFG
	{
		UCSCTL7 &= ~(	XT1LFOFFG + DCOFFG);
		SFRIFG1 &= ~OFIFG;
	}
}

void sys_wdt_off(void)
{
	WDTCTL =  WDTPW + WDTHOLD;
}

void sys_wdt_conf(void)
{
// 	WDTCTL =  WDT_ARST_1000;
    sys_wdt_off();

}

void sys_wdt_clr(void)
{
	sys_wdt_conf();
}

portBASE_TYPE bsp_startup(void)
{
	//watch dog
    sys_wdt_conf();
    //clock
    sys_clock_init();

	//gpio
	cpu_gpioinit();
	//start sys tick
	cpu_sys_tick_run();
    //start sys monitor
    cpu_sys_monitor_run();
	//uart
	uart_ctl_init();
	uart_init(DBG_UART, 9600);
    
	//init hal
    hal_init();
    //net queue init
    net_queue_init();
    //enable int
	cpu_interruptEnableNow();

//	while (1){
//
////		_delay_ms(1);
//		_delay_us(40);
//		cpu_led_toggle();
	//}
    
    return 0;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
