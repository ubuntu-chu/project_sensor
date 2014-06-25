/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "drv_ad.h"

/******************************************************************************
 *                           本文件静态函数声明
******************************************************************************/ 

static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag);
//static portSSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args);
static DevicePoll_TYPE _drv_poll(pDeviceAbstract pdev);

/******************************************************************************
 *                       本文件所定义的静态数据结构
******************************************************************************/

static const DeviceAbstractInfo st_DeviceInfo_ad = {

        //设备名字
        DEVICE_NAME_AD,
        //设备类型->字符设备
        DEVICE_CLASS_CHAR,
        //设备标识->只读设备
        DEVICE_FLAG_RDONLY,
        //init
        _drv_devinit,
        //open
        _drv_devopen,
        //close
        NULL,
        //read
        _drv_devread,
        //write
        NULL,
        //control
        _drv_ioctl,
        //poll
        _drv_poll,
        //rx_indicate
        NULL,
        //tx_complete
        NULL,

    };

static DeviceAbstract  st_Device_ad                            = {

        //设备信息
        &st_DeviceInfo_ad,
    };

/*
int AdcRd(ADI_ADC_TypeDef *pPort) ==========Reads the ADC status.

Parameters
pPort	:{pADI_ADC0, pADI_ADC1}
pADI_ADC0 for ADC0.
pADI_ADC1 for ADC1.
Returns
ADC data (ADCxDAT). MSb of data is bit 27. Bits 28 to 31 give extended sign. 
Value of 0x0fffffff => VRef. 
Value of 0x00000000 => 0V. 
Value of 0xF0000000 => -Vref. 
Warning
Returns ADCxDAT even if it does not contain new data.
*/

static volatile int uxADC1Data[def_AD_SAMPLE_LEN]; // ADC1 Data, updated in ADC1 interrupt

enum adc_status{
	ADC_STAT_NONE = 0,
	ADC_STAT_READABLE,
	ADC_STAT_READABLE_FAKE,
	ADC_STAT_IN_PROCESSING,
};

volatile enum adc_status 	adc_stat 		= ADC_STAT_NONE;

void AD_DMAINIT(void)
{
    DmaSet(0,DMAENSET_ADC1,0,DMAPRISET_ADC1);       // Enable ADC0 && ADC1 DMA primary structure
}

//ADC0 SINGLE
void ADC0_Init(void)
{
    AdcGo(pADI_ADC0,ADCMDE_ADCMD_IDLE);          // Place ADC1 in Idle mode
    //AdcMski(pADI_ADC0,ADCMSKI_RDY,1);

    //AdcFlt(pADI_ADC0,124,14,FLT_NORMAL|ADCFLT_NOTCH2|ADCFLT_CHOP);    // ADC filter set for 3.75Hz update rate with chop on enabled
	//AdcFlt(pADI_ADC0,125,9,FLT_NORMAL|ADCFLT_NOTCH2|ADCFLT_CHOP);   //4.12Hz
	//AdcFlt(pADI_ADC0,126,2,FLT_NORMAL|ADCFLT_NOTCH2|ADCFLT_CHOP);  //6.0Hz
	//AdcFlt(pADI_ADC0,127,0,FLT_NORMAL|ADCFLT_NOTCH2|ADCFLT_CHOP);   // 8.239Hz
	AdcFlt(pADI_ADC0,125,0,FLT_NORMAL|ADCFLT_NOTCH2);   //49.4Hz
}
int ADC0_GetSglADC_RTD()
{
    IexcDat(IEXCDAT_IDAT_50uA,IDAT0Dis);         // Set output for 50uA
    IexcCfg(IEXCCON_PD_off,IEXCCON_REFSEL_Int,IEXCCON_IPSEL1_Off,IEXCCON_IPSEL0_AIN5);

    AdcGo(pADI_ADC1,ADCMDE_ADCMD_IDLE);
    AdcFlt(pADI_ADC1,125,0,FLT_NORMAL|ADCFLT_NOTCH2);   //49.4Hz
    // Turn off input buffers to ADC and external reference
    //AdcBuf(pADI_ADC0,ADCCFG_EXTBUF_OFF,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN);
    //AdcBuf(pADI_ADC0,ADCCFG_EXTBUF_OFF,ADC_BUF_ON);
    AdcBuf(pADI_ADC1,ADCCFG_EXTBUF_VREFP_VREF2P,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN);

    AdcPin(pADI_ADC1,ADCCON_ADCCN_AIN7,ADCCON_ADCCP_AIN6);            // Select AIN6 as postive input and AIN7 as negative input
    //AdcRng(pADI_ADC0,ADCCON_ADCREF_INTREF,ADCMDE_PGA_G8,ADCCON_ADCCODE_UINT);  // Internal reference selected, Gain of 8, Signed integer output
    AdcRng(pADI_ADC1,ADCCON_ADCREF_EXTREF2,ADCMDE_PGA_G16,ADCCON_ADCCODE_UINT);

    AdcGo(pADI_ADC1,ADCMDE_ADCMD_SINGLE);

    while(!(AdcSta(pADI_ADC1)&0x01));

    return AdcRd(pADI_ADC1);
}
int ADC0_GetSglADC_RHS()
{
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_IDLE);
    AdcFlt(pADI_ADC1,125,0,FLT_NORMAL|ADCFLT_NOTCH2);   //49.4Hz
    // Turn off input buffers to ADC and internal reference
    //AdcBuf(pADI_ADC0,ADCCFG_EXTBUF_OFF,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN);
    //AdcBuf(pADI_ADC0,ADCCFG_EXTBUF_OFF,ADC_BUF_ON);
    AdcBuf(pADI_ADC1,ADCCFG_EXTBUF_OFF,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN);

    AdcPin(pADI_ADC1,ADCCON_ADCCN_AGND,ADCCON_ADCCP_AIN2);            // Select AIN2 as postive input and AGND as negative input
    //AdcRng(pADI_ADC0,ADCCON_ADCREF_INTREF,ADCMDE_PGA_G8,ADCCON_ADCCODE_UINT);  // Internal reference selected, Gain of 32, Signed integer output
    AdcRng(pADI_ADC1,ADCCON_ADCREF_INTREF,ADCMDE_PGA_G1,ADCCON_ADCCODE_UINT);

    AdcGo(pADI_ADC1,ADCMDE_ADCMD_SINGLE);

    while(!(AdcSta(pADI_ADC1)&0x01));

    return AdcRd(pADI_ADC1);
}
// ****************************************************************************
// ADC 1 RTD INITIALISATION
// ****************************************************************************
// Measures RTD (PT1000) voltage
// Input(+) buffered, AIN6, via RC filter to RTD
// Input(-) buffered, AIN7, via RC filter to RTD
// Gain = 16
// Reference ext. based on Reference Resistor( 37.4kohm 10ppm )
// Conversion continuous

