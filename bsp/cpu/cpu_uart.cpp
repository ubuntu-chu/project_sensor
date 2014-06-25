#include    "cpu_uart.h"
#include    "../../api/log/log.h"
#include    "../../api/utils.h"
#include    "../../includes/includes.h"

#define     def_DMA_SEND     
#define     def_HALF_DUPLEX_RS485

enum TX_STAT{
	TX_STAT_IDLE 			= 0x00,
	TX_STAT_PROCESSING,
};

enum{
	DUPLEX 					= 0,
	HALF_DUPLEX				= 1,
};

enum RX_STAT{
	RX_STAT_IDLE		= 0,
	RX_STAT_RECV,
	RX_STAT_DONE,
	RX_STAT_TIMEOUT,

};

#define     def_TRANSCEIVER_RX_BUF_SIZE                     (100)
#define     def_TRANSCEIVER_TX_BUF_SIZE                     (300)

class transceiver:noncopyable {
public:
	transceiver(uint8 duplex):m_duplex(duplex),
                            m_handle_rx((timer_handle_type)-1)
    {
        buf_queue_init(&m_buf_queue);
    };
	virtual ~transceiver(){};

	virtual uint16 transmit(int8 *pdata, uint16 len){return 0;};
	virtual portBASE_TYPE push(int8 c);
	portBASE_TYPE fetch(int8 *pbuf, uint16 *plen);
	portSSIZE_TYPE poll(int8 *pbuf, uint16 * plen, uint16 timeout);
	virtual bool  is_done(void){return false;}
	portBASE_TYPE 	force_done(void);
	void rx_status_set(enum RX_STAT new_status){ m_rx_status = new_status;}
    void tx_status_set(enum TX_STAT new_status){ m_tx_status = new_status;}
    enum TX_STAT tx_status_get(void){return m_tx_status;}
	void recv_init(void);
    static void timer_timeout(void *pvoid);

protected:
	enum TX_STAT 			m_tx_status;
	uint8					m_duplex;
#ifdef def_DMA_SEND
	uint8 					m_tx_buf[def_TRANSCEIVER_TX_BUF_SIZE];
#endif

private:
	uint16					m_rx_index;
	uint16					m_tx_index;
	uint16					m_tx_len;
	enum RX_STAT 			m_rx_status;
	int8 					m_rx_buf[def_TRANSCEIVER_RX_BUF_SIZE];
    timer_handle_type       m_handle_rx;
    struct buf_queue        m_buf_queue;    
};

void transceiver::recv_init(void)
{
    m_rx_index = 0; 
    m_rx_status = RX_STAT_RECV;
}

portBASE_TYPE transceiver::force_done(void)
{
    if (static_cast<int8>(-1) == buf_queue_put(&m_buf_queue, 
                                                m_rx_buf, 
                                                m_rx_index)){
        buf_queue_pop(&m_buf_queue);
        buf_queue_put(&m_buf_queue, m_rx_buf, m_rx_index);
    }
    if (m_duplex == HALF_DUPLEX){
        m_rx_index = 0;
        rx_status_set(RX_STAT_DONE); 
    }else {
        recv_init();
    }
    //initiative call device handle
    cpu_pendsv_trig();
    return 0;
}

portBASE_TYPE transceiver::push(int8 c)
{
	if ((m_duplex == HALF_DUPLEX) && (TX_STAT_PROCESSING == m_tx_status)){
		return -1;
	}
	if (RX_STAT_RECV != m_rx_status){
		return -1;
	}
    if (m_rx_index >= sizeof(m_rx_buf)){
		force_done();
        //return -1;
	}
	m_rx_buf[m_rx_index++] 		= c;
	if (is_done()){
		force_done();
	}
	return 0;
}

//return value: 0:  no buf valid    1: at least one buf valid
portBASE_TYPE transceiver::fetch(int8 *pbuf, uint16 *plen)
{
	return (0 == buf_queue_get(&m_buf_queue, pbuf, plen))?(1):(0);
}

