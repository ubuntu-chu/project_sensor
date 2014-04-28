/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "drv_ad.h"

/******************************************************************************
 *                           本文件静态函数声明
******************************************************************************/ 

static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag);
//static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args);

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
        //rx_indicate
        NULL,
        //tx_complete
        NULL,

    };

static DeviceAbstract  st_Device_ad                            = {

        //设备信息
        &st_DeviceInfo_ad,
    };

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


static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev){
    
	
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

static void adc0_init(void)
{
    AdcGo(pADI_ADC0,ADCMDE_ADCMD_IDLE);                  // Place ADC1 in Idle mode
    
    //ADC_M_NONE
    AdcMski(pADI_ADC0,ADCMSKI_RDY,1);              // Enable ADC ready interrupt source          
    //enable sinc3 filter: sample freq less than 500Hz
    //ADC filter set for 3.75Hz update rate with chop on enabled  
    //NOTCH2 enable: can generate notches at both 50Hz and 60Hz
    //when chop enable, auto enable RAVG2
    AdcFlt(pADI_ADC0,124,14,FLT_NORMAL|ADCFLT_NOTCH2|ADCFLT_CHOP);
    // Internal reference selected, Gain of 4, Signed integer output
    AdcRng(pADI_ADC0,ADCCON_ADCREF_INTREF,ADCMDE_PGA_G4,ADCCON_ADCCODE_INT); 
    // Turn off input buffers to ADC and external reference      
    AdcBuf(pADI_ADC0,ADCCFG_EXTBUF_OFF,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN); 
    // Select AIN6 as postive input and AIN7 as negative input
    AdcPin(pADI_ADC0,ADCCON_ADCCN_AIN6,ADCCON_ADCCP_AIN7);
}

static void adc1_init(void)
{
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_IDLE);                  // Place ADC1 in Idle mode
    
    //ADC_M_NONE
    AdcMski(pADI_ADC1,ADCMSKI_RDY,1);              // Enable ADC ready interrupt source          
    //enable sinc3 filter: sample freq less than 500Hz
    //ADC filter set for 3.75Hz update rate with chop on enabled  
    //NOTCH2 enable: can generate notches at both 50Hz and 60Hz
    //when chop enable, auto enable RAVG2
    AdcFlt(pADI_ADC1,124,14,FLT_NORMAL|ADCFLT_NOTCH2|ADCFLT_CHOP);
    // Internal reference selected, Gain of 4, Signed integer output
    AdcRng(pADI_ADC1,ADCCON_ADCREF_INTREF,ADCMDE_PGA_G4,ADCCON_ADCCODE_INT); 
    // Turn off input buffers to ADC and external reference      
    AdcBuf(pADI_ADC1,ADCCFG_EXTBUF_OFF,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN); 
    // Select AIN2 as postive input and AIN3 as negative input
    AdcPin(pADI_ADC1,ADCCON_ADCCN_AIN2,ADCCON_ADCCP_AIN3);
    
}

static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag){
    
    //使能电流源
    //REFPD = 0   This bit must be cleared for the ADCs to work, regardless of if an external 
    //reference is selected
    pADI_ANA->REFCTRL                       = 0;
    //PD        0: Enable excitation current source block.
    //REFSEL    1: Select internal current reference resistor source
    //IPSEL1    100: IEXC1 output on AIN4.
    //IPSEL0    101: IEXC0 output on AIN5.
    pADI_ANA->IEXCCON                       = BIT6|(4UL<<3)|(5UL<<0);
    pADI_ANA->IEXCDAT                       = 0;
    
    adc0_init();
    adc1_init();
    
    // Start ADC0 for continuous conversions
    AdcGo(pADI_ADC0,ADCMDE_ADCMD_CONT); 
    // Start ADC1 for continuous conversions
    AdcGo(pADI_ADC1,ADCMDE_ADCMD_CONT); 
    
    return DEVICE_OK;
}

#if 0
static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){
      
    return size;
}
#endif

static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
  
    return size; 
}

static const uint8 iexcdat[ECS_CUR_MAX_NO] = {
    
    //IEXCDAT[5:3]  IEXCDAT[2:1] IEXCDAT[0] 
    //ECS_CUR_0UA
    (0UL<<3)|(0UL<<1)|(0UL<<0),
    //ECS_CUR_50UA
    (1UL<<3)|(0UL<<1)|(0UL<<0),
    //ECS_CUR_100UA
    (1UL<<3)|(1UL<<1)|(0UL<<0),
    //ECS_CUR_150UA
    (1UL<<3)|(2UL<<1)|(0UL<<0),
    //ECS_CUR_200UA
    (1UL<<3)|(3UL<<1)|(0UL<<0),
    //ECS_CUR_250UA
    (5UL<<3)|(0UL<<1)|(0UL<<0),
    //ECS_CUR_300UA
    (2UL<<3)|(2UL<<1)|(0UL<<0),
    //ECS_CUR_400UA
    (2UL<<3)|(3UL<<1)|(0UL<<0),
    //ECS_CUR_450UA
    (3UL<<3)|(2UL<<1)|(0UL<<0),
    //ECS_CUR_500UA
    (5UL<<3)|(1UL<<1)|(0UL<<0),
    //ECS_CUR_600UA
    (3UL<<3)|(3UL<<1)|(0UL<<0),
    //ECS_CUR_750UA
    (5UL<<3)|(2UL<<1)|(0UL<<0),
    //ECS_CUR_800UA
    (4UL<<3)|(3UL<<1)|(0UL<<0),
    //ECS_CUR_1000UA
    (5UL<<3)|(3UL<<1)|(0UL<<0),
};
    

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
            
            if (value == ECS_CUR_CLOSE){
                CBI(pADI_ANA->IEXCCON, ((cmd == AD_IOC_ECS0_SET)?(2):(5)));
            }else if (value == ECS_CUR_EXTRA_10UA){
                SBI(pADI_ANA->IEXCDAT, 0);
            }else {
                pADI_ANA->IEXCDAT = (pADI_ANA->IEXCDAT&(BIT5|BIT4|BIT3|BIT2|BIT1))|iexcdat[value];
            }
            break;

        default:
            rt              = DEVICE_ECMD_INVALID;
	}

	return rt;
}

extern "C" void ADC0_Int_Handler ()
{
   volatile unsigned int uiADCSTA = 0;
   
   uiADCSTA = AdcSta(pADI_ADC0);               // read ADC status register
   AdcRd(pADI_ADC0);            // read ADC result register
}

extern "C" void ADC1_Int_Handler ()
{
   volatile unsigned int uiADCSTA = 0;
   
   uiADCSTA = AdcSta(pADI_ADC1);               // read ADC status register
   AdcRd(pADI_ADC1);            // read ADC result register
}



/*********************************************************************************
**                            End Of File
*********************************************************************************/
