#include    "service.h"
#include    <string.h>
#include "../bsp/hal/hal.h"
#include 	 "../bsp/driver/drv_interface.h"

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

list *list_find(list_head_t *head, list_node_t *node)
{
	list_node_t 	*it;

	list_for_each(it, head){
		if (it == node){
			return it;
		}
	}

	return NULL;
}

void sv_object_init(struct sv_object *object, enum sv_object_class_type type, const char *name)
{
	
}

void sv_object_detach(sv_object_t object)
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
	sv_object_init(&(mq->parent.parent), SV_Object_Class_MessageQueue, name);

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

static void _sv_timer_init(sv_timer_t timer,
						   void (*timeout)(void *parameter), void *parameter,
						   tick_t time, uint8_t flag)
{
	/* set flag */
	timer->parent.flag  = flag;

	/* set deactivated */
	timer->parent.flag &= ~(SV_TIMER_FLAG_ACTIVATED|SV_TIMER_FLAG_TIMEOUT);

	timer->timeout_func = timeout;
	timer->parameter    = parameter;

	timer->timeout_tick = 0;
	timer->init_tick    = time;

	/* initialize timer list */
	list_init(&(timer->list));
}

static tick_t sv_timer_list_next_timeout(list_t *timer_list)
{
	struct sv_timer *timer;

	if (list_isempty(timer_list))
		return TICK_MAX;

	timer = list_entry(timer_list->m_next, struct sv_timer, list);

	return timer->timeout_tick;
}

void sv_timer_init(sv_timer_t timer,
				   const char *name,
				   void (*timeout)(void *parameter), void *parameter,
				   tick_t time, uint8_t flag)
{
	/* timer object initialization */
	sv_object_init((sv_object_t)timer, SV_Object_Class_Timer, name);
	_sv_timer_init(timer, timeout, parameter, time, flag);
}

sv_err_t sv_timer_detach(sv_timer_t timer)
{
	sv_base_t level;

	/* disable interrupt */
	level = sv_hw_interrupt_disable();

	/* remove it from timer list */
	list_del(&(timer->list));

	/* enable interrupt */
	sv_hw_interrupt_enable(level);

	sv_object_detach((sv_object_t)timer);

	return -SV_EOK;
}

extern tick_t cpu_tick_get(void);

sv_err_t sv_timer_start(sv_timer_t timer)
{
	struct sv_timer *t;
	sv_base_t level;
	list_t *n, *timer_list;

	/* timer check */
	if (timer->parent.flag & SV_TIMER_FLAG_ACTIVATED)
		return -SV_ERROR;

	timer->timeout_tick = cpu_tick_get() + timer->init_tick;

	/* disable interrupt */
	level = sv_hw_interrupt_disable();

	if (timer->parent.flag & SV_TIMER_FLAG_SOFT_TIMER) {
		/* insert timer to soft timer list */
		timer_list = &t_timer_manage.m_soft_timer_list;
	}
	else {
		/* insert timer to system timer list */
		timer_list = &t_timer_manage.m_timer_list;
	}

	for (n = timer_list->m_next; n != timer_list; n = n->m_next)
	{
		t = list_entry(n, struct sv_timer, list);

		/*
		 * It supposes that the new tick shall less than the half duration of
		 * tick max.
		 */
		if ((t->timeout_tick - timer->timeout_tick) < TICK_MAX/2)
		{
			list_insert_before(n, &(timer->list));
			break;
		}
	}
	/* no found suitable position in timer list */
	if (n == timer_list)
	{
		list_insert_before(n, &(timer->list));
	}

	timer->parent.flag |= SV_TIMER_FLAG_ACTIVATED;
	timer->parent.flag &= ~SV_TIMER_FLAG_TIMEOUT;

	/* enable interrupt */
	sv_hw_interrupt_enable(level);

	return -SV_EOK;
}

sv_err_t sv_timer_stop(sv_timer_t timer)
{
	sv_base_t level;

	/* timer check */
	if (!(timer->parent.flag & SV_TIMER_FLAG_ACTIVATED))
		return -SV_ERROR;

	/* disable interrupt */
	level = sv_hw_interrupt_disable();

	/* remove it from timer list */
	list_remove(&(timer->list));

	/* enable interrupt */
	sv_hw_interrupt_enable(level);

	/* change stat */
	timer->parent.flag &= ~(SV_TIMER_FLAG_ACTIVATED|SV_TIMER_FLAG_TIMEOUT);

	return SV_EOK;
}

tick_t sv_tick_from_millisecond(uint32_t ms)
{
	/* return the calculated tick */
	return (TICK_PER_SECOND * ms + 999) / 1000;
}


