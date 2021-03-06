#ifndef    _SERVICE_H_
#define    _SERVICE_H_

#include    "config.h"
#include    "types.h"
#include    "macro.h"
#include    <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define 	POLLNONE 					(0x00)
#define 	POLLOUT 					(0x01)
#define	 	POLLIN 						(0x02)

typedef            struct DEVICE_ABSTRACT *                   portDEVHANDLE_TYPE;

#define list_entry(node, type, member) \
                                                                ((type *)((char *)(node) - (unsigned int)(&((type *)0)->member)))

#define list_entry_offset(node, type, offset) \
                                                                ((type *)((char *)(node) - (unsigned int)(offset)))
/*
list_for_each()：list_del(pos)将pos的前后指针指向undefined state，导致kernel panic，
list_del_init(pos)将pos前后指针指向自身，导致死循环。
list_for_each_safe()：首先将pos的后指针缓存到n，处理一个流程后再赋回pos，避免了这种情况发生。
因此之遍历链表不删除结点时，可以使用list_for_each()，而当由删除结点操作时，则要使用list_for_each_safe()。   
*/    
#define list_for_each(pos, head) \
    for (pos = (head)->m_next; pos != (head); \
        pos = pos->m_next)
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->m_next, n = pos->m_next; pos != (head); \
        pos = n, n = pos->m_next)

struct list_node{ 
    struct list_node 				                           *m_next;	    /* point to next node. 						*/
	struct list_node 				                           *m_prev;     /* point to prev node. 						*/
};

typedef struct list_node list;
typedef struct list_node list_t;
typedef struct list_node list_head_t;
typedef struct list_node list_node_t;

void list_init(list *l);
void list_insert_after(list *l, list *n);
void list_insert_before(list *l, list *n);
void list_del(list *n);
#define 	list_remove(n)			list_del(n)
int list_empty(const list *l);
list *list_find(list_head_t *head, list_node_t *node);
void list_splice(list_head_t *list, list_head_t *head);
void list_splice_init(list_head_t *list, list_head_t *head);


#define SV_EOK                          0               /**< There is no error */
#define SV_ERROR                        1               /**< A generic error happens */
#define SV_ETIMEOUT                     2               /**< Timed out */
#define SV_EFULL                        3               /**< The resource is full */
#define SV_EEMPTY                       4               /**< The resource is empty */
#define SV_ENOMEM                       5               /**< No memory */
#define SV_ENOSYS                       6               /**< No system */
#define SV_EBUSY                        7               /**< Busy */
#define SV_EIO                          8               /**< IO error */
#define SV_EPARAM                       9               /**< Param error */



enum sv_object_class_type
{
#ifdef def_USING_SERVICE_MQ
    SV_Object_Class_MessageQueue,                       /**< The object is a message queue. */
#endif
#ifdef def__USING_SERVICE_DEVICE
    SV_Object_Class_Device,                             /**< The object is a device */
#endif
    SV_Object_Class_Timer,
    SV_Object_Class_Unknown,                            /**< The object is unknown. */
};


struct sv_object
{
    const char   *name;                			 /**< name of kernel object */
    uint8_t type;                                    /**< type of kernel object */
    uint8_t flag;                                    /**< flag of kernel object */
    //list_t  list;                                    /**< list node of kernel object */
};
typedef struct sv_object *sv_object_t;                  /**< Type for kernel objects. */


struct sv_ipc_object
{
    struct sv_object parent;                            /**< inherit from rt_object */
};

//------------------------------------------------------------------------------
//unit: ms
#define def_TIME_1_SECOND				(1000UL)
#define def_TIME_1_MINUTE				(60UL*def_TIME_1_SECOND)
#define def_TIME_1_HOUR					(60UL*def_TIME_1_MINUTE)

#define SV_TIMER_FLAG_DEACTIVATED       0x0             /**< timer is deactive */
#define SV_TIMER_FLAG_ACTIVATED         0x1             /**< timer is active */
#define SV_TIMER_FLAG_ONE_SHOT          0x0             /**< one shot timer */
#define SV_TIMER_FLAG_PERIODIC          0x2             /**< periodic timer */

#define SV_TIMER_FLAG_HARD_TIMER        0x0             /**< hard timer,the timer's callback function will be called in tick isr. */
#define SV_TIMER_FLAG_SOFT_TIMER        0x4             /**< soft timer,the timer's callback function will be called in event loop. */

