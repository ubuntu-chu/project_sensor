/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "cpu_iic.h"
#include "../../includes/macro.h"
#include "../../api/log/log.h"
#include    "../../api/utils.h"
#include    "../../includes/service.h"


iic_transfer_t t_iic_transfer;

/******************************************************************************
 *                       本文件所定义的静态全局变量
******************************************************************************/ 

/**
    On the master side, the master generates a repeated start if the I2CADR0 register is 
    written while the master is still busy with a transaction. 
    After the state machine has started to transmit the device address, 
    it is safe to write to the I2CADR0 register.
    For example, if a transaction involving a write, repeated start, 
    and then read/write is required, write to the I2CADR0 register either after 
    the state machine starts to transmit the device address or 
    after the first TXREQ interrupt is received. When the transmit FIFO empties, a 
    repeated start is generated.
    
     If the Tx FIFO is not full during an active transfer sequence, the transmit request bit 
(TXREQ) inI2CMSTA or I2CSSTA asserts.

    The master generates a stop condition if there is no data in the transmit FIFO and the master is writing data.
    
     The receive request interrupt 
bit (RXREQ) in I2CMSTA or I2CSSTA indicates whether there is valid data in the Rx FIFO. Data is loaded into the Rx FIFO after each 
byte is received. If valid data in the Rx FIFO is overwritten by the Rx shifter, the receive overflow status bit (RXOF) is asserted (I2CMSTA[9] 
or I2CSSTA[4]).

    When receiving data, the master responds with a NACK if its FIFO is full and an attempt is made to write another byte to the FIFO. This 
last byte received is not written to the FIFO and is lost.

    
*/

static portBASE_TYPE cpu_iic_interrupt_enable(enum iic_numb numb);

#define     def_I2C_INT_SOURCE              (I2CMCON_IENCMP | I2CMCON_IENRX | I2CMCON_IENTX| I2CMCON_IENNACK)

portBASE_TYPE cpu_iic_init(enum iic_numb numb)
{
    portBASE_TYPE  rt   = 0;
    
    if (numb > enum_NUMB_IIC_MAX){
        return -1;
    }
    if (numb == enum_NUMB_IIC0){
        // Configure P2.0/P2.1 as I2C pins   p2.0-SCL   p2.1-SDA
		DioCfgPin(pADI_GP2, PIN0, 1);
        DioCfgPin(pADI_GP2, PIN1, 1);
        DioPulPin(pADI_GP2, PIN0, 0);
        DioPulPin(pADI_GP2, PIN1, 0);
        //pADI_GP2->GPCON = ((pADI_GP2->GPCON)&(~(BIT0|BIT1|BIT2|BIT3)))|0x5;
        //pADI_GP2->GPPUL = ((pADI_GP2->GPPUL)&(~(BIT0|BIT1))); //disable external pull ups
    }
	// Enable I2C Master mode, baud rate and interrupt sources
    //IENCMP:Enable a transaction completed interrupt. If this bit is asserted, an interrupt is generated when a 
    //stop is detected
	I2cMCfg(I2CMCON_TXDMA_DIS | I2CMCON_RXDMA_DIS,
			def_I2C_INT_SOURCE, I2CMCON_MAS_EN);
    //Fi2cclk = 16Mhz
	I2cBaud(0x4E, 0x4F); // 100kHz clock high time is 5000 ns and low time is 5000ns
	cpu_iic_interrupt_enable(enum_NUMB_IIC0);
    
    return rt;
}

static portBASE_TYPE cpu_iic_interrupt_enable(enum iic_numb numb)
{
    portBASE_TYPE  rt   = 0;

    if (numb > enum_NUMB_IIC_MAX){
        return -1;
    }
    if (numb == enum_NUMB_IIC0){
		NVIC_SetPriority(I2CM_IRQn, I2CM_IRQ_PRIO);
        NVIC_EnableIRQ(I2CM_IRQn);
    }

    return rt;
}