timer_manage::timer_manage()
	:m_size(def_MONITOR_TIME_NR),
	m_bitmap(0)
{
}

void timer_manage::init(void)
{

	list_init(&m_timer_list);
	list_init(&m_soft_timer_list);
	m_soft_timer_handling = false;
    //�����豸
    m_handle_timer   = hal_devicefind(DEVICE_NAME_TIMER);
    //���豸
    hal_deviceopen(m_handle_timer, DEVICE_FLAG_RDWR);

}

timer_handle_type timer_manage::hard_timer_register(uint32 expired_ms,
                                                    uint8 flag,
                                                    fp_void_pvoid *func,
                                                    void *data,
                                                    const char *pname)
{
	flag					&= ~SV_TIMER_FLAG_HARD_TIMER;
	return timer_register(expired_ms, flag|SV_TIMER_FLAG_HARD_TIMER, func, data, pname);
}

timer_handle_type timer_manage::soft_timer_register(uint32 expired_ms,
                                                    uint8 flag,
                                                    fp_void_pvoid *func,
                                                    void *data,
                                                    const char *pname)
{
	flag					&= ~SV_TIMER_FLAG_HARD_TIMER;
	return timer_register(expired_ms, flag, func, data, pname);
}

timer_handle_type timer_manage::timer_register(uint32 expired_ms,
                                                    uint8 flag,
                                                    fp_void_pvoid *func,
                                                    void *data,
                                                    const char *pname)
{
	timer_handle_type handle;
	portuBASE_TYPE 		i;

	for (i = 0; i < m_size; ++i){
		if (!(m_bitmap & (1UL << i))){
			break;
		}
	}
	if (i >= m_size){
		return (timer_handle_type)-1;
	}
	handle 									= i;
	m_bitmap 								|= (1UL << i);
	sv_timer_init(&m_timer[handle], pname, func, data, sv_tick_from_millisecond(expired_ms), flag);

    return handle;
}

bool timer_manage::timer_unregister(timer_handle_type handle)
{
	if (handle >= m_size){
		return false;
	}
	m_bitmap 								&= ~(1UL << handle);
	m_timer[handle].parent.flag 			&= ~SV_TIMER_FLAG_ACTIVATED;

    return true;
}

int timer_manage::timer_start(timer_handle_type handle)
{
	if (handle >= m_size) {
		return false;
	}
	return sv_timer_start(&m_timer[handle]);
}

int timer_manage::timer_stop(timer_handle_type handle)
{
	if (handle >= m_size) {
		return false;
	}
	return sv_timer_stop(&m_timer[handle]);
}

void timer_manage::timer_check(void)
 {
	struct sv_timer *t;
	tick_t current_tick;
	tick_t soft_timer_timeout_tick;
	sv_base_t level;

	current_tick 							= cpu_tick_get();

	/* disable interrupt */
	level = sv_hw_interrupt_disable();

	//hard timer handle  run in interrupt content
	while (!list_isempty(&m_timer_list)) {
		t = list_entry(m_timer_list.m_next, struct sv_timer, list);

		/*
		 * It supposes that the new tick shall less than the half duration of
		 * tick max.
		 */
		if ((current_tick - t->timeout_tick) < TICK_MAX / 2) {

			/* remove timer from timer list firstly */
			list_remove(&(t->list));

			/* call timeout function */
			t->timeout_func(t->parameter);

			/* re-get tick */
			current_tick = cpu_tick_get();

			if ((t->parent.flag & SV_TIMER_FLAG_PERIODIC)
					&& (t->parent.flag & SV_TIMER_FLAG_ACTIVATED)) {
				/* start it */
				t->parent.flag &= ~SV_TIMER_FLAG_ACTIVATED;
				sv_timer_start(t);
			} else {
				/* stop timer */
				t->parent.flag &= ~SV_TIMER_FLAG_ACTIVATED;
				t->parent.flag |= SV_TIMER_FLAG_TIMEOUT;
			}
		} else
			break;
	}
	//soft timer handle     run in event loop
	while ((m_soft_timer_handling == false) && !list_isempty(&m_soft_timer_list)) {
		/* re-get tick */
		current_tick = cpu_tick_get();
		soft_timer_timeout_tick 	= sv_timer_list_next_timeout(&m_soft_timer_list);
		if ((current_tick - soft_timer_timeout_tick) < TICK_MAX / 2) {
			uint32 	event = 0;
			//write timer-device to read ready
			hal_devicewrite(m_handle_timer, 0, (void *)&event, sizeof(event));
			m_soft_timer_handling					= true;
			break;

		}
	}

	/* enable interrupt */
	sv_hw_interrupt_enable(level);
}

timer_manage 	t_timer_manage;


void sv_service_init(void)
{
	t_timer_manage.init();
}





















