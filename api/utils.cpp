#include "utils.h"


monitor_handle_type monitor_manage::monitor_register(uint16 expired_time, uint8 mode, pf_void *func, void *data)
{
	monitor_handle_type handle;
	portuBASE_TYPE 		i;

	for (i = 0; i < m_size; ++i){
		if (!(m_bitmap & (1UL << i))){
			break;
		}
	}
	if (i >= m_size){
		return (monitor_handle_type)-1;
	}
	handle 									= i;
	m_bitmap 								|= (1UL << i);
	m_monitor[handle].m_expired 			= 0;
	m_monitor[handle].m_start 				= 0;
	m_monitor[handle].m_mode 				= mode;
	monitor_expired_time_set(handle, expired_time);
	m_monitor[handle].m_pfunc 				= func;
	m_monitor[handle].m_pdata 				= data;
    
    return handle;
}

bool monitor_manage::monitor_unregister(monitor_handle_type handle)
{
	if (handle >= m_size){
		return false;
	}
	m_bitmap 								&= ~(1UL << handle);
	m_monitor[handle].m_start 				= 0;
    
    return true;
}

void monitor_manage::run(void)
{
	portuBASE_TYPE 		i;

	for (i = 0; i < m_size; ++i){
		if (!(m_bitmap & (1UL << i))){
			continue;
		}
		if (m_monitor[i].m_start){
			m_monitor[i].m_cnts++;
			if (m_monitor[i].m_cnts >= m_monitor[i].m_expired_time){
				m_monitor[i].m_cnts 			= 0;
				if (m_monitor[i].m_mode == enum_MODE_ONESHOT){
					m_monitor[i].m_expired 			= 1;
					m_monitor[i].m_start 			= 0;
				}
				if (NULL != m_monitor[i].m_pfunc){
					m_monitor[i].m_pfunc(m_monitor[i].m_pdata);
				}
			}
		}
	}
}

monitor_manage 	t_monitor_manage;

char *utils_itoa(int num, char *str, int radix) {
	/* 索引表 */
	char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned unum; /* 中间变量 */
	 int i=0,j,k;

	 /* 确定unum的值 */
	if (radix == 10 && num < 0) /* 十进制负数 */
	{
		unum = (unsigned) -num;
		str[i++] = '-';
	} else
		unum = (unsigned) num; /* 其他情况 */
	/* 逆序 */
	do {
		str[i++]=index[unum%(unsigned)radix];
		unum/=radix;
	}while (unum)
		;
	str[i] = '\0';
	/* 转换 */
	if (str[0] == '-')
		k = 1; /* 十进制负数 */
	else
		k = 0;
	char temp;
	for (j = k; j <= (i - k - 1) / 2; j++) {
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - j - 1] = temp;
	}

	return str;
}

void buf_dump_hex(const char *buf, uint16 len)
{
	uint16 i;
	uint16 index;
	uint8 buff[] = "0x00\r\n";
	uint8 prefix[] = "buf[000] = ";
	char  chartable[] = "0123456789ABCDEF";

	for (i = 0; i < len; i++) {

		index = i;
		prefix[4] = index / 100 + 0x30;
		index %= 100;
		prefix[5] = index / 10 + 0x30;
		index %= 10;
		prefix[6] = index + 0x30;
		SYS_LOG_LEV_TINY_LEN(SYS_LOG_LEV_DEBUG, prefix, strlen((char const * )prefix));
		buff[2] = chartable[(buf[i] >> 4)];
		buff[3] = chartable[(buf[i] & 0x0f)];
		SYS_LOG_LEV_TINY_LEN(SYS_LOG_LEV_DEBUG, buff, strlen((char const * )buff));
	}
}

void bzero(void *s, size_t len)
{
    memset(s, 0, len);
}

