/******************************************************************************
 *                          锟斤拷锟侥硷拷锟斤拷锟斤拷锟矫碉拷头锟侥硷拷
******************************************************************************/ 
#include "ad.h"
#include <math.h>


float ADC1_GetADC_Temp(float value);
float ADC1_GetADC_Pres(float value);
float ADC1_GetADC_RHS(float value);

float fADCTemp[def_RECORD_LEN];
float fADCPres[def_RECORD_LEN];
float fADCRHS[def_RECORD_LEN];

device_ad::device_ad():device(DEVICE_NAME_AD)
{

}

device_ad::~device_ad()
{
}

//ADC转换后最大的值  2的28次幂
#define 		_def_ADC_MAX_VALUE 						(268435456UL)
#define 		_def_TEMP_VOLT							(37400UL)
#define 		_def_RHS_VOLT							(1200UL)

enum PROC_CMD_STAT device_ad::process_command(enum PROC_CMD dir, enum PROC_PHASE phase, struct device_buffer &device_buffer)
{
	if (CMD_READ == dir){
		switch (phase){
		case PHASE_PREPARE:

#ifndef		def_AVARAGE_IN_READ
			if (m_ioc == AD_IOC_FAKE_READABLE){
                break;
            }
            device_buffer.m_pbuf_recv						= reinterpret_cast<char *>(m_buffer);
			device_buffer.m_buf_recv_size 					= sizeof(m_buffer);
#endif
			break;

		case PHASE_DONE:
		{
			//将ADC值换算成电压值
			float fValue;
			float fAdc;

#ifdef		def_AVARAGE_IN_READ
			fAdc											= *((float *)device_buffer.m_pbuf_recv);
#else
			fAdc											= GetAvarageValue(m_buf, def_AD_SAMPLE_LEN);
#endif
			if (m_ioc == AD_IOC_FAKE_READABLE){
                break;
            }
            if (m_ioc == AD_IOC_START_RTD_ADC){
#ifndef def_ADC_SOFT_FILTER
				fValue 		= ADC1_GetADC(fAdc)*_def_TEMP_VOLT/_def_ADC_MAX_VALUE;
#else
				fValue  	= ADC1_GetADC_Temp(fAdc)*_def_TEMP_VOLT/_def_ADC_MAX_VALUE;
#endif
			}else if (m_ioc == AD_IOC_START_RHS_ADC){
#ifndef def_ADC_SOFT_FILTER
				fValue		= ADC1_GetADC(fAdc) * _def_RHS_VOLT / _def_ADC_MAX_VALUE;
#else
				fValue		= ADC1_GetADC_RHS(fAdc) * _def_RHS_VOLT / _def_ADC_MAX_VALUE;
#endif
			}else if (m_ioc == AD_IOC_START_PRS_ADC){
#ifndef def_ADC_SOFT_FILTER
				fValue 		= ADC1_GetADC(fAdc)/256;
#else
				fValue 		= ADC1_GetADC_Pres(fAdc)/256;
#endif
			}			device_buffer.m_recv_actual_size 				= sizeof(fValue);
			memcpy(device_buffer.m_pbuf_recv, &fValue, sizeof(fValue));
		}
			break;

		default:
			break;
		}
	}else if (CMD_IOC == dir){
		m_ioc 											    = device_buffer.m_cmd;
	}else {
		device::process_command(dir, phase, device_buffer);
	}

	return PROC_CMD_STAT_OK;
}




#ifndef     def_ADC_SOFT_FILTER
float ADC1_GetADC(float value)
{
    return value;
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
	   for(i=def_RECORD_LEN-1;i>=0;i--)
	      fAry[i] = fADC;
	}

	max = min = sum = fAry[0] = fADC;
	for(i=def_RECORD_LEN-1;i!=0;i--)
	{
	    if(fAry[i]>max) max = fAry[i];
		else if(fAry[i]<min) min = fAry[i];
		sum += fAry[i];
		fAry[i] = fAry[i-1];
	}
	i = def_RECORD_LEN-2;
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
signed char cRLPFRatioChangeNum;

float ADC1_GetADC_RHS(float value)
{
    float fNewRHS;
    float fPreRHSADCVal;
    fNewRHS = CombiFilter(value, fADCRHS);
    fPreRHSADCVal = RapidLPFilter(fNewRHS, fPreRHSADCVal, &cRLPFRatioChangeNum);
    
    return fPreRHSADCVal;
}

//GetFltRsutTemp
float ADC1_GetADC_Temp(float value)
{
    return CombiFilter(value,fADCTemp);
}

//GetFltRsutTemp
float ADC1_GetADC_Pres(float value)
{
    return CombiFilter(value,fADCPres);
}

#endif

