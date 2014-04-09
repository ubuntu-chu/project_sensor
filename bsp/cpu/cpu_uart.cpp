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
static inline void com_info_status_set(uint8 uart, uint8 status);
static inline uint8 com_info_status_get(uint8 uart);
static inline void com_info_framelen_inc(uint8 uart);
void com_info_recv(uint8 uart, uint8 ch);
static int8 uart0_protocol(struct info_com *pinfo_com);
static int8 uart1_protocol(struct info_com *pinfo_com);
static int8 uart2_protocol(struct info_com *pinfo_com);
static int8 uart3_protocol(struct info_com *pinfo_com);
static int8 uart0_protocol_init(uint8 param);
static int8 uart1_protocol_init(uint8 param);
static int8 uart2_protocol_init(uint8 param);
static int8 uart3_protocol_init(uint8 param);

const pf_protocol uart_protocol_array[4] = {

		uart0_protocol, uart1_protocol,
		uart2_protocol, uart3_protocol,
};

const pf_protocol_init uart_protocol_init_array[4] = {

		uart0_protocol_init, uart1_protocol_init,
		uart2_protocol_init, uart3_protocol_init,
};

void uart_ctl_init(void)
{
	P4DIR |= (BIT0 + BIT1 + BIT2);
	P4OUT &= ~BIT0;
	P4OUT |= BIT1 + BIT2;

}

void uart_init(uint8_t uart_id, uint32_t baud_rate)
{
	//init com info manage struct
    com_info_init(uart_id);
    com_protocol_init(uart_id, 1);

	switch (uart_id) {
	case UART_0:
		P3SEL |= BIT5 + BIT4; // P3.5 = USCI_A0 RXD, P3.4 = USCI_A0 TXD
		P3DIR |= BIT4;
		P3DIR &= ~BIT5;

	  UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
	  UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
	  UCA0BR0 = 0x03;                           // 32k/9600 - 3.41
	  UCA0BR1 = 0x00;                           //
	  UCA0MCTL = 0x06;                          // Modulation
#if 0
		UCA0CTL1 |= UCSWRST;
		UCA0CTL0 = UCMODE_0;
		UCA0CTL1 |= UCSSEL__SMCLK;
#if 1
		UCA0BR0 = 0x82;
		UCA0BR1 = 0x6;
		UCA0MCTL |= UCBRS_6 + UCBRF_0; // Modulation UCBRSx=3, UCBRFx=0
#endif
#endif
#if 0
        UCA0BR0 = 0x15;
		UCA0BR1 = 0x1;
		UCA0MCTL |= UCBRS_7 + UCBRF_0; // Modulation UCBRSx=3, UCBRFx=0
#endif
		UCA0CTL1 &= ~UCSWRST;
		break;

	case UART_1: // 430-PC
		P5SEL |= BIT6 + BIT7; // P5.7 = USCI_A1 RXD, P5.6 = USCI_A1 TXD
		P5DIR |= BIT6;
		P5DIR &= ~BIT7;

		  UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
		  UCA1CTL1 |= UCSSEL_1;                     // CLK = ACLK
		  UCA1BR0 = 0x03;                           // 32k/9600 - 3.41
		  UCA1BR1 = 0x00;                           //
		  UCA1MCTL = 0x06;                          // Modulation
#if 0
		UCA1CTL1 = UCSWRST;
		UCA1CTL0 = UCMODE_0;
		UCA1CTL1 |= UCSSEL__SMCLK;
#if 1
		//9600 bps
        UCA1BR0 = 0x82;
		UCA1BR1 = 0x6;
		UCA1MCTL |= UCBRS_6 + UCBRF_0; // Modulation UCBRSx=3, UCBRFx=0
#endif
#endif
#if 0
		//115200 bps
		UCA1BR0 = 0x8a;
		UCA1BR1 = 0x0;
		UCA1MCTL |= UCBRS_7 + UCBRF_0;
#endif
#if 0
		//57600 bps
		UCA1BR0 = 0x15;
		UCA1BR1 = 0x1;
		UCA1MCTL |= UCBRS_7 + UCBRF_0;
#endif
		UCA1CTL1 &= ~UCSWRST;
		UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
		break;

	case UART_2: // 430-2530   rate:9600, ACLK为时钟源
		P9SEL |= BIT5 + BIT4; // P10.5 = USCI_A3 RXD, P10.4 = USCI_A3 TXD
		P9DIR |= BIT4;
		P9DIR &= ~BIT5;

		UCA2CTL1 |= UCSWRST;
		UCA2CTL0 = UCMODE_0;
		UCA2CTL1 |= UCSSEL__SMCLK;
#if 0
		//115200 bps
		UCA2BR0 = 0x8a;
		UCA2BR1 = 0x0;
		UCA2MCTL |= UCBRS_7 + UCBRF_0;
#endif
		UCA2IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
		UCA2CTL1 &= ~UCSWRST;
		break;

	case UART_3: //430-E2- GPRS
		P10SEL |= BIT5 + BIT4;
		P10DIR |= BIT4;
		P10DIR &= ~BIT5;

		UCA3CTL1 |= UCSWRST;
		UCA3CTL0 = UCMODE_0;
		UCA3CTL1 |= UCSSEL__SMCLK;
#if 1
		UCA3BR0 = 0x82;
		UCA3BR1 = 0x6;
		UCA3MCTL |= UCBRS_6 + UCBRF_0; // Modulation UCBRSx=3, UCBRFx=0
#endif
		UCA3CTL1 &= ~UCSWRST;	      // Initialize USCI state machine

		UCA3IE |= UCRXIE;                         // Enable USCI_A3 RX interrupt
		//trig recv
		com_info_status_set(uart_id, _UART_STATUS_RECV);
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
            while( !(UCA0IFG & UCTXIFG) );  // Tx buffer empty
			for( i=0; i < len; i++ )
			{
				UCA0TXBUF = pData[i];
				while( !(UCA0IFG & UCTXIFG) );	// Tx complete
			}
			//init com info manage struct
			com_info_init(uart_id);
            //clear rx buffer
			tmp             = UCA0RXBUF;
            //trig recv
			com_info_status_set(uart_id, _UART_STATUS_RECV);
			UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
			break;
            
		case UART_1:
            while( !(UCA1IFG & UCTXIFG) );  // Tx buffer empty
			for( i=0; i < len; i++ )
			{
				UCA1TXBUF = pData[i];
				while( !(UCA1IFG & UCTXIFG) );	// Tx complete
			}
            //init com info manage struct
			com_info_init(uart_id);
            //clear rx buffer
			tmp             = UCA1RXBUF;
			//trig recv
			com_info_status_set(uart_id, _UART_STATUS_RECV);
			UCA1IE |= UCRXIE;
			break;
			
		case UART_2:
			while( !(UCA2IFG & UCTXIFG) );  // Tx buffer empty
			for( i=0; i < len; i++ )
			{
				UCA2TXBUF = pData[i];
				while( !(UCA2IFG & UCTXIFG) );	// Tx complete
			}
			break;
		case UART_3:
            while( !(UCA3IFG & UCTXIFG) );  // Tx buffer empty
			for( i=0; i < len; i++ )
			{
				UCA3TXBUF = pData[i];
				while( !(UCA3IFG & UCTXIFG) );	// Tx complete
			}
			break;
			break;
		default:
			break;
	}
	
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void) {
	switch (__even_in_range(UCA0IV, 4)) {
	case 0:
		break;                             // Vector 0 - no interrupt
	case 2:                                   // Vector 2 - RXIFG
        com_info_recv(UART_0, UCA0RXBUF);
	    break;
	case 4:
		break;                             // Vector 4 - TXIFG
	default:
		break;
	}
}

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
	switch (__even_in_range(UCA1IV, 4)) {
	case 0:
		break;                             // Vector 0 - no interrupt
	case 2:                                   // Vector 2 - RXIFG
		com_info_recv(UART_1, UCA1RXBUF);
		break;
	case 4:
		break;                             // Vector 4 - TXIFG
	default:
		break;
	}
}

