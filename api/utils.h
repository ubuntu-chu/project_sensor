#ifndef __API_UTILS_TOOL_H__
#define __API_UTILS_TOOL_H__

#include "../includes/includes.h"


#define 						_NET_SKBUF_CNTS					 (5)
#define 						_NET_SKBUF_LEN					 (130)

struct net_recv_skbuff{
	uint8 			m_buffer[_NET_SKBUF_LEN];
	uint16 			m_framelen;
};
typedef struct net_recv_skbuff  net_recv_skbuff_t;

struct skbuf_queue {

	net_recv_skbuff_t 	m_net_recv_skbuff[_NET_SKBUF_CNTS];
	uint8			 	front;
	uint8 			 	rear;
	uint8 			 	size;
    uint8               capacity;
};
typedef struct skbuf_queue   skbuf_queue_t;

void _delay_ms(uint16_t num);
void _delay_us(uint16_t num);
void buf_dump_hex(const char *buf, uint16 len);


void net_queue_init(void);
int8 net_queue_put(uint8 *pbuf, uint16 len);
int8 net_queue_get(uint8 *pbuf, uint16 *len);
int8 skbuf_queue_pick(skbuf_queue_t* lq, uint8 *pbuf, uint16 *len);
int8 skbuf_queue_pop(skbuf_queue_t* lq);

void bzero(void *s, size_t len);
char *reverse(char *str);

//--------------------------------------------------------------------

typedef char 							monitor_handle_type;

typedef void (pf_void)(void *);

enum {
	enum_MODE_ONESHOT	= 0,
	enum_MODE_PERIODIC,
};

class monitor_manage: noncopyable{

public:
	monitor_manage():m_precision(1), m_size(def_TIME_MONITOR_NR), m_bitmap(0){}
	~monitor_manage(){}

	//return handle for monitor
	monitor_handle_type monitor_register(uint16 expired_time, uint8 mode, pf_void *func, void *data);
	bool monitor_unregister(monitor_handle_type handle);

	void precision_set(uint16 precision)
	{
		m_precision 						= precision;
	}

	bool monitor_start(monitor_handle_type handle)
	{
		if (handle >= m_size){
			return false;
		}
		m_monitor[handle].m_start 	= 1;
        return true;
	}
	bool monitor_stop(monitor_handle_type handle)
	{
		if (handle >= m_size){
			return false;
		}
		m_monitor[handle].m_start 	= 0;
        return true;
	}

	bool monitor_expired_time_set(monitor_handle_type handle, uint16 expired_time)
	{
		if (handle >= m_size){
			return false;
		}
		if (m_monitor[handle].m_start == 0){
            //round up -- m_precision
			m_monitor[handle].m_expired_time 	= (expired_time+m_precision-1)/m_precision;
			m_monitor[handle].m_cnts 			= 0;
		}
        return true;
	}

	portBASE_TYPE monitor_expired(monitor_handle_type handle)
	{
		if (handle >= m_size){
			return (portBASE_TYPE)-1;
		}
		return 	m_monitor[handle].m_expired;
	}

	void run(void);

private:

class monitor_unit{

public:
	monitor_unit():m_start(0), m_cnts(0), m_expired_time(0), m_expired(0), m_mode(enum_MODE_ONESHOT){}
	~monitor_unit(){}

	volatile uint16 			m_cnts;
	volatile uint16 			m_expired_time;				//unit: ms
	volatile uint8 				m_start;
	volatile uint8 				m_mode;
	volatile uint8 				m_expired;
	//run in interrupt context, must fast and no sleep
	pf_void 						*m_pfunc;
	void 							*m_pdata;
};
	uint16						m_precision;                  //precision  unit: ms
	uint8 						m_size;
	uint8 						m_bitmap;
	monitor_unit 				m_monitor[def_TIME_MONITOR_NR];
};

extern monitor_manage 	t_monitor_manage;



#ifdef MINI_TIME_LIBRARY

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

struct tm *gmtime_r(const time_t *timep, struct tm *r);
struct tm* localtime_r(const time_t* t, struct tm* r);
struct tm* localtime(const time_t* t);
time_t mktime(struct tm * const t);
char *asctime_r(const struct tm *t, char *buf);
char *asctime(const struct tm *timeptr);
char *ctime(const time_t *timep);
int gettimeofday(struct timeval *tp, void *ignore);

#endif

#endif


