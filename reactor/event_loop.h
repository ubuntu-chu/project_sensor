#ifndef REACTOR_EVENT_LOOP_H
#define REACTOR_EVENT_LOOP_H

#include "../includes/service.h"
#include "../api/timestamp.h"

class channel;
class poller;
//class TimerQueue;

class eventloop: noncopyable {
public:
	eventloop();
	~eventloop();  // force out-line dtor, for scoped_ptr members.

	///
	/// Loops forever.
	///
	/// Must be called in the same thread as creation of the object.
	///
	void loop();

	void quit();

	void updateChannel(channel* channel);
	void removeChannel(channel* channel);

private:

	bool looping_; /* atomic */
	bool quit_; /* atomic */
	bool eventHandling_; /* atomic */
	bool callingPendingFunctors_; /* atomic */
	Timestamp pollReturnTime_;

	list_head_t m_active_channels;
	channel* 	m_current_acitve_channel;

	poller		*m_ppoller;
};











/******************************************************************************
 *                             END  OF  FILE
******************************************************************************/
#endif
