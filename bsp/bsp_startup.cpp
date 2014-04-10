#include    "bsp_startup.h"

void sys_clock_init(void)
{
	
}

void sys_wdt_off(void)
{
	
}

void sys_wdt_conf(void)
{
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
