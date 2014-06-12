#include "event_loop.h"
#include "poller.h"

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


}

void eventloop::updateChannel(channel* channel) {
	m_ppoller->updateChannel(channel);
}

void eventloop::removeChannel(channel* channel) {
	m_ppoller->removeChannel(channel);
}

