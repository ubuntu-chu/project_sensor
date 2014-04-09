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
#define         SBI32(reg, bit)                                 ((reg) |= (uint32)((uint32)1 << bit))
#define         CBI32(reg, bit)                                 ((reg) &= (uint32)(~((uint32)1 << bit)))
#define         XBI32(reg, bit)                                 ((reg) ^= (uint32)((uint32)1 << bit))
#define         BIT_IS_SET32(reg, bit)                          (((reg & ((uint32)((uint32)1<<bit))) == ((uint32)((uint32)1<<bit)))? (1): (0))
#define         BIT_IS_CLEAR32(reg, bit)                        (((reg & ((uint32)((uint32)1<<bit))) == ((uint32)((uint32)1<<bit)))? (0): (1))

//-----------------------------------------------------------------------------
#define         SBI16(reg, bit)                                 ((reg) |= (uint16)((uint16)1 << bit))
#define         CBI16(reg, bit)                                 ((reg) &= (uint16)(~((uint16)1 << bit)))
#define         XBI16(reg, bit)                                 ((reg) ^= (uint16)((uint16)1 << bit))
#define         BIT_IS_SET16(reg, bit)                          (((reg & ((uint16)((uint16)1<<bit))) == ((uint16)((uint16)1<<bit)))? (1): (0))
#define         BIT_IS_CLEAR16(reg, bit)                        (((reg & ((uint16)((uint16)1<<bit))) == ((uint16)((uint16)1<<bit)))? (0): (1))

//------------------------------------------------------------------------------
#define         SBI8(reg, bit)                                 ((reg) |= (uint8)((uint8)1 << bit))
#define         CBI8(reg, bit)                                 ((reg) &= (uint8)(~((uint8)1 << bit)))
#define         XBI8(reg, bit)                                 ((reg) ^= (uint8)((uint8)1 << bit))
#define         BIT_IS_SET8(reg, bit)                          (((reg & ((uint8)((uint8)1<<bit))) == ((uint8)((uint8)1<<bit)))? (1): (0))
#define         BIT_IS_CLEAR8(reg, bit)                        (((reg & ((uint8)((uint8)1<<bit))) == ((uint8)((uint8)1<<bit)))? (0): (1))

//------------------------------------------------------------------------------
#define         XBYTE(addr)                                     (*(volatile unsigned char *)addr)
#define         XWORD(addr)                                     (*(volatile short unsigned int *)addr)
#define         XDWORD(addr)                                    (*(volatile long unsigned int *)addr)

//------------------------------------------------------------------------------
#define         MIN(a,b)                                        ((a)<(b)?(a):(b))
#define         MAX(a,b)                                        ((a)<(b)?(b):(a))
#define         C2N(char)                                       (char - '0')
#define         N2C(numb)                                       (numb + '0')

//------------------------------------------------------------------------------
#define	        GET_ARRAY_COUNT(addr)					        (sizeof(addr)/sizeof(addr[0]))
#define         OFFSET(Struct, Field) 				            ((unsigned int)(unsigned int*)&(((Struct *)0)->Field))
#define 		ARRAY_SIZE(arr) 								(sizeof(arr) / sizeof((arr)[0]))

//------------------------------------------------------------------------------

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
