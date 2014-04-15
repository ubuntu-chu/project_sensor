/******************************************************************************
 *                              ͷ�ļ���ʿ
******************************************************************************/ 
 
#ifndef __CPU_GPIO_H
#define __CPU_GPIO_H

/******************************************************************************
 *                             ���ļ�����
******************************************************************************/

#ifndef INCLUDES_LOW_H
#define INCLUDES_LOW_H
	#include    "../../includes/includes-low.h"
#endif

/******************************************************************************
 *                           �ļ��ӿ���Ϣ�궨��
******************************************************************************/


#define         DIR_OUTPUT                                      (0)
#define         DIR_INPUT                                       (1)
#define         PULL_UP_RES                                     (0)
#define         PULL_DOWN_RES                                   (1)
#define         NONE_RES                                        (2)

// BASE FILE
typedef enum{
	PORT_1 = 0x01,
	PORT_2,
	PORT_3,
	PORT_4,
	PORT_5,
}port_enum_type_t;



#define ps_e2_on()	(P2OUT |= BIT1)
#define ps_e2_off()	(P2OUT &= ~BIT1)

#define ps_uart01_on() (P8OUT &= ~BIT4)
#define ps_uart01_off() (P8OUT |= BIT4)

#define ps_12v_on() (P11OUT &= ~BIT0)
#define ps_12v_off() (P11OUT |= BIT0)

#define ps_rf_on() (P8OUT &= ~BIT5)
#define ps_rf_off() (P8OUT |= BIT5)


#define m_led1_on() (P1OUT &= ~BIT7)
#define m_led1_off() (P1OUT |= BIT7)

#define m_led2_on() (P2OUT &= ~BIT0)
#define m_led2_off() (P2OUT |= BIT0)

#define r_dir_in() (P10OUT &= ~BIT2)
#define r_dir_out() (P10OUT |= BIT2)

#define r_oe_en() (P10OUT &= ~BIT3)
#define r_oe_dis() (P10OUT |= BIT3)

#define r_ctl1_on() (P9OUT &= ~BIT6)
#define r_ctl1_off() (P9OUT |= BIT6)
#define r_ctl2_on() (P9OUT &= ~BIT7)
#define r_ctl2_off() (P9OUT |= BIT7)
#define r_ctl3_on() (P10OUT &= ~BIT0)
#define r_ctl3_off() (P10OUT |= BIT0)
#define r_ctl4_on() (P10OUT &= ~BIT1)
#define r_ctl4_off() (P10OUT |= BIT1)

/******************************************************************************
 *                            �ļ��ӿں�������
******************************************************************************/ 

extern portBASE_TYPE cpu_gpioinit(void);

void cpu_led_toggle(void);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif
