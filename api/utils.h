#ifndef __API_UTILS_TOOL_H__
#define __API_UTILS_TOOL_H__

#include "../includes/includes.h"


#ifdef		CIRCULAR_BUFFER

template<int CAPACITY>
class circular_buffer: copyable
{
public:
	circular_buffer();
	~circular_buffer();

	size_t size() const {
		return size_;
	}
	size_t capacity() const {
		return CAPACITY;
	}
	size_t write(const char *data, size_t bytes);
	size_t read(char *data, size_t bytes);

private:
	size_t beg_index_, end_index_, size_;
	char data_[CAPACITY];
};

#endif

void _delay_ms(uint16_t num);
void _delay_us(uint16_t num);
void buf_dump_hex(const char *buf, uint16 len);

#ifdef		BUF_QUEUE

#define		QUEUE_BUF_CNTS					 	(3)
#define		QUEUE_BUF_LEN					 	(130)

struct rx_buf{
	int8 			    m_buf[QUEUE_BUF_LEN];
	uint16 			    m_len;
};
typedef struct rx_buf  rx_buf_t;

struct buf_queue {

	rx_buf_t 	        m_rx_buf[QUEUE_BUF_CNTS];
	uint8			 	front;
	uint8 			 	rear;
	uint8 			 	size;
    uint8               capacity;
};
typedef struct buf_queue   buf_queue_t;


void buf_queue_init(buf_queue_t* lq);
int8 buf_queue_put(buf_queue_t* lq, int8 *pbuf, uint16 len);
int8 buf_queue_get(buf_queue_t* lq, int8 *pbuf, uint16 *len);
int8 buf_queue_pick(buf_queue_t* lq, int8 *pbuf, uint16 *len);
int8 buf_queue_pop(buf_queue_t* lq);
#endif

#ifdef		SKBUF_QUEUE

void skbuf_queue_init(void);
int8 skbuf_queue_put(int8 *pbuf, uint16 len);
int8 skbuf_queue_get(int8 *pbuf, uint16 *len);
#endif

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
	monitor_manage():m_precision(1), 
                        m_size(MONITOR_TIME_NR), 
                        m_bitmap(0){}
	~monitor_manage(){}

	//return handle for monitor
	monitor_handle_type monitor_register(uint16 expired_time, 
                                        uint8 mode, 
                                        pf_void *func, 
                                        void *data,
                                        const char *pname);
	bool monitor_unregister(monitor_handle_type handle);

    //unit: ms
	void precision_set(uint16 precision)
	{
		m_precision 						= precision;
	}

	bool monitor_start(monitor_handle_type handle)
	{
		if (handle >= m_size){
			return false;
		}
		m_monitor[handle].m_start 	        = 1;
        m_monitor[handle].m_expired         = 0;
        return true;
	}
	bool monitor_stop(monitor_handle_type handle)
	{
		if (handle >= m_size){
			return false;
		}
		m_monitor[handle].m_start 	        = 0;
        m_monitor[handle].m_expired         = 0;
        return true;
	}
    
    bool monitor_cnt_clr(monitor_handle_type handle)
    {
        if (handle >= m_size){
			return false;
		}
        m_monitor[handle].m_cnts 			= 0;
        return true;
    }
    
	bool monitor_expired_time_set(monitor_handle_type handle, uint16 expired_time)
	{
		if (handle >= m_size){
			return false;
		}
		if (m_monitor[handle].m_start == 0){
            //round up -- m_precision
			m_monitor[handle].m_expired         = 0;
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
    
    portBASE_TYPE monitor_started(monitor_handle_type handle)
	{
		if (handle >= m_size){
			return (portBASE_TYPE)-1;
		}
		return 	m_monitor[handle].m_start;
	}

	void run(void);

private:

class monitor_unit{

public:
	monitor_unit():m_cnts(0), 
                    m_expired_time(0), 
                    m_start(0), 
                    m_mode(enum_MODE_ONESHOT), 
                    m_expired(0),
                    m_pfunc(NULL),
                    m_pdata(NULL),
                    m_pname(NULL){}
	~monitor_unit(){}

	volatile uint16 			m_cnts;
	volatile uint16 			m_expired_time;				//unit: ms
	volatile uint8 				m_start;
	volatile uint8 				m_mode;
	volatile uint8 				m_expired;
	//run in interrupt context, must fast and no sleep
	pf_void 				    *m_pfunc;
	void 						*m_pdata;
    const char                  *m_pname;
};
	uint16						m_precision;                  //precision  unit: ms
	uint8 						m_size;
	uint8 						m_bitmap;
	monitor_unit 				m_monitor[MONITOR_TIME_NR];
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

#ifdef		CRC16
uint16 crc16(const uint8 *buf, uint16 len);
#endif

#ifdef		SUM16
uint16 sum16(const uint8 * ptr, uint16 len)
#endif

#endif


