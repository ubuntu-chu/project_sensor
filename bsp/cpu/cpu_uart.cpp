#include    "cpu_uart.h"


struct info_com;
typedef  int8 (*pf_protocol)(struct info_com *pinfo_com);
typedef  int8 (*pf_protocol_init)(uint8 param);
struct info_com{
    uint8 			m_status;
    uint8 			m_buffer[_UART_BUFFER_SIZE];
    uint8 			m_framedone;
    uint16 			m_framelen;
    pf_protocol 	m_pfprotocol;
};
typedef struct info_com 	info_com_t;

info_com_t t_info_com[4];

struct uart3_recv_ctl{
	uint8 			m_frame_start_delimiter;
	uint8   		m_frame_len;
};
typedef struct uart3_recv_ctl  uart3_recv_ctl_t;

uart3_recv_ctl_t  t_uart3_recv_ctl;


void com_info_init(uint8 uart);
static  void com_info_status_set(uint8 uart, uint8 status);
static  uint8 com_info_status_get(uint8 uart);
static  void com_info_framelen_inc(uint8 uart);
void com_info_recv(uint8 uart, uint8 ch);


void uart_ctl_init(void)
{

}

void uart_init(uint8_t uart_id, uint32_t baud_rate)
{
	//init com info manage struct
//    com_info_init(uart_id);
//    com_protocol_init(uart_id, 1);

	switch (uart_id) {
	case UART_0:
		
		break;

	case UART_1: // 430-PC
		
		break;

	case UART_2: // 430-2530   rate:9600, ACLK为时钟源
		
		break;

	case UART_3: //430-E2- GPRS
		
		break;

	default:
		break;
	}
}

void uart_tx(uint8_t uart_id, uint8_t *pData, uint16_t len)
{
	uint16_t i;
    uint8 tmp;
    
    (void)tmp;

	switch(uart_id){
		case UART_0:
            
			break;
            
		case UART_1:
            
			break;
			
		case UART_2:
			
			break;
		case UART_3:
            
			break;
			break;
		default:
			break;
	}
	
}


/*********************************************************************************
**                            End Of File
*********************************************************************************/
