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

static void adc0_init(ADI_ADC_TypeDef *padc);
static void adc1_init(ADI_ADC_TypeDef *padc);

enum{
    enum_ADC0       = 0,
    enum_ADC1,
    enum_ADC_MAX_NO,
};

struct adc_resu{
    
    volatile uint32         m_value;
    volatile uint32         m_sta;
    volatile bool           m_rdy;
};

class adc:noncopyable{
public:
    adc()
    {
        m_resu[enum_ADC0].m_rdy     = false; 
        m_resu[enum_ADC1].m_rdy     = false;
    }
    ~adc(){}
    
    void init(portuBASE_TYPE index)
    {
        if (index == enum_ADC0){
            adc0_init(pADI_ADC0);
        }else {
            adc1_init(pADI_ADC1);
        }
    }
        
    void cpy(portuBASE_TYPE index)
    {
        ADI_ADC_TypeDef *type   = typedef_get(index);
        
        m_resu[index].m_sta     = AdcSta(type);        // read ADC status register
        m_resu[index].m_value   = AdcRd(type);         // read ADC result register
        m_resu[index].m_rdy     = true; 
    }

    void go(portuBASE_TYPE index, uint32 cmd)
    {
        AdcGo(typedef_get(index),cmd); 
    }

    void wait(portuBASE_TYPE index)
    {
        while (!m_resu[index].m_rdy); 
        m_resu[index].m_rdy         = false;
    }
    
    uint32 value(portuBASE_TYPE index)
    {
        return m_resu[index].m_value; 
    }

    uint32 sta(portuBASE_TYPE index)
    {
        return m_resu[index].m_sta; 
    }
    
    void int_enable(portuBASE_TYPE index)
    {
        NVIC_EnableIRQ((index == enum_ADC0)?(ADC0_IRQn):(ADC1_IRQn));
    }

private:
    ADI_ADC_TypeDef *typedef_get(portuBASE_TYPE index)
    {
        return (index == enum_ADC0)?(pADI_ADC0):(pADI_ADC1);
    }
        
private:
    
    struct adc_resu         m_resu[enum_ADC_MAX_NO];
};    

class adc       t_adc; 

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

static void adc0_init(ADI_ADC_TypeDef *padc)
{
    AdcGo(padc,ADCMDE_ADCMD_IDLE);                  // Place ADC1 in Idle mode
    
    //ADC_M_NONE
    AdcMski(padc,ADCMSKI_RDY,1);              // Enable ADC ready interrupt source          
    //enable sinc3 filter: sample freq less than 500Hz
    //ADC filter set for 3.75Hz update rate with chop on enabled  
    //NOTCH2 enable: can generate notches at both 50Hz and 60Hz
    //when chop enable, auto enable RAVG2
    AdcFlt(padc,124,14,FLT_NORMAL|ADCFLT_NOTCH2|ADCFLT_CHOP);
    // Internal reference selected, Gain of 4, Signed integer output
    AdcRng(padc,ADCCON_ADCREF_INTREF,ADCMDE_PGA_G4,ADCCON_ADCCODE_INT); 
    // Turn off input buffers to ADC and external reference      
    //AdcBuf(padc,ADCCFG_EXTBUF_OFF,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN); 
    // Turn on input buffers   ADC_BUF_ON for both reference buffers on
    // ADCxCFG:EXTBUF   11: External buffer on VREF+ is enabled. External buffer on VREF? is powered down and bypassed.
    AdcBuf(padc,ADCCFG_EXTBUF_OFF,ADC_BUF_ON);
    // Select AIN6 as postive input and AIN7 as negative input
    AdcPin(padc,ADCCON_ADCCN_AIN6,ADCCON_ADCCP_AIN7);
}

static void adc1_init(ADI_ADC_TypeDef *padc)
{
    AdcGo(padc,ADCMDE_ADCMD_IDLE);                  // Place ADC1 in Idle mode
    
    //ADC_M_NONE
    AdcMski(padc,ADCMSKI_RDY,1);              // Enable ADC ready interrupt source          
    //enable sinc3 filter: sample freq less than 500Hz
    //ADC filter set for 3.75Hz update rate with chop on enabled  
    //NOTCH2 enable: can generate notches at both 50Hz and 60Hz
    //when chop enable, auto enable RAVG2
    AdcFlt(padc,124,14,FLT_NORMAL|ADCFLT_NOTCH2|ADCFLT_CHOP);
    // Internal reference selected, Gain of 4, Signed integer output
    AdcRng(padc,ADCCON_ADCREF_INTREF,ADCMDE_PGA_G4,ADCCON_ADCCODE_INT); 
    // Turn off input buffers to ADC and external reference      
    //AdcBuf(padc,ADCCFG_EXTBUF_OFF,ADCCON_BUFBYPN|ADCCON_BUFBYPP|ADCCON_BUFPOWP|ADCCON_BUFPOWN); 
    // Turn on input buffers   ADC_BUF_ON for both reference buffers on
    // ADCxCFG:EXTBUF   11: External buffer on VREF+ is enabled. External buffer on VREF- is powered down and bypassed.
    AdcBuf(padc,3,ADC_BUF_ON);
    // Select AIN2 as postive input and AIN3 as negative input
    AdcPin(padc,ADCCON_ADCCN_AIN2,ADCCON_ADCCP_AIN3);
    
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
    IexcCfg(IEXCCON_PD_off,IEXCCON_REFSEL_Int,IEXCCON_IPSEL1_AIN4, IEXCCON_IPSEL0_AIN5);
    IexcDat(IEXCDAT_IDAT_0uA,IDAT0Dis);
    
    t_adc.init(enum_ADC0);
    t_adc.init(enum_ADC1);
    
    // Start ADC0 for continuous conversions
    t_adc.go(enum_ADC0, ADCMDE_ADCMD_CONT);
    // Start ADC1 for continuous conversions
    t_adc.go(enum_ADC1, ADCMDE_ADCMD_CONT);
    
    t_adc.int_enable(enum_ADC0);
    t_adc.int_enable(enum_ADC1);
    
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

#if 0

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
#endif
    

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
                //IEXCDAT_IDAT_0uA
                IexcDat(value, (BIT_IS_SET(pADI_ANA->IEXCDAT, 0)));
                
                //pADI_ANA->IEXCDAT = (pADI_ANA->IEXCDAT&(BIT5|BIT4|BIT3|BIT2|BIT1))|iexcdat[value];
            }
            break;

        default:
            rt              = DEVICE_ECMD_INVALID;
	}

	return rt;
}

extern "C" void ADC0_Int_Handler ()
{
    t_adc.cpy(enum_ADC0);
}

extern "C" void ADC1_Int_Handler ()
{
    t_adc.cpy(enum_ADC1);
}

extern "C" void SINC2_Int_Handler()
{
 
}



/*********************************************************************************
**                            End Of File
*********************************************************************************/