#define SV_TIMER_FLAG_TIMEOUT           0x10             /**< timeout in one shot timer */
#define SV_TIMER_FLAG_HANDLING          0x20             /**< timeout handler in server */

/**
 * timer structure
 */
struct sv_timer
{
    struct sv_object parent;                            /**< inherit from sv_object */
    list_node_t     list;                               /**< the node of timer list */

    fp_void_pvoid   *timeout_func;              /**< timeout function */
    void            *parameter;                         /**< timeout function's parameter */

    tick_t        	init_tick;                          /**< timer timeout tick */
    tick_t        	timeout_tick;                       /**< timeout tick */
};
typedef struct sv_timer *sv_timer_t;

class timer_manage: noncopyable{
public:
    friend  sv_err_t sv_timer_start(sv_timer_t timer);

	timer_manage();
	~timer_manage(){}

	void init(void );

	//return handle for monitor
	timer_handle_type hard_timer_register(uint32 expired_ms,
                                        uint8 flag,
                                        fp_void_pvoid *func,
                                        void *data,
                                        const char *pname);
	timer_handle_type soft_timer_register(uint32 expired_ms,
                                        uint8 flag,
                                        fp_void_pvoid *func,
                                        void *data,
                                        const char *pname);
	bool timer_unregister(timer_handle_type handle);

	sv_err_t timer_start(timer_handle_type handle);
	sv_err_t timer_start(timer_handle_type handle, uint32 expired_ms);
	sv_err_t timer_stop(timer_handle_type handle);
    sv_err_t timer_restart(timer_handle_type handle);
	sv_err_t timer_restart(timer_handle_type handle, uint32 expired_ms);
    sv_err_t timer_timeout(timer_handle_type handle, uint32 expired_ms);
	portBASE_TYPE timer_expired(timer_handle_type handle);

    portBASE_TYPE timer_started(timer_handle_type handle)
	{
		if (handle >= m_size){
			return (portBASE_TYPE)-1;
		}
		return (m_timer[handle].parent.flag & SV_TIMER_FLAG_ACTIVATED)?(true):(false);
	}

	void timer_check(void);
	void soft_timer_handle(void);

private:

	bool timer_unregister(struct sv_timer *ptimer);
	timer_handle_type timer_register(uint32 expired_ms,
                                        uint8 flag,
                                        fp_void_pvoid *func,
                                        void *data,
                                        const char *pname);

	void timer_handle(list_head_t *list_head);
	uint8						m_size;
	bool						m_soft_timer_handling;
    portDEVHANDLE_TYPE			m_handle_timer;
	uint16 						m_bitmap;
	list_head_t					m_timer_list;
	list_head_t					m_soft_timer_list;
	struct sv_timer 		    m_timer[def_MONITOR_TIME_NR];
};

extern timer_manage   t_timer_manage;
#define 	timer_manage_singleton()			singleton<timer_manage>::instance()

//----------------------------------------------------------------------------------------------------------------

class buffer {
public:
	buffer(int8 *pdata, size_t len, size_t cheapPrepend = 4) :
			buffer_(reinterpret_cast<char *>(pdata)),
			len_(len),
			cheapPrepend_(cheapPrepend),
			readerIndex_(cheapPrepend),
			writerIndex_(cheapPrepend)
	{

	}

	~buffer()
	{
	}

	buffer &operator= (const buffer &other)
	{
		if (this == &other){
			return *this;
		}
		if (buffer_ != NULL){
			len_				= 	other.len_;
		}
		readerIndex_ 			= other.readerIndex_;
		writerIndex_ 			= other.writerIndex_;
        cheapPrepend_ 			= other.cheapPrepend_;

		return *this;
	}

	size_t readableBytes() const
	{
		return writerIndex_ - readerIndex_;
	}

	size_t writableBytes() const
	{
		return len_ - writerIndex_;
	}

	size_t prependableBytes() const
	{
		return readerIndex_;
	}

	const char* peek() const
	{
		return begin() + readerIndex_;
	}

	char* peek() 
	{
		return begin() + readerIndex_;
	}

	void retrieve(size_t len)
	{
		if (len < readableBytes()) {
			readerIndex_ += len;
		} else {
			retrieveAll();
		}
	}

	void retrieveUntil(const char* end)
	{
		retrieve(end - peek());
	}

	void retrieveInt32()
	{
		retrieve(sizeof(int32_t));
	}

	void retrieveInt16()
	{
		retrieve(sizeof(int16_t));
	}

	void retrieveInt8()
	{
		retrieve(sizeof(int8_t));
	}

