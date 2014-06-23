/******************************************************************************
 * �ļ���Ϣ : 
 * 
 * �� �� �� :  
 *
 * �������� : 
 * 
 * ԭʼ�汾 :  
 *     
 * �޸İ汾 :   
 *    
 * �޸����� :  
 *
 * �޸����� : 
 * 
 * �� �� �� : 
 *
 * ��    ע : 
 *
 * ��    �� :   Դ����
 *
 * ��    Ȩ :
 * 
******************************************************************************/

/******************************************************************************
 *                              ͷ�ļ���ʿ
******************************************************************************/ 
 
#ifndef _DRV_AD_H
#define _DRV_AD_H

/******************************************************************************
 *                             �����ļ�����
******************************************************************************/

#ifndef    _INCLUDES_H_
	#include    "../../../includes/includes.h"
#endif

/******************************************************************************
 *                           �ļ��ӿ���Ϣ�궨��
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
 *                            �ļ��ӿں�������
******************************************************************************/ 

extern portuBASE_TYPE drv_adregister(void);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif
