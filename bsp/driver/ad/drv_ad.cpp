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

//static int uxADC0Data[16]; // ADC0 Data, updated in ADC0 interrupt
static int uxADC1Data[16]; // ADC1 Data, updated in ADC1 interrupt
static float fADCTemp[RECORD_LEN];
static float fADCPres[RECORD_LEN];
static float fADCRHS[RECORD_LEN];

int iADCTemp[RECORD_LEN];
int iADCRHS[RECORD_LEN];

float fPreRHSADCVal;
signed char cRLPFRatioChangeNum;

volatile unsigned long ulDmaStatus = 0;
//volatile unsigned char ucADC0Status;  // ADC status, updated in ADC0 interrupt
//volatile unsigned char ucADC1Status;  // ADC status, updated in ADC1 interrupt

//volatile unsigned char ucADC0NewData; // New data flag, set in ADC0 interrupt
volatile unsigned char ucADC1NewData; // New data flag, set in ADC1 interrupt
//volatile unsigned char ucADC0RunStus;
volatile unsigned char ucADC1RunStus;


void AD_DMAINIT(void)
{
	DmaBase();
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
	AdcDmaReadSetup(ADC1DMAREAD,DMA_SIZE_WORD|DMA_DSTINC_WORD|DMA_SRCINC_NO|DMA_BASIC,16,uxADC1Data);
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

    ADC1_Switch_to_RTD();
    DmaClr(DMARMSKCLR_ADC1,0,0,0);                     // Clear Masking of ADC1 DMA channel
	DmaCycleCntCtrl(ADC1_C,16,DMA_DSTINC_WORD|
                              DMA_SRCINC_NO|DMA_SIZE_WORD|DMA_BASIC);
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_CONT);		            // Start ADC1 for continuous conversions
}
void ADC1_StartPRS_ADC(void)
{

    ADC1_Switch_to_PrsSensor();
    DmaClr(DMARMSKCLR_ADC1,0,0,0);                     // Clear Masking of ADC1 DMA channel
	DmaCycleCntCtrl(ADC1_C,16,DMA_DSTINC_WORD|
                              DMA_SRCINC_NO|DMA_SIZE_WORD|DMA_BASIC);
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_CONT);		            // Start ADC1 for continuous conversions
}
void ADC1_StartRHS_ADC(void)
{
    ADC1_Switch_to_RHS();
    DmaClr(DMARMSKCLR_ADC1,0,0,0);                     // Clear Masking of ADC1 DMA channel
	DmaCycleCntCtrl(ADC1_C,16,DMA_DSTINC_WORD|
                              DMA_SRCINC_NO|DMA_SIZE_WORD|DMA_BASIC);
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_CONT);		            // Start ADC1 for continuous conversions
}

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

#ifndef ADC_SOFT_FILTER
float ADC1_GetADC(void)
{
    return GetAvarageValue(uxADC1Data,16);
}
#else

