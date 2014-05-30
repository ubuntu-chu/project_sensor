#include    "service.h"
#include    <string.h>

void list_init(list *l)
{
	l->m_next                                                   = l; 
    l->m_prev                                                   = l;
}

void list_insert_after(list *l, list *n)
{
	l->m_next->m_prev                                           = n;
	n->m_next                                                   = l->m_next;

	l->m_next                                                   = n;
	n->m_prev                                                   = l;
}

void list_insert_before(list *l, list *n)
{
	l->m_prev->m_next                                           = n;
	n->m_prev                                                   = l->m_prev;

	l->m_prev                                                   = n;
	n->m_next                                                   = l;
}

void list_del(list *n)
{
	n->m_next->m_prev                                           = n->m_prev;
	n->m_prev->m_next                                           = n->m_next;

	n->m_next                                                   = n;
    n->m_prev                                                   = n;
}

int list_isempty(const list *l)
{
	return (l->m_next == l);
}

void rt_object_init(struct sv_object *object, enum sv_object_class_type type, const char *name)
{
	
}

extern void cpu_interruptEnable(portCPSR_TYPE level);
extern portCPSR_TYPE cpu_interruptDisable(void);

size_t kCheapPrepend;

/**
 * This function will initialize a message queue and put it under control of resource
 * management.
 *
 * @param mq the message object
 * @param name the name of message queue
 * @param msgpool the beginning address of buffer to save messages
 * @param msg_size the maximum size of message
 * @param pool_size the size of buffer to save messages
 * @param flag the flag of message queue
 *
 * @return the operation status, RT_EOK on successful
 */
portBASE_TYPE sv_eq_init(sv_eq_t mq, const char *name, void *msgpool, 
                            portSIZE_TYPE msg_size, portSIZE_TYPE pool_size, uint8_t flag,
                            event_handler *def_handler, void *pprivate)
{
	struct sv_mq_message *head;
	portuBASE_TYPE  temp;

	/* init object */
	rt_object_init(&(mq->parent.parent), SV_Object_Class_MessageQueue, name);

	/* set parent flag */
	mq->parent.parent.flag = flag;

	/* set messasge pool */
	mq->msg_pool        = msgpool;
    mq->m_def_handler   = def_handler;
    mq->m_pvoid         = pprivate;

	/* get correct message size */
	mq->msg_size = ALIGN_UP(msg_size, def_MQ_ALIGN_SIZE);
	mq->max_msgs = pool_size / (mq->msg_size + sizeof(struct sv_mq_message));

	/* init message list */
	mq->msg_queue_head = NULL;
	mq->msg_queue_tail = NULL;

	/* init message empty list */
	mq->msg_queue_free = NULL;
	for (temp = 0; temp < mq->max_msgs; temp ++)
	{
		head = (struct sv_mq_message *)((uint8_t *)mq->msg_pool +
			temp * (mq->msg_size + sizeof(struct sv_mq_message)));
		head->next = (struct sv_mq_message *)mq->msg_queue_free;
		mq->msg_queue_free = head;
	}

	/* the initial entry is zero */
	mq->entry = 0;

	return SV_EOK;
}


portBASE_TYPE sv_eq_send(sv_eq_t mq, void *buffer, portSIZE_TYPE size)
{
	portCPSR_TYPE temp;
	struct sv_mq_message *msg;
 
	/* greater than one message size */
	if (size > mq->msg_size)
		return -SV_ERROR;

	/* disable interrupt */
	temp = cpu_interruptDisable();

	/* get a free list, there must be an empty item */
	msg = (struct sv_mq_message*)mq->msg_queue_free;
	/* message queue is full */
	if (msg == NULL)
	{
		/* enable interrupt */
		cpu_interruptEnable(temp);

		return -SV_EFULL;
	}
	/* move free list pointer */
	mq->msg_queue_free = msg->next;

	/* enable interrupt */
	cpu_interruptEnable(temp);

	/* the msg is the new tailer of list, the next shall be NULL */
	msg->next = NULL;
	/* copy buffer */
	memcpy(msg + 1, buffer, size);

	/* disable interrupt */
	temp = cpu_interruptDisable();
	/* link msg to message queue */
	if (mq->msg_queue_tail != NULL)
	{
		/* if the tail exists, */
		((struct sv_mq_message *)mq->msg_queue_tail)->next = msg;
	}

	/* set new tail */
	mq->msg_queue_tail = msg;
	/* if the head is empty, set head */
	if (mq->msg_queue_head == NULL)
		mq->msg_queue_head = msg;

	/* increase message entry */
	mq->entry ++;

	/* enable interrupt */
	cpu_interruptEnable(temp);

	return SV_EOK;
}

portBASE_TYPE sv_eq_recv(sv_eq_t mq, void *buffer, portSIZE_TYPE size, int32_t timeout)
{
	portCPSR_TYPE temp;
	struct sv_mq_message *msg;
	uint32_t tick_delta;
    
	/* initialize delta tick */
	tick_delta = 0;

	/* disable interrupt */
	temp = cpu_interruptDisable();

	/* for non-blocking call */
	if (mq->entry == 0 && timeout == 0){
		cpu_interruptEnable(temp);

		return -SV_ETIMEOUT;
	}

	/* message queue is empty */
	while (mq->entry == 0)
	{
		/* no waiting, return timeout */
		if (timeout == 0)
		{
			/* enable interrupt */
			cpu_interruptEnable(temp);
			return -SV_ETIMEOUT;
		}
   #if 0
		/* has waiting time, start thread timer */
		if (timeout > 0)
		{
			/* get the start tick of timer */
			tick_delta = rt_tick_get();

			RT_DEBUG_LOG(RT_DEBUG_IPC, ("set thread:%s to timer list\n", thread->name));

			/* reset the timeout of thread timer and start it */
			rt_timer_control(&(thread->thread_timer), RT_TIMER_CTRL_SET_TIME, &timeout);
			rt_timer_start(&(thread->thread_timer));
		}
     #endif

		/* enable interrupt */
		cpu_interruptEnable(temp);
   #if 0     
		/* disable interrupt */
		temp = cpu_interruptDisable();

		/* if it's not waiting forever and then re-calculate timeout tick */
		if (timeout > 0)
		{
			tick_delta = rt_tick_get() - tick_delta;
			timeout -= tick_delta;
			if (timeout < 0)
				timeout = 0;
		}
    #endif
	}

	/* get message from queue */
	msg = (struct sv_mq_message *)mq->msg_queue_head;

	/* move message queue head */
	mq->msg_queue_head = msg->next;
	/* reach queue tail, set to NULL */
	if (mq->msg_queue_tail == msg)
		mq->msg_queue_tail = NULL;

	/* decrease message entry */
	mq->entry --;

	/* enable interrupt */
	cpu_interruptEnable(temp);

	/* copy message */
	memcpy(buffer, msg + 1, size > mq->msg_size ? mq->msg_size : size);

	/* disable interrupt */
	temp = cpu_interruptDisable();
	/* put message to free list */
	msg->next = (struct sv_mq_message *)mq->msg_queue_free;
	mq->msg_queue_free = msg;
	/* enable interrupt */
	cpu_interruptEnable(temp);

	return SV_EOK;
}










