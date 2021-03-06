#include "event_loop.h"
#include "poller.h"
#include "channel.h"
#include "timer_queue.h"
#include "../app/event.h"
#include "../app/devices.h"

const int kPollTimeMs = 2000;

static device_event 		t_device_event;

static class channel *event_create(eventloop *loop)
{
    static channel  event(loop, &t_device_event);
    
    t_device_event.open(DEVICE_FLAG_RDWR);

    return &event;
}

eventloop::eventloop() :
		looping_(false),
		eventHandling_(false),
		callingPendingFunctors_(false),
		m_current_acitve_channel(NULL),
		m_ppoller(poller::newDefaultPoller(this)),
		m_pchannel_event(event_create(this)),
		m_ptimer_queue(timer_queue::new_timerqueue(this))
{
	list_init(&m_list_event);
	m_pchannel_event->event_handle_register(&eventloop::event_handle, this);
	m_pchannel_event->enableReading();
}

eventloop::~eventloop() {
}

void eventloop::loop() {
	looping_ = true;
	channel 	*it;
	list_node_t 	*pos;

	while (true == looping_){
		list_init(&m_active_channels);

		pollReturnTime_ = m_ppoller->poll(kPollTimeMs, &m_active_channels);

		list_for_each(pos, &m_active_channels){

			it		= list_entry_offset(pos, class channel, channel::active_list_node_offset_get());
			m_current_acitve_channel	= it;
			it->handleEvent(pollReturnTime_);
		}
	}
}

void eventloop::update_channel(channel* channel) {
	m_ppoller->updateChannel(channel);
}

void eventloop::remove_channel(channel* channel) {
	m_ppoller->removeChannel(channel);
}

timer_handle_type eventloop::run_at(const uint32& ms, uint8 flag, fp_void_pvoid *cb, void *pparam, const char* pname)
{
	return m_ptimer_queue->timer_add(ms, flag, cb ,pparam, pname);
}

bool eventloop::run_after(uint32 ms, fp_void_pvoid *cb, void *pparam, const char* pname)
{
	timer_handle_type 	handle;

	handle 	= run_at(ms, SV_TIMER_FLAG_SOFT_TIMER|SV_TIMER_FLAG_ONE_SHOT, cb, pparam, pname);
	return (handle == (timer_handle_type)-1)?(false):(true);
}

bool eventloop::run_after(struct tm &tm, fp_void_pvoid *cb, void *pparam, const char* pname)
{
    uint32 ms                       = mktime(&tm)*1000;

    return run_after(ms, cb, pparam, pname);
}

timer_handle_type eventloop::run_every(uint32 ms, fp_void_pvoid *cb, void *pparam, const char* pname)
{
	return run_at(ms, SV_TIMER_FLAG_SOFT_TIMER|SV_TIMER_FLAG_PERIODIC, cb, pparam, pname);
}

timer_handle_type eventloop::run_every(struct tm &tm, fp_void_pvoid *cb, void *pparam, const char* pname)
{
    uint32 ms                       = mktime(&tm)*1000;
    return run_every(ms, cb, pparam, pname);
}

portBASE_TYPE eventloop::timer_ioctl(timer_handle_type handle, enum timer_ioc ioc, void *pparam)
{
	return m_ptimer_queue->timer_ioctl(handle, ioc, pparam);
}

int eventloop::event_handle(void *pvoid, int event_type, class buffer &buf, class Timestamp &ts)
{
	eventloop *peventloop      = reinterpret_cast<eventloop *> (pvoid);

	//event  readable
	if (event_type == POLLIN){
		uint32 	event_var;
		list_head_t 	list_head_event;
		sv_base_t level;
		list_node_t 	*pos;
		struct event 	*it;

		list_init(&list_head_event);
        /* disable interrupt   禁止中断 这样在中断中也可使用run in loop 函数*/
		level = sv_hw_interrupt_disable();
		list_splice_init(&(peventloop->m_list_event), &list_head_event);
		t_device_event.read(reinterpret_cast<char *>(&event_var), sizeof(event_var));
		/* enable interrupt */
		sv_hw_interrupt_enable(level);

		list_for_each(pos, &list_head_event){

			it		= list_entry_offset(pos, class event, event::list_node_offset_get());
			it->call();
			event_manage_singleton()->event_free(it);
		}
	}

	return 0;
}

portBASE_TYPE eventloop::run_inloop(class event *event)
{
	uint32 	event_var 	= 0;
	class event 	*pevent_malloc;

	pevent_malloc 	= event_manage_singleton()->event_malloc();
	if (NULL == pevent_malloc){
		return -1;
	}
	*pevent_malloc 	= *event;
	list_insert_after(&m_list_event, pevent_malloc->list_node_get());
	t_device_event.write(reinterpret_cast<char*>(&event_var), sizeof(event_var));

	return 0;
}

