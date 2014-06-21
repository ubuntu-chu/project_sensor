#ifndef REACTOR_EVENT_LOOP_H
#define REACTOR_EVENT_LOOP_H

#include "../includes/service.h"
#include "../api/log/log.h"
#include "../api/timestamp.h"
#include "../app/devices.h"

class channel;
class poller;
class timer_queue;

class eventloop: noncopyable {
public:
	eventloop();
	~eventloop();

	void loop();
	void quit();
	portBASE_TYPE run_inloop(class event *event);

	timer_handle_type run_after(uint32 ms, fp_void_pvoid *cb, void *pparam, const char* pname);
    timer_handle_type run_after(struct tm &tm, fp_void_pvoid *cb, void *pparam, const char* pname);
    timer_handle_type run_every(uint32 ms, fp_void_pvoid *cb, void *pparam, const char* pname);
	timer_handle_type run_every(struct tm &tm, fp_void_pvoid *cb, void *pparam, const char* pname);

	void update_channel(channel* channel);
	void remove_channel(channel* channel);

private:

	timer_handle_type run_at(const uint32& ms, uint8 flag, fp_void_pvoid *cb, void *pparam, const char* pname);
	static int event_handle(void *pvoid, int event_type, class buffer &buf, class Timestamp &ts);
	bool 					looping_; /* atomic */
	bool 					eventHandling_; /* atomic */
	bool 					callingPendingFunctors_; /* atomic */
	Timestamp 				pollReturnTime_;

	list_head_t 			m_active_channels;
	channel* 				m_current_acitve_channel;

	poller					*m_ppoller;

	channel 				*m_pchannel_event;						//事件   应用可以向event loop注入事件
	list_head_t 			m_list_event;							//事件列表头

	timer_queue 			*m_ptimer_queue;						//定时器队列

	device					*m_device_event;
};











/******************************************************************************
 *                             END  OF  FILE
******************************************************************************/
#endif
