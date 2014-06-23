#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include "../includes/includes.h"



float CalculateRTDTemp(float r);
float CalculatePRSVal(float fADC,unsigned short usaADC[],unsigned short usaPRS[]);
float CalculateDensity(float* fPRS20,float fPressure,float fTemprature);













#endif
