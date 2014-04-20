/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "cpu_iic.h"
#include "../../includes/macro.h"


volatile I2CCtrlInfo t_I2CCtrlInfo = {0};

/******************************************************************************
 *                       本文件所定义的静态全局变量
******************************************************************************/ 

static portBASE_TYPE cpu_iic_interrupt_enable(enum iic_numb numb);

portBASE_TYPE cpu_iic_init(enum iic_numb numb)
{
    portBASE_TYPE  rt   = 0;
    
    if (numb > enum_NUMB_IIC_MAX){
        return -1;
    }
    if (numb == enum_NUMB_IIC0){
        // Configure P2.0/P2.1 as I2C pins   p2.0-SCL   p2.1-SDA
		pADI_GP2->GPCON = ((pADI_GP2->GPCON)&(~(BIT0|BIT1|BIT2|BIT3)))|0x5;
        pADI_GP2->GPPUL = ((pADI_GP2->GPPUL)&(~(BIT0|BIT1))); //disable external pull ups
    }
	// Enable I2C Master mode, baud rate and interrupt sources
	I2cMCfg(I2CMCON_TXDMA_DIS | I2CMCON_RXDMA_DIS,
			I2CMCON_IENCMP | I2CMCON_IENRX | I2CMCON_IENTX, I2CMCON_MAS_EN);
	I2cBaud(0x4E, 0x4F); // 100kHz clock
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
		NVIC_EnableIRQ(I2CM_IRQn);
    }

    return rt;
}

portSIZE_TYPE cpu_iic_transfer(struct iic_ctrl *pctrl)
{
	portSIZE_TYPE 	rt;



	return rt;
}

// I2C0 master handler
extern "C" void I2C0_Master_Int_Handler(void)
{
#if 0
  unsigned int uiStatus;
  uiStatus = I2cSta(MASTER);
  if((uiStatus & I2CMSTA_RXREQ) == I2CMSTA_RXREQ)       // Master Recieve IRQ
  {
    ucMasterRxDat[uiMasterRxIndex++] = I2cRx(MASTER);
    if(uiMasterRxIndex > (sizeof(ucMasterRxDat)-1) )                     // Resetting value of i if it has been incremented by RX
      uiMasterRxIndex = 0;
  }

  if((uiStatus & I2CMSTA_TXREQ) == I2CMSTA_TXREQ) // Master Transmit IRQ
  {
    if (uiMasterTxIndex < sizeof(ucMasterTxDat) )
      I2cTx(MASTER, ucMasterTxDat[uiMasterTxIndex++]);
    else
      I2cMCfg(I2CMCON_TXDMA_DIS|I2CMCON_RXDMA_DIS, I2CMCON_IENCMP|I2CMCON_IENRX|I2CMCON_IENTX_DIS, I2CMCON_MAS_EN);    // TXREQ disabled to avoid multiple unecessary interrupts
  }
  if((uiStatus & I2CMSTA_TCOMP_SET) == I2CMSTA_TCOMP_SET) // communication complete
  {
    I2cMCfg(I2CMCON_TXDMA_DIS|I2CMCON_RXDMA_DIS, I2CMCON_IENCMP|I2CMCON_IENRX|I2CMCON_IENTX_EN, I2CMCON_MAS_EN);   // TXREQ enabled for future master transmissions
    ucComplete = 1;
  }
#endif
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
