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
typedef unsigned long int 						uint64;
#if 0
typedef unsigned long int 						uint64_t;
typedef long signed int   					 	int64_t;
#endif
typedef long signed int    						int64;
typedef float                   					fp32_t;
typedef double                  					fp64_t;

typedef unsigned int           					bool_t;  	/* Boolean type */

//���ʺ�cpu���Ͷ���
typedef int 										portBASE_TYPE;
typedef unsigned int 								portuBASE_TYPE;
//�жϼĴ������Ͷ���
typedef unsigned int 								portCPSR_TYPE;
typedef uint16                     		        portSIZE_TYPE;
typedef int16                     		            portSSIZE_TYPE;
typedef portSIZE_TYPE                              portOFFSET_TYPE;
typedef portBASE_TYPE                              sv_err_t;
typedef portCPSR_TYPE								sv_base_t;

/* error code definitions */
#define SV_EOK                          0               /**< There is no error */
#define SV_ERROR                        1               /**< A generic error happens */
#define SV_ETIMEOUT                     2               /**< Timed out */
#define SV_EFULL                        3               /**< The resource is full */
#define SV_EEMPTY                       4               /**< The resource is empty */
#define SV_ENOMEM                       5               /**< No memory */
#define SV_ENOSYS                       6               /**< No system */
#define SV_EBUSY                        7               /**< Busy */
#define SV_EIO                          8               /**< IO error */

//ʱ�����Ͷ���
typedef unsigned long int 						time_t;
typedef uint32    				                    tick_t;
//typedef int 										size_t;
/* maximum value of base type */
//#define UINT8_MAX                    				0xff            /**< Maxium number of UINT8 */
//#define UINT16_MAX                   				0xffff          /**< Maxium number of UINT16 */
//#define UINT32_MAX                   				0xffffffff      /**< Maxium number of UINT32 */
#define TICK_MAX                     				((tick_t)-1)   	/**< Maxium number of tick */



//intptr_t ,uintptr_t : ��ʾ��ǰƽ̨���ܹ���ȫ�ض�ָ�����ת�͵����ͱ���
typedef unsigned int      						uintptr_t;
typedef int               							intptr_t;

//application type define
typedef uint32_t									dev_adr_t;
typedef uint32_t									sen_adr_t;
typedef char        								string_char;  	/* Signed    8 bit quantity  */


//sig atomic  ���ѱ�������Ϊ�����ͻᱣ֤�ñ�����ʹ�û�ֵʱ, ��������32λ����64λ�Ļ����϶��ܱ�֤������ԭ�ӵ�, ������ݻ����������Զ���Ӧ
typedef uint32_t									sig_atomic;

#define WORD                						uint16_t
#define DWORD               						uint32_t
#define BYTE                						uint8_t
#define BYTE_PTR            						uint8_t*

typedef char 										timer_handle_type;
typedef char 										event_handle_type;
typedef void (fp_void_pvoid)(void *);



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
	#define    true                                ((portuBASE_TYPE)1)
#endif

#ifndef	   FALSE
	#define    FALSE                               ((portuBASE_TYPE)0)
	#define    false                               ((portuBASE_TYPE)0)
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

/*
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */
struct timeval {
	long	tv_sec;		/* seconds */
	long	tv_usec;	/* and microseconds */
};

/*
 * Structure defined by POSIX.1b to be like a timeval.
 */
struct timespec {
	time_t	tv_sec;		/* seconds */
	long	tv_nsec;	/* and nanoseconds */
};

struct timezone {
  int tz_minuteswest;	/* minutes west of Greenwich */
  int tz_dsttime;	/* type of dst correction */
};

struct tm {
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year - 1900. */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/

  long int tm_gmtoff;		/* Seconds east of UTC.  */
  const char *tm_zone;		/* Timezone abbreviation.  */
};


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

//����ģ����
template <class T>
class singleton
{
public:
     static inline T* instance(void)
     {
    	 static T t;
    	 return &t;
     }
protected:
     singleton(void);
     ~singleton(void);
private:
     singleton(const singleton&);
     singleton & operator= (const singleton &);
};

//���嵥��ģ����Ϊ��Ҫ���������Ԫ��  ������instance�����з�����Ĺ������������
#define DEFINE_SINGLETON_CLASS(type)        friend singleton<type>;

#endif




#if 0

//Tobject:���ö�������ͣ�Tparam�ص���������������
template<typename Tobject, typename Tparam>
class CCallbackProxy
{
	typedef void (Tobject::*CbFun)(Tparam*);
public:
	CCallbackProxy(Tobject *pInstance, CbFun pFun)
    {
        m_pInstance = pInstance; 
        pCbFun = pFun;
    }
    void operator()(Tparam* pParam)
    {
        (m_pInstance->*pCbFun)(pParam);
    }

private:	
	CbFun		pCbFun;		//�ص�����ָ��
	Tobject*	m_pInstance;	//���ö���
};

#if 0
//���õ��ö�����ص�����
template<typename Tobject, typename Tparam>
void CCallbackProxy<Tobject, Tparam>::Set(Tobject *pInstance , CbFun pFun)
{
	m_pInstance = pInstance; 
	pCbFun = pFun;
};

//���ûص�����
template<typename Tobject, typename Tparam>
bool CCallbackProxy<Tobject, Tparam>::Exec(Tparam* pParam)
{
	(m_pInstance->*pCbFun)(pParam);
	return true;
}
#endif

#endif

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
