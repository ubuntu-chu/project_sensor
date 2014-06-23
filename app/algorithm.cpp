#include    "algorithm.h"
#include    <math.h>


#ifndef CALC_RTD_TEMP_FORMULA

//RTD constants
#define TMIN (-40)                  // = minimum temperature in degC
#define TMAX (125)                  // = maximum temperature in degC
#define RMIN (842.707)              // = input resistance in ohms at -40 degC
#define RMAX (1479.51)              // = input resistance in ohms at 125 degC
#define RREF (1000)                 // = reference resistance initial value
#define NSEG 30                     // = number of sections in table
#define RSEG 21.2269                // = (RMAX-RMIN)/NSEG = resistance  in ohms of each segment
//RTD lookup table
const float C_rtd[] = {-40.0006,-34.6322,-29.2542,-23.8669,-18.4704,-13.0649,-7.65042,-2.22714,3.20489,8.64565,14.0952,
                        19.5536,25.0208,30.497,35.9821,41.4762,46.9794,52.4917,58.0131,63.5436,69.0834,74.6325,80.1909,
                        85.7587,91.3359,96.9225,102.519,108.124,113.74,119.365,124.999};
float CalculateRTDTemp(float r)
{
   float t;

   int j;
   j=(int)((r-RMIN)/RSEG);       // determine which coefficients to use
   if (j<0)               // if input is under-range..
      j=0;                // ..then use lowest coefficients
   else if (j>NSEG-1)     // if input is over-range..
      j=NSEG-1;            // ..then use highest coefficients
   t = C_rtd[j]+(r-(RMIN+RSEG*j))*(C_rtd[j+1]-C_rtd[j])/RSEG;

   return t;
}
#else
float CalculateRTDTemp(float r)
{
    float t;
    if(r>1000.0)
        t = r*(0.0000102718*r+0.23516512)-245.431;
    else
        t = r*(0.0000101006*r+0.23569455)-245.795;
   return t;
}
#endif


#ifndef CALC_PRESS_FORMULA

float CalculatePRSVal(float fADC,unsigned short usaADC[],unsigned short usaPRS[])
{
    unsigned char i;
    float fPrs;
    for(i=1;i<4;i++)
	{
        if(fADC < usaADC[i])
		{
linear_map:
		    fPrs = usaPRS[i-1] + (fADC-usaADC[i-1])*(usaPRS[i]-usaPRS[i-1])/(usaADC[i]-usaADC[i-1]);
			break;
		}
		else
		{
			    if(i==3) goto linear_map;
		}
	}
    return fPrs;
}

float CalculateDensity(float* fPRS20,float fPressure,float fTemprature)
{
    float coA,coB,coC;
#if  0
       fPressure  = 3594;
	   fTemprature= 10;
#endif
	coA = 0.0012655*fTemprature - 0.33733;
    coB = 0.56676*fTemprature + 152.64;
    coC = -0.036515*fTemprature-(fPressure-6.6876);//

	coC = sqrt(coB*coB-4*coA*coC);

    //density g/cm3 --2 power
    coC = (coC-coB)/(2*coA);
    //pressure20
    *fPRS20 = (coC*(163.98-0.312*coC)+6.457 );
    return coC;
}
#endif

