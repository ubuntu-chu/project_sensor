/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 

#include    "cpu_gpio.h"

portBASE_TYPE cpu_gpioinit(void)
{
	//DioOen(pADI_GP1,0x8);
    DioOen(pADI_GP1,0x80);

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
	//DioTgl(pADI_GP1,0x8);   // Toggle P1.3
    DioTgl(pADI_GP1,0x80);   // Toggle P1.7
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
