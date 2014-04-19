#include    "drv_pwm.h"


static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag);
static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args);

static const DeviceAbstractInfo st_DeviceInfo = {

        DEVICE_NAME_PWM,
        DEVICE_CLASS_CHAR,
        DEVICE_FLAG_RDWR,
        //init
        _drv_devinit,
        //open
        _drv_devopen,
        //close
        NULL,
        //read
        _drv_devread,
        //write
        _drv_devwrite,
        //control
        _drv_ioctl,
        //rx_indicate
        NULL,
        //tx_complete
        NULL,

    };

static DeviceAbstract  st_Device                            = {

        &st_DeviceInfo,
    };

portuBASE_TYPE drv_pwmregister(void){

    return API_DeviceRegister(&st_Device);
}

void pwm_output_disable(void)
{
    PwmGo(PWMCON0_ENABLE_DIS,PWMCON0_MOD_DIS);            // disable PWM outputs
}

void pwm_output_enable(void)
{
    PwmGo(PWMCON0_ENABLE_EN,PWMCON0_MOD_DIS);            // Enable PWM outputs
}

static void _pwm_rhref_pins_disable(void)
{
    pADI_GP1->GPCON = ((pADI_GP1->GPCON)&(~(BIT4|BIT5)));
    DioOen(pADI_GP1,(pADI_GP1->GPOEN)|BIT(2));
}

static void pwm_rhref_pins_disable(void)
{
    if (((pADI_GP1->GPCON)&(BIT4|BIT5))){
        _pwm_rhref_pins_disable();
    }
}

static void pwm_rhref_pins_enable(void)
{
    if (!(((pADI_GP1->GPCON)&(BIT4|BIT5)))){
        pADI_GP1->GPCON = ((pADI_GP1->GPCON)&(~(BIT4|BIT5)))|(BIT4);
    }
}

static void _pwm_heat_pins_disable(void)
{
    pADI_GP1->GPCON = ((pADI_GP1->GPCON)&(~(BIT8|BIT9)));
    DioOen(pADI_GP1, (pADI_GP1->GPOEN)|BIT(4));
}

static void pwm_heat_pins_disable(void)
{
    if (((pADI_GP1->GPCON)&(BIT8|BIT9))){
        _pwm_heat_pins_disable();
    }
}

static void pwm_heat_pins_enable(void)
{
    if (!(((pADI_GP1->GPCON)&(BIT8|BIT9)))){
        pADI_GP1->GPCON = ((pADI_GP1->GPCON)&(~(BIT8|BIT9)))|(BIT8);
    }
}
    
static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev){
    //PWM in standard mode
    PwmInit(UCLK_2,PWMCON0_PWMIEN_DIS,PWMCON0_SYNC_DIS,PWMCON1_TRIPEN_DIS); //UCLK/2 to PWM, Enable IRQs
    _pwm_heat_pins_disable();
    _pwm_rhref_pins_disable();
    //The length of the PWM period is defined by PWMxLEN. Each pair has an associated counter. 
    //the PWMxCOMx MMRs controls the point at which the PWM output changes state
    //PWM 1 must have a duty cycle <=PWM0 
    
    return DEVICE_OK;
}

enum{
    enum_RHREF  = 0,
    enum_HEAT,
    enum_MAX,
};


static portuBASE_TYPE PWM_ParamGet(int channel, uint16 *plen, uint16 *pcom, int *ppair)
{
    portuBASE_TYPE  rt  = 0;
    
    if ((NULL == plen) || (NULL == pcom) 
        || (NULL == ppair) || (channel >= enum_MAX)){
        return -1;
    }
    
    if (enum_RHREF == channel){
        *plen                           = pADI_PWM->PWM0LEN;
        *pcom                           = pADI_PWM->PWM0COM1;
        *ppair                          = PWM0_1;
    }else {
        *plen                           = pADI_PWM->PWM1LEN;
        *pcom                           = pADI_PWM->PWM1COM1;
        *ppair                          = PWM2_3;
    }
}

unsigned char PWM_Freq(int channel, uint16 freq)
{
    int iPair;
    uint16 duty_cycle;
    uint16 old_freq;
    
    PWM_ParamGet(channel, &old_freq, &duty_cycle, &iPair);
    if (freq != old_freq){
        if (duty_cycle > freq){
            duty_cycle                  = freq;
        }
        pwm_output_disable(); 
        PwmTime(iPair, freq, duty_cycle, 0);
        pwm_output_enable();
    }
    
    return 0;
}

