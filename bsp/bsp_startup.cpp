#include    "bsp_startup.h"

void sys_clock_init(void)
{
	ClkDis(
			CLKDIS_DISSPI0CLK | CLKDIS_DISSPI1CLK 
					| CLKDIS_DISPWMCLK 
					| CLKDIS_DISDACCLK 
					);
	ClkCfg(CLK_CD0, CLK_HF, CLKSYSDIV_DIV2EN_DIS, CLK_UCLKCG); // Select CD0 for CPU clock - 16Mhz clock
}

void sys_wdt_on(void)
{
	WdtCfg(T3CON_PRE_DIV256,T3CON_IRQ_DIS,T3CON_PD_EN);   // Enable Watchdog timer to reset CPU on time-out
}

void sys_wdt_off(void)
{
	 WdtCfg(T3CON_PRE_DIV1,T3CON_IRQ_EN,T3CON_PD_DIS);  // Disable Watchdog timer resets
}

void sys_wdt_init(void)
{
	sys_wdt_off();
}

void sys_wdt_start(void)
{
    sys_wdt_on();
    WdtLd(0x1000);                                        // Set timeout period to ~32 seconds
    WdtGo(T3CON_ENABLE_EN);                               // Start the watchdog timer
}

void sys_wdt_stop(void)
{
	sys_wdt_off();
}

void sys_wdt_clr(void)
{
	WdtClrInt();  	         // Refresh watchdog timer
}

portBASE_TYPE bsp_startup(void)
{
	//watch dog
    sys_wdt_init();
    sys_wdt_stop();
    //clock
    sys_clock_init();

	//gpio
	cpu_gpioinit();
	//start sys tick
	cpu_sys_tick_run();
    //start sys monitor
//    cpu_sys_monitor_run();
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
