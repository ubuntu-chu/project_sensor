#ifndef    _SERVICE_H_
#define    _SERVICE_H_

#include    "config.h"
#include    "types.h"
#include    "macro.h"
#include    <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define list_entry(node, type, member) \
                                                                ((type *)((char *)(node) - (unsigned int)(&((type *)0)->member)))

/*
list_for_each()：list_del(pos)将pos的前后指针指向undefined state，导致kernel panic，
list_del_init(pos)将pos前后指针指向自身，导致死循环。
list_for_each_safe()：首先将pos的后指针缓存到n，处理一个流程后再赋回pos，避免了这种情况发生。
因此之遍历链表不删除结点时，可以使用list_for_each()，而当由删除结点操作时，则要使用list_for_each_safe()。   
*/    
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); \
        pos = pos->next)  
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next) 

struct list_node{ 
    struct list_node 				                           *m_next;	    /* point to next node. 						*/
	struct list_node 				                           *m_prev;     /* point to prev node. 						*/
};

typedef struct list_node list;
typedef struct list_node list_t;

void list_init(list *l);
void list_insert_after(list *l, list *n);
void list_insert_before(list *l, list *n);
void list_del(list *n);
int list_isempty(const list *l);


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

class Buffer{
public:
    static const size_t kCheapPrepend = 4;

    Buffer(int8 *pdata, size_t len)
    : buffer_(reinterpret_cast<char *>(pdata)),
      len_(len),
      readerIndex_(kCheapPrepend),
      writerIndex_(kCheapPrepend)
    {
        
    }
    ~Buffer(){}
        
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


#ifdef __cplusplus
}
#endif


/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/
#endif
