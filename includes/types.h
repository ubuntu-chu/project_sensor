#ifndef    _TYPES_H_
#define    _TYPES_H_

//base type define
typedef unsigned char 							uint8_t;
typedef unsigned char 							uint8;
typedef signed char 								int8_t;
typedef signed char 								int8;
typedef unsigned short int 						uint16_t;
typedef unsigned short int 						uint16;
typedef short signed int 							int16_t;
typedef short signed int 							int16;
typedef unsigned int 								uint32_t;
typedef unsigned int 								uint32;
typedef signed int 								int32_t;
typedef int 										int32;
#if 0
typedef unsigned long int 						uint64_t;
#endif
typedef unsigned long int 						uint64;
#if 0
typedef long signed int   					 	int64_t;
#endif
typedef long signed int    						int64;
typedef float                   					fp32_t;
typedef double                  					fp64_t;

typedef unsigned int           					bool_t;  	/* Boolean type */

//最适合cpu类型定义
typedef int 										portBASE_TYPE;
typedef unsigned int 								portuBASE_TYPE;
//中断寄存器类型定义
typedef unsigned int 								portCPSR_TYPE;
typedef uint16                     		        portSIZE_TYPE;
typedef int16                     		            portSSIZE_TYPE;
typedef portSIZE_TYPE                              portOFFSET_TYPE;
typedef portBASE_TYPE                              sv_err_t;
#ifndef		LINUX_OS
//时间类型定义
typedef unsigned long int 						time_t;
//typedef int 										size_t;
#endif
//intptr_t ,uintptr_t : 表示当前平台下能够安全地对指针进行转型的整型变量
typedef unsigned int      						uintptr_t;
typedef int               							intptr_t;

//application type define
typedef uint16_t									os_period_t;
typedef uint32_t   									sys_time_t;
typedef uint32_t    								sys_tick_t;
typedef uint32_t									dev_adr_t;
typedef uint32_t									sen_adr_t;
typedef char        								string_char;  	/* Signed    8 bit quantity  */


#define WORD                						uint16_t
#define DWORD               						uint32_t
#define BYTE                						uint8_t
#define BYTE_PTR            						uint8_t*


typedef         void (fp_void_pvoid)(void *);



#ifndef    CLOSE
    #define    CLOSE                               ('0')
#endif

#ifndef    OPEN
    #define    OPEN                                ('1')
#endif

#ifndef	   NULL
	#define    NULL                                ((portuBASE_TYPE)0)
#endif

#ifndef	   TRUE
	#define    TRUE                                ((portuBASE_TYPE)1)
#endif

#ifndef	   FALSE
	#define    FALSE                               ((portuBASE_TYPE)0)
#endif

#ifndef ZERO
	#define ZERO            						((portuBASE_TYPE)0)
#endif

#ifndef SUCCESS
	#define SUCCESS         						((portuBASE_TYPE)1)
#endif

#ifndef FAILE
	#define FAILE           						((portuBASE_TYPE)0)
#endif

#ifndef DEV_ONLINE
	#define DEV_ONLINE								((portuBASE_TYPE)1)
#endif

#ifndef DEV_OFFLINE
	#define DEV_OFFLINE 							((portuBASE_TYPE)0)
#endif

#ifdef __cplusplus 

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
#endif

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