	void retrieveAll()
	{
		readerIndex_ = cheapPrepend_;
		writerIndex_ = cheapPrepend_;
	}

	int retrieveAllAsCharArray(int8 *dst)
	{
		return retrieveAsCharArray(dst, readableBytes());;
	}

	int retrieveAsCharArray(int8 *dst, size_t len)
	{
		if (len > readableBytes()) {
			return -1;
		}
		memcpy(dst, peek(), len);
		retrieve(len);

		return len;
	}

	int append(const char* /*restrict*/data, size_t len)
	{
		if (writableBytes() < len) {
			return -1;
		}
		memcpy(beginWrite(), data, len);
		hasWritten(len);
		return 0;
	}

	int append(const void* /*restrict*/data, size_t len)
	{
		return append(static_cast<const char*>(data), len);
	}
	char* beginWrite()
	{
		return begin() + writerIndex_;
	}

	const char* beginWrite() const
	{
		return begin() + writerIndex_;
	}

	void hasWritten(size_t len)
	{
		writerIndex_ += len;
	}

	///
	/// Append int32_t using network endian
	///
	void appendInt32(int32_t x)
	{
		int32_t be32 = (x);
		append(&be32, sizeof be32);
	}

	void appendInt16(int16_t x)
	{
		int16_t be16 = (x);
		append(&be16, sizeof be16);
	}

	void appendInt8(int8_t x)
	{
		append(&x, sizeof x);
	}

	float readFloat()
	{
		float result = peekFloat();
		retrieveInt32();
		return result;
	}
	///
	/// Read int32_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int32_t readInt32()
	{
		int32_t result = peekInt32();
		retrieveInt32();
		return result;
	}

	int16_t readInt16()
	{
		int16_t result = peekInt16();
		retrieveInt16();
		return result;
	}

	int8_t readInt8()
	{
		int8_t result = peekInt8();
		retrieveInt8();
		return result;
	}

	float peekFloat() const
	{
		float be32 = 0;
		memcpy(&be32, peek(), sizeof be32);
		return (be32);
	}

	int32_t peekInt32() const
	{
		int32_t be32 = 0;
		memcpy(&be32, peek(), sizeof be32);
		return (be32);
	}

	int16_t peekInt16() const
	{
		int16_t be16 = 0;
		memcpy(&be16, peek(), sizeof be16);
		return (be16);
	}

	int8_t peekInt8() const
	{
		int8_t x = *peek();
		return x;
	}

	///
	/// Prepend int32_t using network endian
	///
	void prependInt32(int32_t x)
	{
		int32_t be32 = (x);
		prepend(&be32, sizeof be32);
	}

	void prependInt16(int16_t x)
	{
		int16_t be16 = (x);
		prepend(&be16, sizeof be16);
	}

	void prependInt8(int8_t x)
	{
		prepend(&x, sizeof x);
	}

	void prepend(const void* /*restrict*/data, size_t len)
	{
		readerIndex_ -= len;
		const char* d = static_cast<const char*>(data);
		memcpy(begin() + readerIndex_, d, len);
	}

private:
	char* begin()
	{
		return buffer_;
	}

	const char* begin() const
	{
		return buffer_;
	}

	char *buffer_;
	size_t len_;
	size_t cheapPrepend_;
	size_t readerIndex_;
	size_t writerIndex_;
};

class event;

typedef portBASE_TYPE   (event_handle)(void *pvoid, class event*pevent);
#define     def_EVENT_DATA_SIZE         (15)

class event{
public:
    event(event_handle handler = NULL, void *pvoid = NULL):
    	m_priority(0),
    	m_index(0),
    	m_buffer(m_param, sizeof(m_param)),
    	m_handler(handler),
    	m_pvoid(pvoid)
	{
    	list_init(&m_node);
	}
    ~event(){}
    
    event &operator=(const event &other)
    {
    	if (this == &other){
    		return *this;
    	}
    	m_priority 					= other.m_priority;
    	m_index						= other.m_index;
    	memcpy(m_param, other.m_param, sizeof(m_param));
    	m_handler					= other.m_handler;
    	m_pvoid						= other.m_pvoid;
    	m_buffer 					= other.m_buffer;

    	return *this;
    }

    class buffer &buffer_get(void)
    {
    	return m_buffer;
    }

    void buffer_init(void)
    {
    	m_buffer.retrieveAll();
    }

    portBASE_TYPE call(void)
    {
    	portBASE_TYPE 	rt			= 0;

    	if (NULL != m_handler){
			rt 	= m_handler(m_pvoid, this);
		}

    	return rt;
    }

