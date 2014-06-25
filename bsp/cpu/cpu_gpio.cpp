/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 

#include    "cpu_gpio.h"

portBASE_TYPE cpu_gpioinit(void)
{
#ifdef def_RUN_IN_EVB 
    DioOen(pADI_GP1,0x8);
#endif


    return 0;
}

void cpu_led_toggle(void)
{
#if 0
	static portBASE_TYPE s_toggle = 0;

	if (s_toggle == 1) {
		s_toggle = 0;
		//m_led1_off();
	} else {
		s_toggle = 1;
		//m_led1_on();
	}
#endif
#ifdef def_RUN_IN_EVB 
    DioTgl(pADI_GP1,0x8);   // Toggle P1.3
#endif    
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
