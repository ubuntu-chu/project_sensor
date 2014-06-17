#ifndef REACTOR_POLLER_H
#define REACTOR_POLLER_H

#include "../includes/service.h"
#include "event_loop.h"

class channel;

class poller: noncopyable {
public:
	poller();
	~poller();

	void eventloop_set(eventloop* loop) { loop_ 	= loop; }

	/// Polls the I/O events.
	/// Must be called in the loop thread.
	Timestamp poll(int timeoutMs, list_head_t* activeChannels);

	/// Changes the interested I/O events.
	/// Must be called in the loop thread.
	void updateChannel(channel* channel);

	/// Remove the channel, when it destructs.
	/// Must be called in the loop thread.
	void removeChannel(channel* channel);

	static poller* newDefaultPoller(eventloop* loop);

private:
	eventloop			*loop_;
	list_head_t 		m_channels;
	timer_handle_type 	m_handle_timer;
};



/******************************************************************************
 *                             END  OF  FILE
******************************************************************************/
#endif