void ADC1_Init(void)
{
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_IDLE);          // Place ADC1 in Idle mode
    AdcMski(pADI_ADC1,ADCMSKI_RDY,1);

    AdcFlt(pADI_ADC1,125,0,FLT_NORMAL|ADCFLT_NOTCH2);   //49.4Hz

    AdcDmaCon(ADC1DMAREAD,1);     // Call function to init ADc1 for DMA reads
	AdcDmaReadSetup(ADC1DMAREAD,DMA_SIZE_WORD|DMA_DSTINC_WORD|DMA_SRCINC_NO|DMA_BASIC,def_AD_SAMPLE_LEN, (int *)uxADC1Data);
    //NVIC_EnableIRQ(ADC1_IRQn); // Enable ADC interrupt
    NVIC_EnableIRQ(DMA_ADC1_IRQn);

} // ADC1_Init_RTD

void ADC1_Switch_to_RTD(void)
{
    IexcDat(IEXCDAT_IDAT_50uA,IDAT0Dis);         // Set output for 50uA
    IexcCfg(IEXCCON_PD_off,IEXCCON_REFSEL_Int,IEXCCON_IPSEL1_Off,IEXCCON_IPSEL0_AIN5);

    // Turn off input buffers to ADC and external reference
    //AdcBuf(pADI_ADC1,ADCCFG_EXTBUF_VREFP_VREF2P,ADC_BUF_ON);
    AdcBuf(pADI_ADC1,ADCCFG_EXTBUF_VREFP_VREF2P,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN);
    // Select AIN6 as postive input and AIN7 as negative input
    AdcPin(pADI_ADC1,ADCCON_ADCCN_AIN7,ADCCON_ADCCP_AIN6);
	AdcRng(pADI_ADC1,ADCCON_ADCREF_EXTREF2,ADCMDE_PGA_G16,ADCCON_ADCCODE_UINT);
}
void ADC1_Switch_to_PrsSensor(void)
{
    IexcDat(IEXCDAT_IDAT_400uA,IDAT0Dis);         // Set output for 400uA
	IexcCfg(IEXCCON_PD_off,IEXCCON_REFSEL_Int,IEXCCON_IPSEL1_AIN4,IEXCCON_IPSEL0_Off);
    // Turn off input buffers to ADC and external reference
    //AdcBuf(pADI_ADC1,ADCCFG_EXTBUF_VREFP_VREF2P,ADC_BUF_ON);
    AdcBuf(pADI_ADC1,ADCCFG_EXTBUF_OFF,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN);
    // Select AIN0 as postive input and AIN1 as negative input
    AdcPin(pADI_ADC1,ADCCON_ADCCN_AIN1,ADCCON_ADCCP_AIN0);
    //AdcPin(pADI_ADC1,ADCCON_ADCCN_AIN0,ADCCON_ADCCP_AIN1);
    AdcRng(pADI_ADC1,ADCCON_ADCREF_INTREF,ADCMDE_PGA_G8,ADCCON_ADCCODE_INT);  // Internal reference selected, Gain of 8, Signed integer output
}
void ADC1_Switch_to_RHS(void)
{
    IexcDat(IEXCDAT_IDAT_0uA,IDAT0Dis);         // Set output for 400uA
	IexcCfg(IEXCCON_PD_En,IEXCCON_REFSEL_Int,IEXCCON_IPSEL1_Off,IEXCCON_IPSEL0_Off);
    // Turn off input buffers to ADC and external reference
    //AdcBuf(pADI_ADC1,ADCCFG_EXTBUF_VREFP_VREF2P,ADC_BUF_ON);
    AdcBuf(pADI_ADC1,ADCCFG_EXTBUF_OFF,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN);
    // Select AIN2 as postive input and AGND as negative input
    AdcPin(pADI_ADC1,ADCCON_ADCCN_AGND,ADCCON_ADCCP_AIN2);
	AdcRng(pADI_ADC1,ADCCON_ADCREF_INTREF,ADCMDE_PGA_G1,ADCCON_ADCCODE_UINT);
}


