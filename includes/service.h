#ifndef    _SERVICE_H_
#define    _SERVICE_H_

#include    "config.h"
#include    "types.h"
#include    "macro.h"
#include    <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define 	POLLOUT 					(0x01)
#define	 	POLLIN 						(0x02)

#define list_entry(node, type, member) \
                                                                ((type *)((char *)(node) - (unsigned int)(&((type *)0)->member)))

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
int list_isempty(const list *l);
list *list_find(list_head_t *head, list_node_t *node);


#define SV_EOK                          0               /**< There is no error */
#define SV_ERROR                        1               /**< A generic error happens */
#define SV_ETIMEOUT                     2               /**< Timed out */
#define SV_EFULL                        3               /**< The resource is full */
#define SV_EEMPTY                       4               /**< The resource is empty */
#define SV_ENOMEM                       5               /**< No memory */
#define SV_ENOSYS                       6               /**< No system */
#define SV_EBUSY                        7               /**< Busy */
#define SV_EIO                          8               /**< IO error */



enum sv_object_class_type
{
#ifdef def_USING_SERVICE_MQ
    SV_Object_Class_MessageQueue,                       /**< The object is a message queue. */
#endif
#ifdef def__USING_SERVICE_DEVICE
    SV_Object_Class_Device,                             /**< The object is a device */
#endif
    SV_Object_Class_Unknown,                            /**< The object is unknown. */
};


struct sv_object
{
    char       name[OBJECT_NAME_MAX];                /**< name of kernel object */
    uint8_t type;                                    /**< type of kernel object */
    uint8_t flag;                                    /**< flag of kernel object */
    list_t  list;                                    /**< list node of kernel object */
};
typedef struct sv_object *sv_object_t;                  /**< Type for kernel objects. */


struct sv_ipc_object
{
    struct sv_object parent;                            /**< inherit from rt_object */
};


//compatible c/c++
#ifdef def_USING_SERVICE_MQ

#define     def_MQ_ALIGN_SIZE               (4)

enum   event_type{
    enum_TYPE_ANA       = 0,
    enum_TYPE_MAX,
}; 

enum   event_subtype{
    enum_SUBTYPE_UINT8       = 0,
    enum_SUBTYPE_INT8,
    enum_SUBTYPE_UINT16,
    enum_SUBTYPE_INT16,
    enum_SUBTYPE_UINT32,
    enum_SUBTYPE_INT32,
    enum_SUBTYPE_UINT64,
    enum_SUBTYPE_INT64,
    enum_SUBTYPE_FLOAT,
    enum_SUBTYPE_DOUBLE,
    enum_SUBTYPE_STRING,
    enum_SUBTYPE_MAX,
};

class buffer{
public:
    static const size_t kCheapPrepend = 4;

    buffer(int8 *pdata, size_t len)
    : buffer_(reinterpret_cast<char *>(pdata)),
      len_(len),
      readerIndex_(kCheapPrepend),
      writerIndex_(kCheapPrepend)
    {
        
    }
    ~buffer(){}
        
    size_t readableBytes() const
    { return writerIndex_ - readerIndex_; }

    size_t writableBytes() const
    { return len_ - writerIndex_; }

    size_t prependableBytes() const
    { return readerIndex_; }

    const char* peek() const
    { return begin() + readerIndex_; }
    
    void retrieve(size_t len)
    {
        if (len < readableBytes())
        {
            readerIndex_ += len;
        }
        else
        {
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
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }
    
    int append(const char* /*restrict*/ data, size_t len)
    {
        if (writableBytes() < len)
        {
            return -1;
        }
        memcpy(beginWrite(), data, len);
        hasWritten(len);
        return 0;
    }

    int append(const void* /*restrict*/ data, size_t len)
    {
        return append(static_cast<const char*>(data), len);
    }
    char* beginWrite()
  { return begin() + writerIndex_; }

  const char* beginWrite() const
  { return begin() + writerIndex_; }

  void hasWritten(size_t len)
  { writerIndex_ += len; }

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

  void prepend(const void* /*restrict*/ data, size_t len)
  {
    readerIndex_ -= len;
    const char* d = static_cast<const char*>(data);
    memcpy(begin()+readerIndex_, d, len);
  }
    

    
private:
    char* begin()
    { return buffer_; }

    const char* begin() const
    { return buffer_; }


    char        *buffer_;
    size_t      len_;
    size_t      readerIndex_;
    size_t      writerIndex_;
};    

class event_param;

typedef portBASE_TYPE   (event_handler)(void *, class event_param *);

class event_param{
    event_param(enum event_type type, enum event_subtype subtype, int8 *pval):m_type(type),
        m_subtype(subtype),m_pval(pval){}
    ~event_param(){}
    
    enum   event_type       m_type;
    enum   event_subtype    m_subtype;
    int8                   *m_pval;                     //value ptr 
}; 

#define     def_EVENT_DATA_SIZE         (15)

class event{
    event(event_handler handler, void *pprivate):m_handler(handler), m_pprivate(pprivate){}
    ~event(){}
    
    uint8                   m_priority;                         //优先级
    int8                    m_data[def_EVENT_DATA_SIZE];
    event_handler           *m_handler;
    void                    *m_pprivate;
    list                    m_event_list;
};

/*
int32_t peekInt32() const
  {
    assert(readableBytes() >= sizeof(int32_t));
    int32_t be32 = 0;
    ::memcpy(&be32, peek(), sizeof be32);
    return sockets::networkToHost32(be32);
  }
*/

class event_loop{
public:
    event_loop()
    {
        
    }
    ~event_loop(){}
    
    portBASE_TYPE run(struct event &event);
    
        
        
private:
    
    list                m_event_list;

};






struct sv_mq_message
{
	struct sv_mq_message *next;
};

struct sv_eventqueue
{
    struct sv_ipc_object parent;                    /**< inherit from ipc_object */

    event_handler       *m_def_handler;                 //def handler
    void                *m_pvoid;
    void                *msg_pool;                      /**< start address of message queue */

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
                            event_handler *def_handler, void *pprivate);




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

enum  DEVICE_STATUS_TYPE
{
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
};

typedef            enum  DEVICE_STATUS_TYPE                   DeviceStatus_TYPE;

enum DEVICE_CLASS_TYPE
{
	DEVICE_CLASS_CHAR = 0,						                            /* character device								*/
	DEVICE_CLASS_BLOCK,							                            /* block device 								*/
	DEVICE_CLASS_NETIF,							                            /* net interface 								*/
	DEVICE_CLASS_MTD,							                            /* memory device 								*/
	DEVICE_CLASS_CAN,							                            /* CAN device 									*/
	DEVICE_CLASS_RTC,							                            /* RTC device 									*/
	DEVICE_CLASS_SOUND,							                            /* Sound device 								*/
	DEVICE_CLASS_UNKNOWN							                        /* unknown device 								*/
};

typedef            enum DEVICE_CLASS_TYPE                     DeviceClassType;

typedef            struct DEVICE_ABSTRACT *                   pDeviceAbstract;
typedef            struct DEVICE_ABSTRACT *                   portDEVHANDLE_TYPE;

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

/* device call back */
typedef     DeviceStatus_TYPE  (FP_pfrx_indicate)(pDeviceAbstract pdev, portSIZE_TYPE size);
typedef     DeviceStatus_TYPE  (FP_pftx_complete)(pDeviceAbstract pdev, void* buffer);

struct DEVIVE_ABSTRACT_INFO
{
    //设备名字
    int8      	                                                m_name[DEVICE_NAME_MAX];
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


#ifdef __cplusplus
}
#endif


/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
