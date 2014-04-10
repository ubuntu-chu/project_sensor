#ifndef __CPU_UART_H
#define __CPU_UART_H

#ifndef INCLUDES_LOW_H
	#include    "../../includes/includes-low.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


enum{
	UART_0         =             0,
	UART_1,
	UART_2,
	UART_3,
	UART_MAX_NUMB,
};

enum{
	UART_RECV_STATT_IDLE		= 0,
	UART_RECV_STATT_DONE,
	UART_RECV_STATT_TIMEOUT,

};

#define UART_BAND                   (9600u)

#define                        _UART_BUFFER_SIZE                (255)
#define                        _UART_STATUS_RECV                (1)
#define                        _UART_STATUS_IDLE                (0)
#define                        _UART_STATUS_PARSER              (2)



void uart_ctl_init(void);
void uart_init(uint8_t uart_id, uint32_t baud_rate);
void sensor_uart_tx_char(uint8_t val);
void uart_tx(uint8_t uart_id, uint8_t *pData, uint16_t len);
void uart_a1_clear(void);


void uart3_init(void);
void debug_uart_tx_char(uint8_t val);
void debug_uart_tx(uint8_t *pData, uint16_t len);
void uart_a3_handler(uint8_t ch);

uint8 com_info_recv_done(uint8 uart, char **ppbuff, uint16 * plen);
void com_protocol_init(uint8 uart, uint8 param);

int8 uart3_recv_init(void);

void com_info_framedone_set(uint8 uart, uint8 stat);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#ifdef __cplusplus
}
#endif


#endif