void ADC1_StartRTD_ADC(void)
{
	adc_stat 								= ADC_STAT_IN_PROCESSING;
    ADC1_Switch_to_RTD();
    DmaClr(DMARMSKCLR_ADC1,0,0,0);                     // Clear Masking of ADC1 DMA channel
	DmaCycleCntCtrl(ADC1_C,16,DMA_DSTINC_WORD|
                              DMA_SRCINC_NO|DMA_SIZE_WORD|DMA_BASIC);
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_CONT);		            // Start ADC1 for continuous conversions
}
void ADC1_StartPRS_ADC(void)
{
	adc_stat 								= ADC_STAT_IN_PROCESSING;
    ADC1_Switch_to_PrsSensor();
    DmaClr(DMARMSKCLR_ADC1,0,0,0);                     // Clear Masking of ADC1 DMA channel
	DmaCycleCntCtrl(ADC1_C,16,DMA_DSTINC_WORD|
                              DMA_SRCINC_NO|DMA_SIZE_WORD|DMA_BASIC);
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_CONT);		            // Start ADC1 for continuous conversions
}
void ADC1_StartRHS_ADC(void)
{
	adc_stat 								= ADC_STAT_IN_PROCESSING;
    ADC1_Switch_to_RHS();
    DmaClr(DMARMSKCLR_ADC1,0,0,0);                     // Clear Masking of ADC1 DMA channel
	DmaCycleCntCtrl(ADC1_C,16,DMA_DSTINC_WORD|
                              DMA_SRCINC_NO|DMA_SIZE_WORD|DMA_BASIC);
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_CONT);		            // Start ADC1 for continuous conversions
}

/******************************************************************************
 *  函数名称 :
 *
 *  函数功能 : 设备注册
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
 *  附    注 :
 *
 *
******************************************************************************/

portuBASE_TYPE drv_adregister(void){

    return API_DeviceRegister(&st_Device_ad);
}


static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev)
{
	return DEVICE_OK;
}

/*
A write to ADCxMDE resets the corresponding ADC, including the RDY bits and other ADCxSTA flags. 
Depending on the clock used by the ADC, there may be a delay between when the register is 
written and when the setting takes effect for the ADC logic. Therefore, the 
more reliable method to reset the RDY bit is to read ADCxDAT. 

The user can write to the offset and gain registers only when the ADC is in idle 
or power-down mode or when ADCEN= 0 (ADCxCON[19]). However, a software delay is required 
between writing to ADCMDE and writing to the offset or gain register.
*/


static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag){
    
    AD_DMAINIT();
    ADC0_Init();
	ADC1_Init();
    adc_stat 								= ADC_STAT_NONE;
    return DEVICE_OK;
}

#if 0
static portSSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){
      
    return size;
}
#endif

float GetAvarageValue(int iAry[],char cnt)
{
    unsigned char i;
    float fVal;
    for(i=0;i<cnt;i++)
    {
        fVal += (iAry[i]);
    }

    return (fVal/cnt);
}

static portSSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
  
	char 	*ptr		= reinterpret_cast<char *>(buffer);

	if (adc_stat == ADC_STAT_NONE){
		return 0;
	}
	if (adc_stat == ADC_STAT_READABLE_FAKE){
		adc_stat 							= ADC_STAT_NONE;
		return 0;
	}
	adc_stat 								= ADC_STAT_NONE;
#ifdef		def_AVARAGE_IN_READ
	{
		float fadc 							= GetAvarageValue((int *)uxADC1Data, def_AD_SAMPLE_LEN);
		if (size > sizeof(fadc)){
			size							= sizeof(fadc);
		}
		*(float *)ptr 						= fadc;
	}
#else
	{
		if (size > sizeof(uxADC1Data)){
			size							= sizeof(uxADC1Data);
		}
		memcpy((char *)ptr, (char *)uxADC1Data, size);
	}
#endif

    return size;
}

    

static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args)
{
	DeviceStatus_TYPE   rt  					= DEVICE_OK;
    int32  *pbuf           						= reinterpret_cast<int32 *>(args);
    
    if (cmd >= AD_IOC_MAX_NO){
        return DEVICE_ECMD_INVALID;
    }
    
    switch (cmd) {
	case AD_IOC_START_RTD_ADC:

		ADC1_StartRTD_ADC();
		break;

	case AD_IOC_START_RHS_ADC:

		ADC1_StartRHS_ADC();
		break;

	case AD_IOC_START_PRS_ADC:

		ADC1_StartPRS_ADC();
		break;

	case AD_IOC_FAKE_READABLE:

		if (adc_stat == ADC_STAT_NONE){
			adc_stat 							= ADC_STAT_READABLE_FAKE;
		}
		break;

	case AD_IOC_GET_SGL_RTD_ADC:
    case AD_IOC_GET_SGL_RHS_ADC:

		while (adc_stat == ADC_STAT_IN_PROCESSING);
        if (NULL != pbuf){
			if (cmd == AD_IOC_GET_SGL_RTD_ADC){
                *pbuf 							= ADC0_GetSglADC_RTD();
            }else {
                *pbuf 							= ADC0_GetSglADC_RHS();
            }
		}
        adc_stat 						        = ADC_STAT_NONE;
		break;

	default:
		rt = DEVICE_ECMD_INVALID;
	}

	return rt;
}

static DevicePoll_TYPE _drv_poll(pDeviceAbstract pdev)
{
	return ((adc_stat == ADC_STAT_READABLE_FAKE) || (adc_stat == ADC_STAT_READABLE))?(DEVICE_POLLIN):(DEVICE_POLLNONE);
}

// ****************************************************************************
// ADC 0 INTERRUPT
// ****************************************************************************

extern "C" void ADC0_Int_Handler ()
{
	//ucADC0Status = pADI_ADC0->STA;  // Read ADC status
	//lADC0Data = pADI_ADC0->DAT;     // Read ADC conversion result
	//ucADC0NewData++;                // Set ADC new data flag
}

// ****************************************************************************
// ADC 1 INTERRUPT
// ****************************************************************************

extern "C" void ADC1_Int_Handler ()
{
	//ucADC1Status = pADI_ADC1->STA;  // Read ADC status
	//lADC1Data = pADI_ADC1->DAT;     // Read ADC conversion result
	//ucADC1NewData++;                // Set ADC new data flag
}

extern "C" void SINC2_Int_Handler()
{
 
}

// ****************************************************************************
// ADC 0 DMA INTERRUPT
// ****************************************************************************
extern "C" void DMA_ADC0_Int_Handler ()
{
    //ulDmaStatus = DmaSta();
	//ucADC0NewData++;
    //AdcGo(pADI_ADC0,ADCMDE_ADCMD_IDLE);
	//DmaSet(DMARMSKSET_ADC0,DMAENSET_ADC0,0,DMAPRISET_ADC0);
}

// ****************************************************************************
// ADC 1 DMA INTERRUPT
// ****************************************************************************
extern "C" void DMA_ADC1_Int_Handler ()
{
	volatile unsigned long ulDmaStatus = DmaSta();
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_IDLE);
	DmaSet(DMARMSKSET_ADC1,DMAENSET_ADC1,0,DMAPRISET_ADC1);
	adc_stat 									= ADC_STAT_READABLE;
}
extern "C" void DMA_Err_Int_Handler ()
{
	volatile unsigned long ulDmaStatus = DmaSta();
	DmaErr(DMA_ERR_CLR);
}



/*********************************************************************************
**                            End Of File
*********************************************************************************/
