#include    "bsp_startup.h"

void sys_clock_init(void)
{
	ClkDis(
			CLKDIS_DISSPI0CLK | CLKDIS_DISSPI1CLK 
					| CLKDIS_DISDACCLK 
					);
	ClkCfg(CLK_CD0, CLK_HF, CLKSYSDIV_DIV2EN_DIS, CLK_UCLKCG); // Select CD0 for CPU clock - 16Mhz clock
    //spi - 16Mhz     iic - 16Mhz        uart - 16Mhz(uart clock will be changed by uart_init)          pwm - 16Mhz
    ClkSel(CLK_CD0, CLK_CD0, CLK_CD0, CLK_CD0);
    //fclk hclk pclk - 16Mhz
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
    cpu_pendsv_init();
	//gpio
	cpu_gpioinit();
	//start sys tick
	cpu_tick_run();
    //start sys monitor
//    cpu_sys_monitor_run();
	//uart
	//init hal
    hal_init();
#ifdef		SKBUF_QUEUE
    //net queue init
    net_queue_init();
#endif
    //enable int
	cpu_interruptEnableNow();
    
    return 0;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