char *reverse(char *str)   
{   
    if(!str ){   
        return NULL;
    }   

    portuBASE_TYPE  len     = strlen(str);
    char            *phead  = str;
    char            *ptail  = str + len -1;
    char            temp;  
    
    for(portuBASE_TYPE i = 0; i < len/2; ++i){    
        temp    = *phead;   
        *phead  = *ptail;   
        *ptail  = temp;   
    }   
    return str;   
}

void _delay_ms(uint16_t num)
{
	uint16_t i = num;
	for(;i > 0;i--){
		//__delay_cycles(def_MCLK/1000);
	}
}

void _delay_us(uint16_t num)
{
	uint16_t i = num;
	for(;i > 0;i--){
		//__delay_cycles(def_MCLK/1000000);
	}
}

void skbuf_queue_init(skbuf_queue_t* lq)
{

	lq->front               = lq->rear = 0;
	lq->size                = 0;
    lq->capacity            = _NET_SKBUF_CNTS;
}

int8 skbuf_queue_put(skbuf_queue_t* lq, uint8 *pbuf, uint16 len)
{
	net_recv_skbuff_t 	*pnet_recv_skbuf;

	if (lq->size >= lq->capacity){
		return -1;
	}
	pnet_recv_skbuf 		= &(lq->m_net_recv_skbuff[lq->rear]);
	if (len > _NET_SKBUF_LEN){
		len 					= _NET_SKBUF_LEN;
	}
	pnet_recv_skbuf->m_framelen	= len;
	if (NULL != pbuf){
		memcpy(pnet_recv_skbuf->m_buffer, pbuf, len);
	}
	lq->rear = (lq->rear + 1) % lq->capacity;
	lq->size++;

	return 0;
}

int8 skbuf_queue_get(skbuf_queue_t* lq, uint8 *pbuf, uint16 *len)
{
	net_recv_skbuff_t 	*pnet_recv_skbuf;
	portCPSR_TYPE	level 		= cpu_interruptDisable();

	if (lq->size <= 0){
		cpu_interruptEnable(level);
		return -1;
	}
	lq->size--;
	cpu_interruptEnable(level);
	pnet_recv_skbuf 		= &(lq->m_net_recv_skbuff[lq->front]);
	if (NULL != len){
		*len				= pnet_recv_skbuf->m_framelen;
	}
	if (NULL != pbuf){
		memcpy(pbuf, pnet_recv_skbuf->m_buffer, pnet_recv_skbuf->m_framelen);
	}
	lq->front = (lq->front + 1) % lq->capacity;

	return 0;
}

int8 skbuf_queue_pick(skbuf_queue_t* lq, uint8 *pbuf, uint16 *len)
{
	net_recv_skbuff_t 	*pnet_recv_skbuf;

	if (lq->size <= 0){
		return -1;
	}
	pnet_recv_skbuf 		= &(lq->m_net_recv_skbuff[lq->front]);
	if (NULL != len){
		*len				= pnet_recv_skbuf->m_framelen;
	}
	if (NULL != pbuf){
		memcpy(pbuf, pnet_recv_skbuf->m_buffer, pnet_recv_skbuf->m_framelen);
	}

	return 0;
}

int8 skbuf_queue_pop(skbuf_queue_t* lq)
{
	portCPSR_TYPE	level 		= cpu_interruptDisable();

	if (lq->size <= 0){
		cpu_interruptEnable(level);
		return -1;
	}
	lq->size--;
	lq->front = (lq->front + 1) % lq->capacity;
	cpu_interruptEnable(level);
    
    return 0;
}

skbuf_queue_t 	t_skbuf_queue;

void net_queue_init(void)
{
	skbuf_queue_init(&t_skbuf_queue);
}

int8 net_queue_put(uint8 *pbuf, uint16 len)
{
	return skbuf_queue_put(&t_skbuf_queue, pbuf, len);
}

int8 net_queue_get(uint8 *pbuf, uint16 *len)
{
	return skbuf_queue_get(&t_skbuf_queue, pbuf, len);
}

#ifdef MINI_TIME_LIBRARY

