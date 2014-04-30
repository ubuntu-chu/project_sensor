#ifndef    _MACRO_H_
#define    _MACRO_H_

#include    "types.h"
#include    "config.h"

#if BIG_ENDIAN == 0

#define LD_WORD(ptr)        (WORD)(((WORD)*(BYTE_PTR)((ptr)+1)<<8)|(WORD)*(BYTE_PTR)(ptr))

#define LD_DWORD(ptr)       (DWORD)(((DWORD)*(BYTE_PTR)((ptr)+3)<<24)   |   \
                            ((DWORD)*(BYTE_PTR)((ptr)+2)<<16)           |   \
                            ((WORD)*(BYTE_PTR)((ptr)+1)<<8)             |   \
                            *(BYTE_PTR)(ptr))

#define ST_WORD(ptr,val)    *(BYTE_PTR)(ptr)=(BYTE)(val);                   \
                            *(BYTE_PTR)((ptr)+1)=(BYTE)((WORD)(val)>>8)

#define ST_DWORD(ptr,val)   *(BYTE_PTR)(ptr)=(BYTE)(val);                   \
                            *(BYTE_PTR)((ptr)+1)=(BYTE)((WORD)(val)>>8);    \
                            *(BYTE_PTR)((ptr)+2)=(BYTE)((DWORD)(val)>>16);  \
                            *(BYTE_PTR)((ptr)+3)=(BYTE)((DWORD)(val)>>24)
#else
#define LD_WORD(ptr)        (WORD)(((WORD)*(BYTE_PTR)((ptr)+0)<<8)|(WORD)*(BYTE_PTR)((ptr)+1))

#define LD_DWORD(ptr)       (DWORD)(((DWORD)*(BYTE_PTR)((ptr)+0)<<24)   |   \
                            ((DWORD)*(BYTE_PTR)((ptr)+1)<<16)           |   \
                            ((WORD)*(BYTE_PTR)((ptr)+2)<<8)             |   \
                            *(BYTE_PTR)((ptr)+3))

#define ST_WORD(ptr,val)    *(BYTE_PTR)((ptr)+0)=(BYTE)((WORD)(val)>>8);    \
                            *(BYTE_PTR)((ptr)+1)=(BYTE)(val)

#define ST_DWORD(ptr,val)   *(BYTE_PTR)((ptr)+0)=(BYTE)((DWORD)(val)>>24);  \
                            *(BYTE_PTR)((ptr)+1)=(BYTE)((DWORD)(val)>>16);  \
                            *(BYTE_PTR)((ptr)+2)=(BYTE)((WORD)(val)>>8);    \
                            *(BYTE_PTR)((ptr)+3)=(BYTE)(val)


#endif

//------------------------------------------------------------------------------
#define   		 BIT(n)											 (1UL << n)
#define         SBI(reg, bit)                                 	 ((reg) |= BIT(bit))
#define         CBI(reg, bit)                                 	 ((reg) &= ~BIT(bit))
#define         XBI(reg, bit)                                 	 ((reg) ^= BIT(bit))
#define         BIT_IS_SET(reg, bit)                             (!!((reg) & BIT(bit)))
#define         BIT_IS_CLEAR(reg, bit)                           (!BIT_IS_SET(reg, bit))

//------------------------------------------------------------------------------
#define         XBYTE(addr)                                     (*(volatile uint8 *)addr)
#define         XWORD(addr)                                     (*(volatile uint16 *)addr)
#define         XDWORD(addr)                                    (*(volatile uint32 *)addr)

//------------------------------------------------------------------------------
#define         MIN(a,b)                                        ((a)<(b)?(a):(b))
#define         MAX(a,b)                                        ((a)<(b)?(b):(a))
#define         C2N(c)                                       	 (c - '0')
#define         N2C(n)                                          (n + '0')

//------------------------------------------------------------------------------
#define         OFFSET(Struct, Field) 				            ((unsigned int)(unsigned int*)&(((Struct *)0)->Field))
#define 		ARRAY_SIZE(arr) 								(sizeof(arr) / sizeof((arr)[0]))

//------------------------------------------------------------------------------

#if 	0
#define   		 BIT0 						   					(1UL << 0)
#define   		 BIT1 						   					(1UL << 1)
#define   		 BIT2 						   					(1UL << 2)
#define   		 BIT3 						   					(1UL << 3)
#define   		 BIT4 						   					(1UL << 4)
#define   		 BIT5 						   					(1UL << 5)
#define   		 BIT6 						   					(1UL << 6)
#define   		 BIT7 						   					(1UL << 7)
#endif
#if 	1
#define   		 BIT8 						   					(1UL << 8)
#define   		 BIT9 						   					(1UL << 9)
#define   		 BIT10 						   					(1UL << 10)
#define   		 BIT11 						   					(1UL << 11)
#define   		 BIT12 						   					(1UL << 12)
#define   		 BIT13 						   					(1UL << 13)
#define   		 BIT14 						   					(1UL << 14)
#define   		 BIT15 						   					(1UL << 15)
#endif
#if 	1
#define   		 BIT16 						   					(1UL << 16)
#define   		 BIT17 						   					(1UL << 17)
#define   		 BIT18 						   					(1UL << 18)
#define   		 BIT19 						   					(1UL << 19)
#define   		 BIT20 						   					(1UL << 20)
#define   		 BIT21 						   					(1UL << 21)
#define   		 BIT22 						   					(1UL << 22)
#define   		 BIT23 						   					(1UL << 23)
#endif
#if 	1
#define   		 BIT24 						   					(1UL << 24)
#define   		 BIT25 						   					(1UL << 25)
#define   		 BIT26 						   					(1UL << 26)
#define   		 BIT27 						   					(1UL << 27)
#define   		 BIT28 						   					(1UL << 28)
#define   		 BIT29 						   					(1UL << 29)
#define   		 BIT30 						   					(1UL << 30)
#define   		 BIT31 						   					(1UL << 31)
#endif

#if 0
#define ASSERT(EX)                                                         \
if (!(EX))                                                                    \
{                                                                             \
    volatile char dummy = 0;                                                  \
    printf("(%s) assert failed at %s:%d \n", #EX, __FUNCTION__, __LINE__);\
    while (dummy == 0);                                                       \
}
#endif

#define ASSERT(EX)                                                         \
if (!(EX))                                                                    \
{                                                                             \
    volatile char dummy = 0;                                                  \
    while (dummy == 0);                                                       \
}








/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
