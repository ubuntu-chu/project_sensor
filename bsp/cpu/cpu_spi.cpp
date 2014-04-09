/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "cpu_spi.h"
#include "../../includes/macro.h"

/******************************************************************************
 *                       本文件所定义的静态全局变量
******************************************************************************/ 


/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
 *                                                                           
 *  入口参数 :                                                          
 *                                                                             
 *  出口参数 :                                                              
 *                                                                              
 *  编 写 者 :                                                                
 *                                                                                 
 *  日    期 :                                                                 
 *                                                                              
 *  审 核 者 :                                                                                                                               
 *                                                                             
 *  日    期 :                                                                
 *                                                                                                                                       
 *  附    注 : 系统中的中端应该由其一套中断管理体系进行管理                                                            
 *                                                                            
 *                                                                            
******************************************************************************/ 


void cpu_spi_init(uint8 num)
{
	uint16  div    = 2;               //分频值
    
    if (num == UCB0){
        P3SEL |= BIT1 + BIT2 + BIT3;     //引脚工作在USCI模式下 
        UCB0CTL1 |= UCSWRST;                      // **Put state machine in reset**
        UCB0CTL0 = UCMST  + UCMSB + UCCKPH; //UCMSB ， 3线，8位SPI主机模式,MSB 1st ,0 0 模式
        UCB0CTL1 |= UCSSEL_2;     //SMCLK
        UCB0BR0 = div & 0XFF;       //32分频
        UCB0BR1 = div >> 8;
        UCB0CTL1 &= ~UCSWRST;     //退出复位  
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