#pragma vector=USCI_A2_VECTOR
__interrupt void USCI_A2_ISR(void) {
	switch (__even_in_range(UCA2IV, 4)) {
	case 0:
		break;                             // Vector 0 - no interrupt
	case 2:                                   // Vector 2 - RXIFG
		com_info_recv(UART_2, UCA2RXBUF);
		break;
	case 4:
		break;                             // Vector 4 - TXIFG
	default:
		break;
	}
}

#pragma vector=USCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void) {
	switch (__even_in_range(UCA3IV, 4)) {
	case 0:
		break;                             // Vector 0 - no interrupt
	case 2:                                   // Vector 2 - RXIFG
		com_info_recv(UART_3, UCA3RXBUF);
		break;
	case 4:
		break;                             // Vector 4 - TXIFG
	default:
		break;
	}
}

static inline void com_info_status_set(uint8 uart, uint8 status)
{
	t_info_com[uart].m_status			= status;
}

static inline uint8 com_info_status_get(uint8 uart)
{
	return t_info_com[uart].m_status;
}

void com_info_init(uint8 uart)
{
	t_info_com[uart].m_framelen			= 0;
	com_info_status_set(uart, _UART_STATUS_IDLE);
    com_info_framedone_set(uart, UART_RECV_STATT_IDLE);
	t_info_com[uart].m_pfprotocol		= uart_protocol_array[uart];
    memset(t_info_com[uart].m_buffer, 0, sizeof(t_info_com[uart].m_buffer));
    //memset(t_info_com[uart].m_buffer, 0, t_info_com[uart].m_framelen);
}