//递推中位值平均滤波
float CombiFilter(float fADC,float fAry[])
{
    signed char i;
    float max,min;
	float sum;
	if( (fAry[0] < 0.000001) && (fAry[0] > -0.000001) ) //采样值为0 初始化队列
	{
	   for(i=RECORD_LEN-1;i>=0;i--)
	      fAry[i] = fADC;
	}

	max = min = sum = fAry[0] = fADC;
	for(i=RECORD_LEN-1;i!=0;i--)
	{
	    if(fAry[i]>max) max = fAry[i];
		else if(fAry[i]<min) min = fAry[i];
		sum += fAry[i];
		fAry[i] = fAry[i-1];
	}
	i = RECORD_LEN-2;
	sum = sum - max - min + i/2;
	sum /= i;
	return (sum);
}
#define CHANG_LIMIT  0x0A
float RapidLPFilter(float fNewVal,float fPreVal,signed char *cChgNum)
{
    float fDelta;
	signed char cRatio=1;

    if( (fNewVal < 0.000001) && (fNewVal > -0.000001) )
	    return (float)fNewVal;

	if(fNewVal>fPreVal)
	{
	    fDelta = fNewVal-fPreVal;
		if(fDelta>800000)
	        return (float)fNewVal;

	    if(*cChgNum<0)
		{
		    *cChgNum=0;
		}
		else
		{
		    if(*cChgNum >= CHANG_LIMIT)
	    	{
			    cRatio = (fDelta/50000)+1;
		        if(cRatio> 16)
		            cRatio = 16;
		    }
			else
			    (*cChgNum)++;
		}

	}
	else if(fNewVal<fPreVal)
	{
	    fDelta = fPreVal-fNewVal;
		if(fDelta>800000)
	        return (float)fNewVal;

	    if(*cChgNum>0)
		    *cChgNum=0;
		else
		{
		    if( *cChgNum <= (0-CHANG_LIMIT) )
		    {
			    cRatio = (fDelta/50000)+1;
		        if(cRatio> 16)
		            cRatio = 16;
		    }
			else
	            (*cChgNum)--;
		}
	}
	else
	    return (float)fNewVal;

    return (fPreVal + cRatio*(fNewVal-fPreVal)/16);
}
//
float ADC1_GetADC_RHS(void)
{
    float fNewRHS;
    fNewRHS = CombiFilter(GetAvarageValue(uxADC1Data,16),fADCRHS);
    fPreRHSADCVal = RapidLPFilter(fNewRHS,fPreRHSADCVal,&cRLPFRatioChangeNum);
    return fPreRHSADCVal;
}
//GetFltRsutTemp
float ADC1_GetADC_Temp(void)
{
    return CombiFilter(GetAvarageValue(uxADC1Data,16),fADCTemp);
}
//GetFltRsutTemp
float ADC1_GetADC_Pres(void)
{
    return CombiFilter(GetAvarageValue(uxADC1Data,16),fADCPres);
}
#endif



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
	AD_DMAINIT();
    DioCfgPin(pADI_GP0,PIN0,0); //Set P0.0 as GPIO
	DioOenPin(pADI_GP0,PIN0,1); //Enable P0.0 Output
    DioPulPin(pADI_GP0,PIN0,1); //Enable P0.0 Output Pullup 

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
    
    
    return DEVICE_OK;
}

#if 0
static portSSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){
      
    return size;
}
#endif

static portSSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
  
	char 	*ptr		= reinterpret_cast<char *>(buffer);

    return size;
}

    

static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args)
{
	DeviceStatus_TYPE   rt  = DEVICE_OK;
    uint32  value           = reinterpret_cast<uint32>(args);  
    
    if (cmd >= AD_IOC_MAX_NO){
        return DEVICE_ECMD_INVALID;
    }
    
    switch (cmd) {
        case AD_IOC_ECS0_SET:
        case AD_IOC_ECS1_SET:
/*            
            if (value == ECS_CUR_CLOSE){
                CBI(pADI_ANA->IEXCCON, ((cmd == AD_IOC_ECS0_SET)?(2):(5)));
            }else if (value == ECS_CUR_EXTRA_10UA){
                SBI(pADI_ANA->IEXCDAT, 0);
            }else {
                //IEXCDAT_IDAT_0uA
                IexcDat(value, (BIT_IS_SET(pADI_ANA->IEXCDAT, 0)));
                
                //pADI_ANA->IEXCDAT = (pADI_ANA->IEXCDAT&(BIT5|BIT4|BIT3|BIT2|BIT1))|iexcdat[value];
            }
*/
            break;

        default:
            rt              = DEVICE_ECMD_INVALID;
	}

	return rt;
}

static DevicePoll_TYPE _drv_poll(pDeviceAbstract pdev)
{

	return DEVICE_POLLIN;
	//return DEVICE_POLLNONE;
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
void DMA_ADC0_Int_Handler ()
{
    //ulDmaStatus = DmaSta();
	//ucADC0NewData++;
    //AdcGo(pADI_ADC0,ADCMDE_ADCMD_IDLE);
	//DmaSet(DMARMSKSET_ADC0,DMAENSET_ADC0,0,DMAPRISET_ADC0);
}

// ****************************************************************************
// ADC 1 DMA INTERRUPT
// ****************************************************************************
void DMA_ADC1_Int_Handler ()
{
	ulDmaStatus = DmaSta();
	ucADC1NewData++;
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_IDLE);
	DmaSet(DMARMSKSET_ADC1,DMAENSET_ADC1,0,DMAPRISET_ADC1);
}
void DMA_Err_Int_Handler ()
{
	ulDmaStatus = DmaSta();
	DmaErr(DMA_ERR_CLR);
}



/*********************************************************************************
**                            End Of File
*********************************************************************************/