/* days per month -- nonleap! */
const short __spm[13] =
  { 0,
    (31),
    (31+28),
    (31+28+31),
    (31+28+31+30),
    (31+28+31+30+31),
    (31+28+31+30+31+30),
    (31+28+31+30+31+30+31),
    (31+28+31+30+31+30+31+31),
    (31+28+31+30+31+30+31+31+30),
    (31+28+31+30+31+30+31+31+30+31),
    (31+28+31+30+31+30+31+31+30+31+30),
    (31+28+31+30+31+30+31+31+30+31+30+31),
  };
static long int timezone;
static const char days[] = "Sun Mon Tue Wed Thu Fri Sat ";
static const char months[] = "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";

/* seconds per day */
#define SPD 24*60*60

int __isleap(int year)
{
	/* every fourth year is a leap year except for century years that are
	 * not divisible by 400. */
	/*  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)); */
	return (!(year % 4) && ((year % 100) || !(year % 400)));
}

struct tm *gmtime_r(const time_t *timep, struct tm *r)
{
	time_t i;
	register time_t work = *timep % (SPD);
	r->tm_sec = work % 60;
	work /= 60;
	r->tm_min = work % 60;
	r->tm_hour = work / 60;
	work = *timep / (SPD);
	r->tm_wday = (4 + work) % 7;
	for (i = 1970;; ++i)
	{
		register time_t k = __isleap(i) ? 366 : 365;
		if (work >= k)
			work -= k;
		else
			break;
	}
	r->tm_year = i - 1900;
	r->tm_yday = work;

	r->tm_mday = 1;
	if (__isleap(i) && (work > 58))
	{
		if (work == 59)
			r->tm_mday = 2; /* 29.2. */
		work -= 1;
	}

	for (i = 11; i && (__spm[i] > work); --i)
		;
	r->tm_mon = i;
	r->tm_mday += work - __spm[i];
	return r;
}

struct tm* localtime_r(const time_t* t, struct tm* r)
{
	time_t tmp;
	struct timezone tz = {0};
	gettimeofday(0, &tz);
	timezone = tz.tz_minuteswest * 60L;
	tmp = *t + timezone;
	return gmtime_r(&tmp, r);
}

struct tm* localtime(const time_t* t)
{
	static struct tm tmp;
	return localtime_r(t, &tmp);
}

time_t mktime(struct tm * const t)
{
	register time_t day;
	register time_t i;
	register time_t years = t->tm_year - 70;

	if (t->tm_sec > 60)
	{
		t->tm_min += t->tm_sec / 60;
		t->tm_sec %= 60;
	}
	if (t->tm_min > 60)
	{
		t->tm_hour += t->tm_min / 60;
		t->tm_min %= 60;
	}
	if (t->tm_hour > 24)
	{
		t->tm_mday += t->tm_hour / 24;
		t->tm_hour %= 24;
	}
	if (t->tm_mon > 12)
	{
		t->tm_year += t->tm_mon / 12;
		t->tm_mon %= 12;
	}
	while (t->tm_mday > __spm[1 + t->tm_mon])
	{
		if (t->tm_mon == 1 && __isleap(t->tm_year + 1900))
		{
			--t->tm_mday;
		}
		t->tm_mday -= __spm[t->tm_mon];
		++t->tm_mon;
		if (t->tm_mon > 11)
		{
			t->tm_mon = 0;
			++t->tm_year;
		}
	}

	if (t->tm_year < 70)
		return (time_t) -1;

	/* Days since 1970 is 365 * number of years + number of leap years since 1970 */
	day = years * 365 + (years + 1) / 4;

	/* After 2100 we have to substract 3 leap years for every 400 years
	 This is not intuitive. Most mktime implementations do not support
	 dates after 2059, anyway, so we might leave this out for it's
	 bloat. */
	if (years >= 131)
	{
		years -= 131;
		years /= 100;
		day -= (years >> 2) * 3 + 1;
		if ((years &= 3) == 3)
			years--;
		day -= years;
	}

	day += t->tm_yday = __spm[t->tm_mon] + t->tm_mday - 1 +
			(__isleap(t->tm_year + 1900) & (t->tm_mon > 1));

	/* day is now the number of days since 'Jan 1 1970' */
	i = 7;
	t->tm_wday = (day + 4) % i; /* Sunday=0, Monday=1, ..., Saturday=6 */

	i = 24;
	day *= i;
	i = 60;
	return ((day + t->tm_hour) * i + t->tm_min) * i + t->tm_sec;
}