portSSIZE_TYPE cpu_iic_transfer(struct iic_transfer *ptransfer)
{
	portSSIZE_TYPE 	rt ;
    timer_handle_type     handle_iic;

	t_iic_transfer.m_index  = 0;
	t_iic_transfer.m_lock   = 0;
    rt                      = t_iic_transfer.m_accessNumbBytes;
    
    if (t_iic_transfer.m_accessNumbBytes > 256){
        
        return -2;              
    }
    //unit: ms
    handle_iic = t_timer_manage.hard_timer_register(t_iic_transfer.m_timeout,
                                                SV_TIMER_FLAG_ONE_SHOT, 
                                                NULL, 
                                                NULL,
                                                "iic timeout");
    ASSERT(handle_iic != (timer_handle_type)-1);
    t_timer_manage.timer_start(handle_iic);

	I2cMCfg(I2CMCON_TXDMA_DIS | I2CMCON_RXDMA_DIS,
			def_I2C_INT_SOURCE, I2CMCON_MAS_EN);
    
    if (t_iic_transfer.m_subAddrCnt > 0){
		if (t_iic_transfer.m_subAddrCnt == 2)
		{
			I2cTx(MASTER, (t_iic_transfer.m_subAddr >> 8) & 0xff);
			t_iic_transfer.m_subAddrCnt--;
		}
		else if (t_iic_transfer.m_subAddrCnt == 1)
		{
			I2cTx(MASTER, t_iic_transfer.m_subAddr & 0xff);
			t_iic_transfer.m_subAddrCnt--;
            if(t_iic_transfer.m_accessCtrl == I2C_WRITE){
				t_iic_transfer.m_accessCtrl  = I2C_NONE;
			}
		}
	}
    //If the master enable bit (I2CMON[0], MASEN) is set,
    //a master transfer sequence is initiated by writing a value to the I2CADRx register.
    //If there is valid data in the I2CMTX register, it is the first byte
    //transferred in the sequence after the address byte during a write sequence.
	I2cMWrCfg(t_iic_transfer.m_slaAddr);

	//等待I2C操作完成
	do {
        if (t_timer_manage.timer_expired(handle_iic)){
            rt                          = -1;
            t_iic_transfer.m_accessFinished = I2C_TIMEOUT;
            I2cMCfg(I2CMCON_TXDMA_DIS|I2CMCON_RXDMA_DIS, 0, I2CMCON_MAS_DIS);
            break;
        }
	}while (t_iic_transfer.m_accessFinished == FALSE);
    //存取异常
    if (I2C_ABNORMAL == t_iic_transfer.m_accessFinished){
        rt                              = -1;
    }
    t_timer_manage.timer_stop(handle_iic);
    t_timer_manage.timer_unregister(handle_iic);

	return rt;
}

// I2C0 master handler
extern "C" void I2C0_Master_Int_Handler(void)
{
	unsigned int uiStatus;

	uiStatus = I2cSta(MASTER);
	
    if (((uiStatus & I2CMSTA_RXOF) == I2CMSTA_RXOF)
        || ((uiStatus & I2CMSTA_NACKDATA) == I2CMSTA_NACKDATA)
        || ((uiStatus & I2CMSTA_ALOST) == I2CMSTA_NACKDATA)
        || ((uiStatus & I2CMSTA_NACKADDR) == I2CMSTA_NACKADDR)){ 
        t_iic_transfer.m_accessFinished 	= I2C_ABNORMAL;
        return;
    }
    
    if ((uiStatus & I2CMSTA_RXREQ) == I2CMSTA_RXREQ){       // Master Recieve IRQ
		t_iic_transfer.m_paccessAddr[t_iic_transfer.m_index++] = I2cRx(MASTER);
		if (t_iic_transfer.m_index >= t_iic_transfer.m_accessNumbBytes) // Resetting value of i if it has been incremented by RX
			t_iic_transfer.m_index = 0;
	}

	if ((uiStatus & I2CMSTA_TXREQ) == I2CMSTA_TXREQ){ // Master Transmit IRQ
		if ((t_iic_transfer.m_lock == 0) && (t_iic_transfer.m_accessCtrl == I2C_READ)){
            I2cMRdCfg(t_iic_transfer.m_slaAddr, t_iic_transfer.m_accessNumbBytes, DISABLE);
			t_iic_transfer.m_lock 		= 1;
		}
		if (t_iic_transfer.m_accessCtrl == I2C_NONE){                     //已经发送过读写地址之后 进入到数据传输阶段
			if (t_iic_transfer.m_index < t_iic_transfer.m_accessNumbBytes)
				I2cTx(MASTER, t_iic_transfer.m_paccessAddr[t_iic_transfer.m_index++]);
			else{
				
                //I2cSta(MASTER)          = I2CMSTA_MSTOP;
                I2cMCfg(I2CMCON_TXDMA_DIS | I2CMCON_RXDMA_DIS,
						//I2CMCON_IENCMP | I2CMCON_IENRX | I2CMCON_IENTX_DIS,
                        I2CMCON_IENCMP | I2CMCON_IENRX | I2CMCON_IENTX_DIS| I2CMCON_IENNACK,
						I2CMCON_MAS_EN); // TXREQ disabled to avoid multiple unecessary interrupts
            }
		}else{
			if (t_iic_transfer.m_subAddrCnt == 2) {
				I2cTx(MASTER, (t_iic_transfer.m_subAddr >> 8) & 0xff);
				t_iic_transfer.m_subAddrCnt--;
			}else if (t_iic_transfer.m_subAddrCnt == 1) {
				I2cTx(MASTER, t_iic_transfer.m_subAddr & 0xff);
				t_iic_transfer.m_subAddrCnt--;
				if(t_iic_transfer.m_accessCtrl == I2C_WRITE){
					t_iic_transfer.m_accessCtrl  = I2C_NONE;
				}
			}else if (t_iic_transfer.m_subAddrCnt == 0) {
            #if 0
				if(t_iic_transfer.m_accessCtrl == I2C_READ){
					//t_iic_transfer.m_accessCtrl  = I2C_NONE;
				}
            #endif
			}
		}
	}
	if ((uiStatus & I2CMSTA_TCOMP_SET) == I2CMSTA_TCOMP_SET){ // communication complete
		I2cMCfg(I2CMCON_TXDMA_DIS | I2CMCON_RXDMA_DIS,
				def_I2C_INT_SOURCE,
				I2CMCON_MAS_EN); // TXREQ enabled for future master transmissions
		t_iic_transfer.m_accessFinished 	= TRUE;
	}
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