unsigned char PWM_DutyCycle(int channel, int duty_cycle) 
{ 
    int iPair;
    uint16 freq;
    uint16 old_duty_cycle;
    
    PWM_ParamGet(channel, &freq, &old_duty_cycle, &iPair);
    if (duty_cycle != old_duty_cycle){
        if (duty_cycle > freq){
            duty_cycle                  = freq;
        }
        PwmTime(iPair, freq, duty_cycle, 0);
        PwmLoad(PWMCON0_LCOMP_EN);
    }
    
    return 0;
} 

#if 0
unsigned char PWM_DutyCycle(int channel, int freq) 
{ 
    uint32  pwmlen;
    uint16  pwmh_high;

#if 0
    //uiPWMH_High: mul*10  
    if (uiPWMH_High > 1000) 
        return 1;       // Error 
#else 
    if (uiPWMH_High > freq){
        uiPWMH_High             = freq;
    }
#endif
    //Note that
    //•  Except for LCOMP, all other bits of PWMCON0 register can be changed 
    //only when PWMEN is low.
    //•  When LCOMP is written with Value 1, it stays at that value until 
    //the new value is loaded in the compare registers for all the channels.
    pwm_output_disable(); 
#if 0
    pwmlen  = static_cast<uint32>(8000000)/freq - 1;
    pwmh_high       = (uint16)((pwmlen*uiPWMH_High)/1000);
#else 
    pwmlen      = freq;
    pwmh_high   = uiPWMH_High;
#endif
    PwmTime(iPair, pwmlen, pwmh_high, 0);
    PwmLoad(PWMCON0_LCOMP_EN);
    pwm_output_enable(); 
    
    return 0;
} 
#endif

static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag){
    
    pwm_output_enable();
    _drv_ioctl(pdev, PWM_IOC_RHREF_FORCE_H, NULL);
    _drv_ioctl(pdev, PWM_IOC_HEAT_FORCE_H, NULL);
    
    return DEVICE_OK;
}

static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){
    
    return size;
}

static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
  
    return size; 
}

static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args)
{
	DeviceStatus_TYPE   rt = DEVICE_OK;
    portuBASE_TYPE pin_bit;
    int (*gpio_exec)(ADI_GPIO_TypeDef *pPort, int iVal);
    
    switch (cmd) {
        case PWM_IOC_RHREF_FORCE_H:
            
            pin_bit             = BIT(2);
            gpio_exec           = DioSet;
            pwm_rhref_pins_disable();
            break;
        case PWM_IOC_RHREF_FORCE_L:
            pin_bit             = BIT(2);
            gpio_exec           = DioClr;
            pwm_rhref_pins_disable();
            break;
        case PWM_IOC_HEAT_FORCE_H:
            pin_bit             = BIT(4);
            gpio_exec           = DioSet;
            pwm_heat_pins_disable();
            break;
        case PWM_IOC_HEAT_FORCE_L:
            pin_bit             = BIT(4);
            gpio_exec           = DioClr;
            pwm_heat_pins_disable();
            break;
        
        case PWM_IOC_RHREF_FREQ:
            pwm_rhref_pins_enable();
            PWM_Freq(enum_RHREF, static_cast<uint16>((uint32)args));
            return rt;
        
        case PWM_IOC_RHREF_DUTY_CYCLE:
            pwm_rhref_pins_enable();
            PWM_DutyCycle(enum_RHREF, static_cast<uint16>((uint32)args));
            return rt;
           
        case PWM_IOC_HEAT_FREQ:
            pwm_heat_pins_enable();
            PWM_Freq(enum_HEAT, static_cast<uint16>((uint32)args));
            return rt;
        
        case PWM_IOC_HEAT_DUTY_CYCLE:
            pwm_heat_pins_enable();
            PWM_DutyCycle(enum_HEAT, static_cast<uint16>((uint32)args));
            return rt;

        default:
            return DEVICE_ECMD_INVALID;
	}
    //pwm0 - p1.2   pwm1 - p1.4
    gpio_exec(pADI_GP1, pin_bit);

	return rt;
}


extern "C" void PWM0_Int_Handler()
{
    PwmLoad(PWMCON0_LCOMP_EN);
    PwmClrInt(PWMCLRI_PWM0);
}
extern "C" void PWM1_Int_Handler ()
{
    PwmLoad(PWMCON0_LCOMP_EN);
    PwmClrInt(PWMCLRI_PWM1);
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