//timeout: unit:ms
portSSIZE_TYPE transceiver::poll(int8 *pbuf, uint16 *plen, uint16 timeout)
{
	portSSIZE_TYPE		rt 				= 0;

    if (timeout){
        if (m_handle_rx == (timer_handle_type)-1){
            m_handle_rx = t_timer_manage.hard_timer_register(timeout, 
                                                SV_TIMER_FLAG_ONE_SHOT, 
                                                NULL, 
                                                NULL,
                                                "poll timeout");
            ASSERT(m_handle_rx != (timer_handle_type)-1);
        }
        t_timer_manage.timer_start(m_handle_rx, timeout);
    }
	while (1){
        rt 	= fetch(pbuf, plen);
        if (rt == 1){
            break;
        }
        if (0 == timeout){
            break;
        }
        if (t_timer_manage.timer_expired(m_handle_rx)){
            rt                          = -1;
            break;
        }
    }
    t_timer_manage.timer_stop(m_handle_rx);

	return rt;
}

class transceiver_uart0:public transceiver {
public:
	transceiver_uart0(uint8 duplex):transceiver(duplex){m_handle_uart = (timer_handle_type)-1;};
	virtual ~transceiver_uart0(){};

    static void clock_init(void)
    {
        if (0 == m_clock_init){
            m_clock_init            = 1;
            pADI_CLKCTL->CLKCON1 = ((pADI_CLKCTL->CLKCON1)&(~(BIT(9)|BIT(10)|BIT(11))))|(((uint32)CLK_CD0)<<9);
        }
    }
        
	void init(uint32_t baud_rate)
	{
        if (m_handle_uart == (timer_handle_type)-1){
			//300ms
			m_handle_uart = t_timer_manage.hard_timer_register(300,
									SV_TIMER_FLAG_ONE_SHOT,
									transceiver_uart0::byte_timer_timeout,
									this,
									"byte timer");
			ASSERT(m_handle_uart != (timer_handle_type)-1);
        }
        
		//Select IO pins for UART.
		
    #ifdef def_HALF_DUPLEX_RS485
        DioCfgPin(pADI_GP0,PIN4,0); //Set P0.4 as UART RTS 
        //DioPulPin(pADI_GP0,PIN4,0);
        DioOcePin(pADI_GP0,PIN4,1);
        DioOenPin(pADI_GP0,PIN4,1);
    #endif
    #ifdef def_RUN_IN_EVB    
        // Configure P0.1/P0.2 for UART
		pADI_GP0->GPCON = ((pADI_GP0->GPCON)&(~(BIT2|BIT3|BIT4|BIT5)))|0x3C;
    #else
        DioCfgPin(pADI_GP0,PIN6,1); //Set P0.6 as UART RXD
        DioCfgPin(pADI_GP0,PIN7,2); //Set P0.7 as UART TXD
    #endif
        
        UrtCfg(pADI_UART, baud_rate, COMLCR_WLS_8BITS, 0); // setup baud rate for 9600, 8-bits
		UrtMod(pADI_UART, COMMCR_DTR, 0);              // Setup modem bits
		// Setup UART IRQ sources
        UrtIntCfg(pADI_UART,
				COMIEN_ERBFI | COMIEN_ELSI | COMIEN_EDSSI
    #ifdef def_DMA_SEND	
                |COMIEN_EDMAT); 
    #else
                );
    #endif
    #ifdef def_DMA_SEND	
        UrtDma(pADI_UART,COMIEN_EDMAT);        // Enable UART DMA interrupts    
        DmaPeripheralStructSetup(UARTTX_C,DMA_DSTINC_NO|    // Enable DMA write channel
            DMA_SRCINC_BYTE|DMA_SIZE_BYTE);  
        NVIC_SetPriority(DMA_UART_TX_IRQn, DMA_UART_TX_IRQ_PRIO);
        NVIC_EnableIRQ(DMA_UART_TX_IRQn);                  // Enable UART Tx DMA interrupts
    #endif
        recv_init();
    #ifdef def_HALF_DUPLEX_RS485
        rs485_recv();
    #endif
	}

	void ios(uint32_t baud_rate)
	{
		init(baud_rate);
	}
    
