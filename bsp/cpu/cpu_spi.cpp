/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 

#include    "cpu_spi.h"
#include "../../includes/macro.h"

/******************************************************************************
 *                       ���ļ�������ľ�̬ȫ�ֱ���
******************************************************************************/ 


/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                          
 *                                                                             
 *  ���ڲ��� :                                                              
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע : ϵͳ�е��ж�Ӧ������һ���жϹ�����ϵ���й���                                                            
 *                                                                            
 *                                                                            
******************************************************************************/ 


void cpu_spi_init(uint8 num)
{
	uint16  div    = 2;               //��Ƶֵ
    
    if (num == UCB0){
        P3SEL |= BIT1 + BIT2 + BIT3;     //���Ź�����USCIģʽ�� 
        UCB0CTL1 |= UCSWRST;                      // **Put state machine in reset**
        UCB0CTL0 = UCMST  + UCMSB + UCCKPH; //UCMSB �� 3�ߣ�8λSPI����ģʽ,MSB 1st ,0 0 ģʽ
        UCB0CTL1 |= UCSSEL_2;     //SMCLK
        UCB0BR0 = div & 0XFF;       //32��Ƶ
        UCB0BR1 = div >> 8;
        UCB0CTL1 &= ~UCSWRST;     //�˳���λ  
    }
}

int8 cpu_spi_w8r8(uint8 num, uint8 w8, uint8 *r8)
{
    int8 rt    = 0;
    uint8 tmp;
    
    if (num == UCB0){
        while(!(UCB0IFG & UCTXIFG));			// Wait for USCI_B2 TX buffer ready
	    UCB0TXBUF = w8;						// Send data
	    while(!(UCB0IFG & UCTXIFG));			// Wait for USCI_B2 TX complete
	    while(!(UCB0IFG & UCRXIFG));			// Wait for USCI_B2 RX buffer ready
        UCB0IFG &= ~UCRXIFG;
        tmp      = UCB0RXBUF;
        if (r8 != NULL){
            *r8         = tmp;
        }
    }
    
    return rt;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