    static int list_node_offset_get(void)
    {
		return OFFSET(class event, m_node);
    }
	list_node_t* list_node_get(void) { return &m_node; }

    void index_set(uint8 index){ m_index 	= index;}
    uint8 index_get(void){ return m_index;}

private:
    uint8                   			m_priority;                     //优先级
    uint8                   			m_index;                        //index of m_event
    class buffer						m_buffer;
    int8 								m_param[def_EVENT_DATA_SIZE];						//事件参数
    event_handle           				*m_handler;
    void                    			*m_pvoid;						//仅仅存放this指针  不执行任何其他数据
    list_node_t              			m_node;
};

class event_manage{
public:
	event_manage();
	~event_manage();

	event * event_malloc(void);
	sv_err_t event_free(event *pevent);

private:
	uint8								m_size;
	uint16 								m_bitmap;
	event								m_event[def_EVENT_NR];
};

#define 	event_manage_singleton() 		singleton<class event_manage>::instance()


#ifdef def_USING_SERVICE_MQ

#define     def_MQ_ALIGN_SIZE               (4)
#ifdef 	SV_EVENT_QUEUE

struct sv_mq_message
{
	struct sv_mq_message *next;
};

struct sv_eventqueue
{
    struct sv_ipc_object parent;                    /**< inherit from ipc_object */

    event_handle       *m_def_handler;                 //def handler
    void                *m_pvoid;
    void                *msg_pool;                      /**< stasv address of message queue */

    uint16_t            msg_size;                         /**< message size of each message */
    uint16_t            max_msgs;                         /**< max number of messages */
    uint16_t            entry;                            /**< index of messages in the queue */

    void                *msg_queue_head;                /**< list head */
    void                *msg_queue_tail;                /**< list tail */
    void                *msg_queue_free;                /**< pointer indicated the free node of queue */
};
typedef struct sv_eventqueue *sv_eq_t;

portBASE_TYPE sv_eq_init(sv_eq_t mq, const char *name, void *msgpool,
                            portSIZE_TYPE msg_size, portSIZE_TYPE pool_size, uint8_t flag,
                            event_handle *def_handler, void *pprivate);

#endif



#endif



/* device flags */
#define             DEVICE_FLAG_DEACTIVATE		            0x000		    /* not inited 									*/

#define             DEVICE_FLAG_RDONLY			            0x001		    /* read only 									*/
#define             DEVICE_FLAG_WRONLY			            0x002		    /* write only 									*/
#define             DEVICE_FLAG_RDWR				        0x003		    /* read and write 								*/

#define             DEVICE_FLAG_REMOVABLE		            0x004		    /* removable device 							*/
#define             DEVICE_FLAG_STANDALONE		            0x008		    /* standalone device							*/
#define             DEVICE_FLAG_ACTIVATED		            0x010		    /* device is activated 							*/
#define             DEVICE_FLAG_SUSPENDED		            0x020		    /* device is suspended 							*/
#define             DEVICE_FLAG_NONBLOCK		            0x040		    /* device is non-block 							*/


#define             DEVICE_OFLAG_CLOSE			            0x000		    /* device is closed 							*/
#define             DEVICE_OFLAG_RDONLY			            0x001		    /* read only access								*/
#define             DEVICE_OFLAG_WRONLY			            0x002		    /* write only access							*/
#define             DEVICE_OFLAG_RDWR			            0x003		    /* read and write 								*/
#define             DEVICE_OFLAG_OPEN			            0x008		    /* device is opened 							*/


enum{

	DEVICE_IOC_NONBLOCK 	= 0,
	DEVICE_IOC_BLOCK,
	DEVICE_IOC_USER,

};


//pos位置宏定义
#define             DEVICE_SEEK_SET                                0x00
#define             DEVICE_SEEK_CUR                                0x01
#define             DEVICE_SEEK_END                                0x02

/******************************************************************************
 *                           文件接口结构体定义
******************************************************************************/

enum  DEVICE_STATUS_TYPE{
    DEVICE_ENULL       = 0,
    DEVICE_OK,
    DEVICE_ENOSYS,
    DEVICE_EBUSY,
    DEVICE_EOPEN,
    DEVICE_EPARAM_INVALID,
    DEVICE_ECMD_INVALID,
    DEVICE_EOFLG_INVALID,
	DEVICE_EEXEC,
    DEVICE_EAGAIN,
    DEVICE_ETIMEOUT,
    DEVICE_ENOEXSITED,
};

