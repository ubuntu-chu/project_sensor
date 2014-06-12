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
		m_ppoller(poller::newDefaultPoller(this))
{

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