#ifdef def_HALF_DUPLEX_RS485    
    void rs485_recv(void){DioSet(pADI_GP0,GP0OUT_OUT4);}
    void rs485_send(void){DioClr(pADI_GP0,GP0OUT_OUT4);}
#endif

	virtual uint16 transmit(int8 *pdata, uint16 len)
	{
		volatile unsigned char ucCOMSTA0;
		uint16_t i;
        (void)i;

        //prevent to send fast 主要是指使用dma发送方式时
        while (TX_STAT_PROCESSING == tx_status_get());
        tx_status_set(TX_STAT_PROCESSING);
	#ifndef def_DMA_SEND		
        
        for (i = 0; i < len; i++){
			UrtTx(pADI_UART, pdata[i]);
			do {
				ucCOMSTA0 = UrtLinSta(pADI_UART);         // Read Line Status register
			}while (!(ucCOMSTA0 & COMLSR_THRE));
		}
        do {
			ucCOMSTA0 = UrtLinSta(pADI_UART);             //兼容232与485
		}while (!(ucCOMSTA0 & COMLSR_TEMT));
        
        //recv_init();
		tx_status_set(TX_STAT_IDLE);
    #else 
        if (len > sizeof(m_tx_buf)){
            return 0;
        }
        memcpy(m_tx_buf, pdata, len);
        DmaStructPtrOutSetup(UARTTX_C, len , reinterpret_cast<uint8 *>(m_tx_buf));  // Setup UARTTX source/destination pointers and number of bytes to send
        DmaCycleCntCtrl(UARTTX_C, len, DMA_DSTINC_NO| // Setup CHNL_CFG settings
                        DMA_SRCINC_BYTE|DMA_SIZE_BYTE|DMA_BASIC);
        DmaClr(DMARMSKCLR_UARTTX,0,0,0);           // Disable masking of UARTTX DMA channel
        DmaSet(0,DMAENSET_UARTTX,0,                // Enable UART TX DMA channel
                        DMAPRISET_UARTTX);                      // Enable UART DMA primary structure
        
    #ifdef def_HALF_DUPLEX_RS485
        rs485_send();
    #endif
        UrtTx(pADI_UART,0);                        // Send byte 0 to the UART to start transfer
        
    #endif
		
		return len;
	};

	virtual bool  is_done(void)
	{
		//byte_timer_stop();
        return false;
	}
    
    void byte_timer_start(void)
    {
        t_timer_manage.timer_start(m_handle_uart);
    }
    
    void byte_timer_stop(void)
    {
        t_timer_manage.timer_stop(m_handle_uart);
    }
    
    portBASE_TYPE byte_timer_started(void)
    {
        return t_timer_manage.timer_started(m_handle_uart);
    }
    
    void byte_timer_restart(void)
    {
        t_timer_manage.timer_restart(m_handle_uart);
    }
    
    static void byte_timer_timeout(void *pvoid)
    {
        class transceiver_uart0 *ptransceiver_uart0 = 
                static_cast<class transceiver_uart0 *>(pvoid);
        
        ptransceiver_uart0->force_done();
    }
    
    virtual portBASE_TYPE push(int8 c)
    {
        portBASE_TYPE   rt = transceiver::push(c);
        
        if (0 == rt){
            if (0 == byte_timer_started()){
                byte_timer_start();
            }else{
                byte_timer_restart();
            }
        }
        return rt;
    }

private:
    static uint8        m_clock_init;
    timer_handle_type m_handle_uart;
};

uint8 transceiver_uart0::m_clock_init = 0;

class transceiver_uart0 	t_transceiver_uart0(HALF_DUPLEX);

//-----------------------------------------------------------------------------------------------------

void uart_ctl_init(void) 
{
    transceiver_uart0::clock_init();
}

void uart_init(uint8_t uart_id, uint32_t baud_rate)
{
	switch (uart_id) {
	case UART_0:
		t_transceiver_uart0.init(baud_rate);
		break;

	default:
		break;
	}
}