static void num2str(char *c, int i)
{
	c[0] = i / 10 + '0';
	c[1] = i % 10 + '0';
}

char *asctime_r(const struct tm *t, char *buf)
{
	/* "Wed Jun 30 21:49:08 1993\n" */
	*(int*) buf = *(int*) (days + (t->tm_wday << 2));
	*(int*) (buf + 4) = *(int*) (months + (t->tm_mon << 2));
	num2str(buf + 8, t->tm_mday);
	if (buf[8] == '0')
		buf[8] = ' ';
	buf[10] = ' ';
	num2str(buf + 11, t->tm_hour);
	buf[13] = ':';
	num2str(buf + 14, t->tm_min);
	buf[16] = ':';
	num2str(buf + 17, t->tm_sec);
	buf[19] = ' ';
	num2str(buf + 20, (t->tm_year + 1900) / 100);
	num2str(buf + 22, (t->tm_year + 1900) % 100);
	buf[24] = '\n';
	return buf;
}

char *asctime(const struct tm *timeptr)
{
	static char buf[25];
	return asctime_r(timeptr, buf);
}

char *ctime(const time_t *timep)
{
	return asctime(localtime(timep));
}

int gettimeofday(struct timeval *tp, void *ignore)
{
	//time_t time;
	
    if (tp != NULL){
        tp->tv_sec = cpu_sys_time_get();
		tp->tv_usec = 0;
    }

	return 0;
}

#endif


#ifdef		CIRCULAR_BUFFER

template<int CAPACITY>
circular_buffer<CAPACITY>::circular_buffer() :
		beg_index_(0), end_index_(0), size_(0){
}

template<int CAPACITY>
circular_buffer<CAPACITY>::~circular_buffer() {
}

template<int CAPACITY>
size_t circular_buffer<CAPACITY>::write(const char *data, size_t len) {

	if (len == 0)
		return 0;

	size_t bytes_to_write = MIN(len, CAPACITY - size_);

	// Write in a single step
	if (bytes_to_write <= CAPACITY - end_index_) {
		memcpy(data_ + end_index_, data, bytes_to_write);
		end_index_ += bytes_to_write;
		if (end_index_ == CAPACITY)
			end_index_ = 0;
	}
	// Write in two steps
	else {
		size_t size_1 = CAPACITY - end_index_;
		memcpy(data_ + end_index_, data, size_1);
		size_t size_2 = bytes_to_write - size_1;
		memcpy(data_, data + size_1, size_2);
		end_index_ = size_2;
	}

	size_ += bytes_to_write;
	return bytes_to_write;
}

template<int CAPACITY>
size_t circular_buffer<CAPACITY>::read(char *data, size_t len) {
	if (len == 0)
		return 0;

	size_t bytes_to_read = MIN(len, size_);

	// Read in a single step
	if (bytes_to_read <= CAPACITY - beg_index_) {
		memcpy(data, data_ + beg_index_, bytes_to_read);
		beg_index_ += bytes_to_read;
		if (beg_index_ == CAPACITY)
			beg_index_ = 0;
	}
	// Read in two steps
	else {
		size_t size_1 = CAPACITY - beg_index_;
		memcpy(data, data_ + beg_index_, size_1);
		size_t size_2 = bytes_to_read - size_1;
		memcpy(data + size_1, data_, size_2);
		beg_index_ = size_2;
	}

	size_ -= bytes_to_read;
	return bytes_to_read;
}

#endif
