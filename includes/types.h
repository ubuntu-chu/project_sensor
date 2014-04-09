#ifndef    _TYPES_H_
#define    _TYPES_H_

//常规类型定义
typedef unsigned char uint8_t;
typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short int uint16_t;
typedef unsigned short int uint16;
typedef unsigned long int uint32_t;
typedef long signed int 		int32_t;
typedef unsigned long long int uint64_t;
typedef long long signed int    int64_t;
typedef unsigned long int uint32;
typedef long int 		int32;
typedef short signed int int16;
typedef unsigned char           bool_t;  	/* Boolean type */

//最适合cpu类型定义
typedef char portBASE_TYPE;
typedef unsigned char portuBASE_TYPE;
//中断寄存器类型定义
typedef unsigned char portCPSR_TYPE;
//时间类型定义
typedef unsigned long int time_t;



typedef uint16_t	os_period_t;
typedef uint32_t    sys_time_t;
typedef uint32_t    sys_tick_t;
typedef uint32_t	dev_adr_t;
typedef uint32_t	sen_adr_t;
typedef char        string_char;  	/* Signed    8 bit quantity  */



typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (SR = 16 bits)  */
typedef float                   fp32_t;
typedef double                  fp64_t;

//intptr_t ,uintptr_t : 表示当前平台下能够安全地对指针进行转型的整型变量
typedef unsigned short int      uintptr_t;
typedef short int               intptr_t;









#define WORD                uint16_t
#define DWORD               uint32_t
#define BYTE                uint8_t
#define BYTE_PTR            uint8_t*

typedef         void (*const FP_pfvoid)(void *);



#ifndef    CLOSE
    #define    CLOSE                                            ('0')
#endif

#ifndef    OPEN
    #define    OPEN                                             ('1')
#endif

#ifndef	   NULL
	#define    NULL                                             ((portuBASE_TYPE)0)
#endif

#ifndef	   TRUE
	#define    TRUE                                             ((portuBASE_TYPE)1)
#endif

#ifndef	   FALSE
	#define    FALSE                                            ((portuBASE_TYPE)0)
#endif

#ifndef ZERO
	#define ZERO                    							(portuBASE_TYPE)0
#endif

#ifndef SUCCESS
	#define SUCCESS                 							(portuBASE_TYPE)1
#endif

#ifndef FAILE
	#define FAILE                   							(portuBASE_TYPE)0
#endif

#define			DEV_ONLINE										((portuBASE_TYPE)1)
#define			DEV_OFFLINE										((portuBASE_TYPE)0)


class noncopyable
{
   protected:
      noncopyable() {}
      ~noncopyable() {}
   private: // emphasize the following members are private
      noncopyable( const noncopyable& );
      const noncopyable& operator=( const noncopyable& );
};

class copyable
{
   public:
      copyable() {}
      ~copyable() {}
      copyable( const copyable& );
      const copyable& operator=( const copyable& );
};

template<typename To, typename From>
inline To implicit_cast(From const &f) {
  return f;
}

template<typename To, typename From>     // use like this: down_cast<T*>(foo);
inline To down_cast(From* f) {                   // so we only accept pointers
  // Ensures that To is a sub-type of From *.  This test is here only
  // for compile-time type checking, and has no overhead in an
  // optimized build at run-time, as it will be optimized away
  // completely.
  if (false) {
    implicit_cast<From*, To>(0);
  }
  return static_cast<To>(f);
}

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