void com_protocol_init(uint8 uart, uint8 param)
{
	if (uart_protocol_init_array[uart] != NULL){
		uart_protocol_init_array[uart](param);
	}
}

void com_info_framedone_set(uint8 uart, uint8 stat)
{
	t_info_com[uart].m_framedone 		= stat;
}

uint8 com_info_recv_done(uint8 uart, char **ppbuff, uint16 * plen)
{
	uint8 	tmp 						= t_info_com[uart].m_framedone;

	if (UART_RECV_STATT_DONE == tmp){

		t_info_com[uart].m_framedone	= UART_RECV_STATT_IDLE;
		if (NULL != ppbuff) {
			*ppbuff = (char *) t_info_com[uart].m_buffer;
		}
		if (NULL != plen) {
			*plen = t_info_com[uart].m_framelen;
		}
	}

	return tmp;
}

void com_info_recv(uint8 uart, uint8 ch)
{
	if (_UART_STATUS_RECV != com_info_status_get(uart)){
		uint8 buf[] = "1*\n";

		buf[0]			= uart + 0x30;
		SYS_LOG_LEV_TINY(SYS_LOG_LEV_WARNING, buf);
		return;
	}
	t_info_com[uart].m_buffer[t_info_com[uart].m_framelen++] 		= ch;
	if (!t_info_com[uart].m_pfprotocol(&t_info_com[uart])){
		com_info_framedone_set(uart, UART_RECV_STATT_DONE);
	}

}

static int8 uart0_protocol_init(uint8 param)
{

	return 0;
}
static int8 uart0_protocol(struct info_com *pinfo_com)
{
	if (pinfo_com->m_buffer[0] == (pinfo_com->m_framelen -1)){
		//stop comm monitor
		uart_comm_monitor_clr();
        //UCA0IE &= ~UCRXIE;                         // disable USCI_A0 RX interrupt
        pinfo_com->m_status     = _UART_STATUS_PARSER;
		return 0;
	}else {
		//start comm monitor
	    uart_comm_monitor_set();
        return 1;
	}
}

static int8 uart1_protocol_init(uint8 param)
{

	return 0;
}
static int8 uart1_protocol(struct info_com *pinfo_com)
{

	if (pinfo_com->m_buffer[0] == (pinfo_com->m_framelen -1)){
		UCA1IE &= ~UCRXIE;                         // disable USCI_A0 RX interrupt
        pinfo_com->m_status     = _UART_STATUS_PARSER;
		return 0;
	}else {
		return 1;
	}
}

static int8 uart2_protocol_init(uint8 param)
{

	return 0;
}
static int8 uart2_protocol(struct info_com *pinfo_com)
{

	return 0;
}


static int8 uart3_protocol_init(uint8 param)
{
	memset(&t_uart3_recv_ctl, 0, sizeof(uart3_recv_ctl_t));
	//first init?
	if (param == 1){
	}

	return 0;
}

int8 uart3_recv_init(void)
{
    com_info_init(UART_3);
	com_info_status_set(UART_3, _UART_STATUS_RECV);
    //protocol reinit
    uart3_protocol_init(0);
    
    return 0;
}    

static int8 uart3_protocol(struct info_com *pinfo_com)
{
	uint16 	len			= pinfo_com->m_framelen;

	if (len < 2){
		return 1;
	}
	if (t_uart3_recv_ctl.m_frame_start_delimiter == 0){

		if ((pinfo_com->m_buffer[len - 2] == (def_FRAME_delimiter & 0x00ff))
			&& (pinfo_com->m_buffer[len - 1] == (def_FRAME_delimiter >> 8))){
			t_uart3_recv_ctl.m_frame_start_delimiter 			= 1;
		}else {
			pinfo_com->m_framelen								= 0;
		}
		return 1;
	}else {
		if (t_uart3_recv_ctl.m_frame_len == 0){
			if (len == 4){
                t_uart3_recv_ctl.m_frame_len 	= pinfo_com->m_buffer[len-2];
                t_uart3_recv_ctl.m_frame_len 	+= (uint16)(pinfo_com->m_buffer[len-1])<<8;
			    t_uart3_recv_ctl.m_frame_len    -= 2;
            }
			return 1;
		}else {
			if (t_uart3_recv_ctl.m_frame_len != (len - 6)){
				return 1;
			}
		}
	}
	//push skbuf
	net_queue_put(pinfo_com->m_buffer, len);
	uart3_recv_init();

	return 0;


}


/*********************************************************************************
**                            End Of File
*********************************************************************************/