typedef            enum  DEVICE_STATUS_TYPE                   DeviceStatus_TYPE;

enum{
    DEVICE_POLL_ENULL   = -1,
    DEVICE_POLL_ENOSYS  = -2,
    DEVICE_POLLNONE = 0,
    DEVICE_POLLIN   = 0x02,
    DEVICE_POLLOUT  = 0x04,
    DEVICE_POLLHUP  = 0x08,
    DEVICE_POLLERR  = 0x10,
    
};
typedef            int                   DevicePoll_TYPE;

enum DEVICE_CLASS_TYPE{
	DEVICE_CLASS_CHAR = 0,						                            /* character device								*/
	DEVICE_CLASS_BLOCK,							                            /* block device 								*/
	DEVICE_CLASS_NETIF,							                            /* net interface 								*/
	DEVICE_CLASS_MTD,							                            /* memory device 								*/
	DEVICE_CLASS_CAN,							                            /* CAN device 									*/
	DEVICE_CLASS_svC,							                            /* svC device 									*/
	DEVICE_CLASS_SOUND,							                            /* Sound device 								*/
	DEVICE_CLASS_UNKNOWN							                        /* unknown device 								*/
};

typedef            enum DEVICE_CLASS_TYPE                     DeviceClassType;

typedef            struct DEVICE_ABSTRACT *                   pDeviceAbstract;
//typedef            struct DEVICE_ABSTRACT *                   portDEVHANDLE_TYPE;

//设备注册函数指针
typedef     portuBASE_TYPE     (FP_pfregister)   (void);

//接口函数指针定义  多态的接口便于从同一类中所派生出来的多个对象
/* common device interface */
typedef     DeviceStatus_TYPE  (FP_pfinit)	     (pDeviceAbstract pdev);
typedef     DeviceStatus_TYPE  (FP_pfopen)	     (pDeviceAbstract pdev, uint16 oflag);
typedef     DeviceStatus_TYPE  (FP_pfclose)      (pDeviceAbstract pdev);
typedef     portSSIZE_TYPE      (FP_pfread)	     (pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
typedef     portSSIZE_TYPE      (FP_pfwrite)      (pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
typedef     DeviceStatus_TYPE  (FP_pfcontrol)    (pDeviceAbstract pdev, uint8 cmd, void *args);
typedef     DevicePoll_TYPE  (FP_pfpoll)      (pDeviceAbstract pdev);

/* device call back */
typedef     DeviceStatus_TYPE  (FP_pfrx_indicate)(pDeviceAbstract pdev, portSIZE_TYPE size);
typedef     DeviceStatus_TYPE  (FP_pftx_complete)(pDeviceAbstract pdev, void* buffer);

struct DEVIVE_ABSTRACT_INFO
{
    //设备名字
    const char      	                                       *m_name;
	DeviceClassType                                             m_type;
    /* device flag*/
	uint16                                                      m_flag;

	/* common device interface */
	FP_pfinit                                                  *init;
    FP_pfopen                                                  *open;
    FP_pfclose                                                 *close;
    FP_pfread                                                  *read;
    FP_pfwrite                                                 *write;
    FP_pfcontrol                                               *control;
    FP_pfpoll                                                  *poll;

    /* device call back */
    FP_pfrx_indicate                                           *rx_indicate;
    FP_pftx_complete                                           *tx_complete;
};

typedef            struct DEVIVE_ABSTRACT_INFO                DeviceAbstractInfo;

//在RAM较小的应用环境中 设备信息一般而言都是固化在FLASH中的 涉及到变动的机会很小
struct DEVICE_ABSTRACT
{
	const struct DEVIVE_ABSTRACT_INFO                        *m_pdeviceAbstractInfo;

    //设备链表
    list	                                                    m_list;
    uint16                                                      m_openFlag;
    /* device private data */
	void                                                      *m_private;
};

typedef            struct DEVICE_ABSTRACT                      DeviceAbstract;
typedef            struct DEVICE_ABSTRACT                      device_t;


#define			sv_hw_interrupt_disable() 			cpu_interruptDisable();
#define			sv_hw_interrupt_enable(level)  			cpu_interruptEnable(level);



void sv_startup(void);
tick_t sv_tick_get(void);
void sv_tick_set(tick_t tick);
void sv_tick_increase(void);
void sv_xtime_set(time_t time);
int gettimeofday(struct timeval *tp, void *ignore);

#ifdef __cplusplus
}
#endif


/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
