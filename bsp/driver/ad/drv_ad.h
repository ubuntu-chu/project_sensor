/******************************************************************************
 * 文件信息 : 
 * 
 * 创 建 者 :  
 *
 * 创建日期 : 
 * 
 * 原始版本 :  
 *     
 * 修改版本 :   
 *    
 * 修改日期 :  
 *
 * 修改内容 : 
 * 
 * 审 核 者 : 
 *
 * 附    注 : 
 *
 * 描    述 :   源代码
 *
 * 版    权 :
 * 
******************************************************************************/

/******************************************************************************
 *                              头文件卫士
******************************************************************************/ 
 
#ifndef _DRV_AD_H
#define _DRV_AD_H

/******************************************************************************
 *                             包含文件声明
******************************************************************************/

#ifndef    _INCLUDES_H_
	#include    "../../../includes/includes.h"
#endif

/******************************************************************************
 *                           文件接口信息宏定义
******************************************************************************/

#define         DEVICE_NAME_AD                                  ("AD")


//excitation  current  source
enum{
    AD_IOC_START_RTD_ADC     = (DEVICE_IOC_USER+1),
    AD_IOC_START_RHS_ADC,
    AD_IOC_START_PRS_ADC,
    AD_IOC_GET_SGL_RTD_ADC,
    AD_IOC_GET_SGL_RHS_ADC,
    AD_IOC_FAKE_READABLE,
    AD_IOC_MAX_NO,
};

#define GPIO_TMeasOn()  DioSet(pADI_GP0,1)
#define GPIO_TMeasOff() DioClr(pADI_GP0,1)
#define GPIO_TMeasSta() (DioRd(pADI_GP0)&0x01)

//extern volatile unsigned char ucADC0NewData;// New data flag, set in ADC0 int.
extern volatile unsigned char ucADC1NewData;// New data flag, set in ADC1 int.
//extern volatile unsigned char ucADC0RunStus;
extern volatile unsigned char ucADC1RunStus;


void AFE_Init(void);

void ADC0_Init(void);
int ADC0_GetSglADC_RTD(void);
int ADC0_GetSglADC_RHS(void);
//void ADC0_StartRHD_ADC(void);
//float ADC0_GetADC(void);
//float ADC0_GetADC_RHS(void);
//float ADC0_GetADC_RHR(void);
//float ADC0_GetADC_RHD(void);

void ADC1_Init(void);
void ADC1_StartRTD_ADC(void);
void ADC1_StartPRS_ADC(void);
void ADC1_StartRHS_ADC(void);
float ADC1_GetADC(void);
float ADC1_GetADC_Temp(void);
float ADC1_GetADC_Pres(void);
float ADC1_GetADC_RHS(void);








/******************************************************************************
 *                            文件接口函数声明
******************************************************************************/ 

extern portuBASE_TYPE drv_adregister(void);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif
