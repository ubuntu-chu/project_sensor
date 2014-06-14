#ifndef REACTOR_TIMER_QUEUE_H
#define REACTOR_TIMER_QUEUE_H

#include "../includes/service.h"
#include "event_loop.h"


class timer_queue
 {
public:
	timer_queue();
	~timer_queue();

	timer_handle_type timer_add(const uint32& ms, uint8 flag, fp_void_pvoid *cb, void *pparam, const char* pname);
	int timer_cancel(timer_handle_type handle);
private:

	channel 				*m_pchannel_timer;						//channel timer

	eventloop* loop_;
	bool callingExpiredTimers_; /* atomic */
};



#endif

