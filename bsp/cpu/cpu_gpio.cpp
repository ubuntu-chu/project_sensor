/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 

#include    "cpu_gpio.h"

portBASE_TYPE cpu_gpioinit(void)
{

	P8DIR |= BIT4;      //ctl pin for uart0-1  power
	P8DIR |= BIT5;      //ctl pin for rf power
	P11DIR |= BIT0;     //ctl pin for 12V power
	P2DIR |= BIT1;      //ctl pin for E2 power

	P2DIR |= BIT0;      //ctl pin for LCD power
	P1DIR |= BIT7;		//ctl pin for LCD_0603  power

	r_oe_dis();
	r_dir_out();

	r_ctl1_off();
	r_ctl2_off();
	r_ctl3_off();
	r_ctl4_off();

	P9DIR |= (BIT6 + BIT7);
	P10DIR |= (BIT0 + BIT1 + BIT2 + BIT3);		//dir for r_oe

	r_oe_en();

	ps_12v_on();
	ps_e2_on();
	ps_rf_on();
	ps_uart01_on();

    return 0;
}

void cpu_led_toggle(void)
{
	static portBASE_TYPE s_toggle = 0;

	if (s_toggle == 1) {
		s_toggle = 0;
		m_led1_off();
	} else {
		s_toggle = 1;
		m_led1_on();
	}
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
