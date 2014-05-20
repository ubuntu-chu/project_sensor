#include "utils.h"


monitor_handle_type monitor_manage::monitor_register(uint16 expired_time, 
                                                    uint8 mode, 
                                                    pf_void *func, 
                                                    void *data,
                                                    const char *pname)
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
    m_monitor[handle].m_pname 				= pname;
    
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

//杩愯浜庝腑鏂笂涓嬫枃
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
		//SYS_LOG_LEV_TINY_LEN(SYS_LOG_LEV_DEBUG, prefix, strlen((char const * )prefix));
		buff[2] = chartable[(buf[i] >> 4)];
		buff[3] = chartable[(buf[i] & 0x0f)];
		//SYS_LOG_LEV_TINY_LEN(SYS_LOG_LEV_DEBUG, buff, strlen((char const * )buff));
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

void delay_ms(uint16_t num)
{
	uint16_t i = num;
    uint16   j;
    
	for(;i > 0;i--){
		for (j = 0; j < 1000; j++){
            
        }
	}
}

void delay_us(uint16_t num)
{
	uint16_t i = num;
    uint16   j;
    
	for(;i > 0;i--){
		for (j = 0; j < 1; j++){
            
        }
	}
}

#ifdef		def_BUF_QUEUE

void buf_queue_init(buf_queue_t* lq)
{
	lq->front               = lq->rear = 0;
	lq->size                = 0;
    lq->capacity            = def_QUEUE_BUF_CNTS;
}

int8 buf_queue_put(buf_queue_t* lq, int8 *pbuf, uint16 len)
{
	rx_buf_t 	*prx_buf;
    uint8 		 rear;
    portCPSR_TYPE	level;

	if (lq->size >= lq->capacity){
		return -1;
	}
    level 		                = cpu_interruptDisable();
    rear                        = lq->rear;
    lq->rear = (lq->rear + 1) % lq->capacity;
	lq->size++;
    cpu_interruptEnable(level);
    
	prx_buf 		            = &(lq->m_rx_buf[rear]);
	if (len > def_QUEUE_BUF_LEN){
		len 					= def_QUEUE_BUF_LEN;
	}
	prx_buf->m_len	= len;
	if (NULL != pbuf){
		memcpy(prx_buf->m_buf, pbuf, len);
	}

	return 0;
}

int8 buf_queue_get(buf_queue_t* lq, int8 *pbuf, uint16 *len)
{
	rx_buf_t 	    *prx_buf;
    uint8			front;
	portCPSR_TYPE	level;

	if (lq->size <= 0){
		return -1;
	}
    level 		                = cpu_interruptDisable();
	lq->size--;
    front                       = lq->front;
    lq->front = (lq->front + 1) % lq->capacity;
	cpu_interruptEnable(level);
    
	prx_buf 		= &(lq->m_rx_buf[front]);
	if (NULL != len){
		*len				    = prx_buf->m_len;
	}
	if (NULL != pbuf){
		memcpy(pbuf, prx_buf->m_buf, prx_buf->m_len);
	}
	
	return 0;
}

int8 buf_queue_pick(buf_queue_t* lq, int8 *pbuf, uint16 *len)
{
	rx_buf_t 	    *prx_buf;
    portCPSR_TYPE	level;

	if (lq->size <= 0){
		return -1;
	}
    level 		            = cpu_interruptDisable();
	prx_buf 		        = &(lq->m_rx_buf[lq->front]);
	if (NULL != len){
		*len				= prx_buf->m_len;
	}
	if (NULL != pbuf){
		memcpy(pbuf, prx_buf->m_buf, prx_buf->m_len);
	}
    cpu_interruptEnable(level);

	return 0;
}

int8 buf_queue_pop(buf_queue_t* lq)
{
	portCPSR_TYPE	level;

	if (lq->size <= 0){
		return -1;
	}
    level 		            = cpu_interruptDisable();
	lq->size--;
	lq->front = (lq->front + 1) % lq->capacity;
	cpu_interruptEnable(level);
    
    return 0;
}
#endif

#ifdef		def_SKBUF_QUEUE

buf_queue_t 	t_skbuf_queue;

void skbuf_queue_init(void)
{
	buf_queue_init(&t_skbuf_queue);
}

int8 skbuf_queue_put(int8 *pbuf, uint16 len)
{
	return buf_queue_put(&t_skbuf_queue, pbuf, len);
}

int8 skbuf_queue_get(int8 *pbuf, uint16 *len)
{
	return buf_queue_get(&t_skbuf_queue, pbuf, len);
}
#endif

#ifdef def_MINI_TIME_LIBRARY

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


#ifdef		def_CIRCULAR_BUFFER

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

#ifdef		def_SUM16

//check sum
uint16 sum16(const uint8 * ptr, uint16 len)
{
	uint16 val = 0x0000;

	for (uint16 i = 0x0000; i < len; i++) {
		val += ptr[i];
	}

	return val;
}
#endif

#ifdef		def_CRC16

/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
#if 1
uint16 crc16(const uint8 *buf, uint16 len)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (len--) {
        i = crc_hi ^ *buf++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}
#else

uint16 crc16(const uint8 *buf, uint16 len)
{
	unsigned char i,j;
	unsigned short wCRC = 0xFFFF;
	for ( i = 0; i < len; i++)
	{
		wCRC ^= buf[i];

		for ( j = 0; j < 8; j++)
			if (wCRC & 0x0001)
				wCRC = (wCRC >> 1) ^ 0xA001;
			else
				wCRC = wCRC >> 1;
	}
	return wCRC;
}
#endif


#endif


bool is_power_of_2(unsigned long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}


