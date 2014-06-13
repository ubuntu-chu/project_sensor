#include "event_loop.h"
#include "poller.h"
#include "channel.h"

const int kPollTimeMs = 1000;

eventloop::eventloop() :
		looping_(false),
		quit_(false),
		eventHandling_(false),
		callingPendingFunctors_(false),
		m_current_acitve_channel(NULL),
		m_ppoller(poller::newDefaultPoller(this),
		//m_event_channel(, this))
	
{
	list_init(&m_event_list);
}

eventloop::~eventloop() {
}

void eventloop::loop() {
	looping_ = true;
	quit_ = false;
	channel 	*it;
	list_node_t 	*pos;

	while (1){
		list_init(&m_active_channels);

		m_ppoller->poll(kPollTimeMs, &m_active_channels);
//		pollReturnTime_ = m_ppoller->poll(kPollTimeMs, &m_active_channels);

		list_for_each(pos, &m_active_channels){

			it		= list_entry_offset(pos, class channel, it->active_list_node_offset_get());
			m_current_acitve_channel	= it;
			it->handleEvent(pollReturnTime_);
		}
	}

}

void eventloop::updateChannel(channel* channel) {
	m_ppoller->updateChannel(channel);
}

void eventloop::removeChannel(channel* channel) {
	m_ppoller->removeChannel(channel);
}

timer_handle_type eventloop::run_at(const uint32& ms, uint8 flag, fp_void_pvoid *cb, void *pparam, const char* pname)
{
	timer_handle_type	handle;

	handle 	= t_timer_manage.timer_register(ms, flag, cb, pparam, pname);
    if (handle!= (timer_handle_type)-1){
    	return handle;
    }
    t_timer_manage.timer_start(handle);


    return handle;
}

timer_handle_type eventloop::run_after(uint32 ms, fp_void_pvoid *cb, void *pparam, const char* pname)
{
	return run_at(ms, SV_TIMER_FLAG_SOFT_TIMER|SV_TIMER_FLAG_ONE_SHOT, cb, pparam, pname);
}

timer_handle_type eventloop::run_every(uint32 ms, fp_void_pvoid *cb, void *pparam, const char* pname)
{
	return run_at(ms, SV_TIMER_FLAG_SOFT_TIMER|SV_TIMER_FLAG_PERIODIC, cb, pparam, pname);
}