void uart_ios(uint8_t uart_id, uint32_t baud_rate)
{
	switch (uart_id) {
	case UART_0:
		NVIC_DisableIRQ(UART_IRQn);
        t_transceiver_uart0.ios(baud_rate);
        NVIC_EnableIRQ(UART_IRQn);
		break;

	default:
		break;
	}
}

void uart_enableIRQ(uint8_t uart_id)
{
    switch (uart_id) {
	case UART_0:
		NVIC_SetPriority(UART_IRQn, UART_IRQ_PRIO);
        NVIC_EnableIRQ (UART_IRQn);
		break;

	default:
		break;
	}
}

uint16 uart_tx(uint8_t uart_id, uint8_t *pData, uint16_t len)
{
    uint16  rt;
    
	switch (uart_id) {
	case UART_0:
		rt  = t_transceiver_uart0.transmit(reinterpret_cast<int8 *>(pData), len);
		break;
    
	default:
		break;
	}
    
    return rt;
}

void uart_start_rx(uint8_t uart_id)
{
    switch (uart_id) {
	case UART_0:
		t_transceiver_uart0.recv_init();
		break;

	default:
		break;
	}
}

portSSIZE_TYPE uart_poll(uint8_t uart_id, int8 *pbuf, uint16 * plen, uint16 timeout)
{
    portSSIZE_TYPE rt;
    
    switch (uart_id) {
	case UART_0:
        rt  = t_transceiver_uart0.poll(pbuf, plen, timeout);
        break;
    default:
		break;
	}
    return rt;
}

extern "C" void UART_Int_Handler ()
{
    volatile unsigned char ucCOMSTA0;
    volatile unsigned char ucCOMIID0;
    volatile unsigned char ucComRx;
   
    //portCPSR_TYPE	level = cpu_interruptDisable();
    
    ucCOMSTA0 = UrtLinSta(pADI_UART);         // Read Line Status register
    ucCOMIID0 = UrtIntSta(pADI_UART);         // Read UART Interrupt ID register         
    
    //check if a interrupt occured
    if ((ucCOMIID0 & BIT0) == BIT0){
        goto quit;
    //receive buffer full interrupt
    }else if ((ucCOMIID0 & BIT2) == BIT2){             // Receive byte
		if (static_cast<portBASE_TYPE>(-1) == t_transceiver_uart0.push(UrtRx(pADI_UART))){
			//unexpected data comming

		}

    //receive line status interrupt
    }else if ((ucCOMIID0 & (BIT1|BIT2)) == (BIT1|BIT2)){
        //line status error    drop ucComRx
        if (ucCOMSTA0 & (BIT1|BIT2|BIT3|BIT4)){
            //drop ucComRx
            ucComRx   = UrtRx(pADI_UART);
        }
        goto quit;
    //modem status interrupt  do not care
    }else if ((ucCOMIID0 & (BIT1|BIT2)) == 0){
        goto quit;
    }
quit:
    //cpu_interruptEnable(level);
} 

extern "C" void DMA_UART_TX_Int_Handler()
{
#ifdef def_DMA_SEND	
    volatile unsigned char ucCOMSTA0;
    volatile unsigned char ucCOMIID0 = 0;
    
    ucCOMIID0 = UrtIntSta(pADI_UART);                // Read UART Interrupt ID register 
    DmaSet(DMARMSKSET_UARTTX,DMAENSET_UARTTX,0,0);   // MASK UART DMA channel to prevent further interrupts triggering - unmask when ready to re-transmit
    do {
		ucCOMSTA0 = UrtLinSta(pADI_UART);         // Read Line Status register
	}while (!(ucCOMSTA0 & COMLSR_TEMT));
#ifdef def_HALF_DUPLEX_RS485
    t_transceiver_uart0.rs485_recv();
#endif
    t_transceiver_uart0.tx_status_set(TX_STAT_IDLE);
#endif
}

/*********************************************************************************
 **                            End Of File
 *********************************************************************************/
