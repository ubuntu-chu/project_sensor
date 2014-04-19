#ifndef __CPU_UART_H
#define __CPU_UART_H

#ifndef INCLUDES_LOW_H
#define INCLUDES_LOW_H
	#include    "../../includes/includes-low.h"
    
#endif
//#include    "../../api/utils.h"

enum{
	UART_0         =             0,
	UART_MAX_NUMB,
};


#define UART_BAND                   (9600u)



void uart_ctl_init(void);
void uart_init(uint8_t uart_id, uint32_t baud_rate);
void uart_enableIRQ(uint8_t uart_id);
uint16 uart_tx(uint8_t uart_id, uint8_t *pData, uint16_t len);
void uart_start_rx(uint8_t uart_id);
portBASE_TYPE uart_poll(uint8_t uart_id, int8 *pbuf, uint16 * plen, uint16 timeout);


/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif
