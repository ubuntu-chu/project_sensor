#ifndef REACTOR_EVENT_LOOP_H
#define REACTOR_EVENT_LOOP_H

#include "../includes/service.h"
#include "../api/log/log.h"
#include "../api/timestamp.h"
#include "../app/devices.h"

class channel;
class poller;
class timer_queue;

enum timer_ioc{
	enum_TIMER_IOC_RESTART	= 0,
	enum_TIMER_IOC_RESTART_WITH_TIME,
	enum_TIMER_IOC_STOP,
	enum_TIMER_IOC_UNREGISTER,
};

class eventloop: noncopyable {
public:
	eventloop();
	~eventloop();

	void loop();
	void quit();
	portBASE_TYPE run_inloop(class event *event);

	bool run_after(uint32 ms, fp_void_pvoid *cb, void *pparam, const char* pname);
    bool run_after(struct tm &tm, fp_void_pvoid *cb, void *pparam, const char* pname);
    timer_handle_type run_every(uint32 ms, fp_void_pvoid *cb, void *pparam, const char* pname);
	timer_handle_type run_every(struct tm &tm, fp_void_pvoid *cb, void *pparam, const char* pname);
	portBASE_TYPE timer_ioctl(timer_handle_type handle, enum timer_ioc ioc, void *pparam);

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

	channel 				*m_pchannel_event;						//�¼�   Ӧ�ÿ�����event loopע���¼�
	list_head_t 			m_list_event;							//�¼��б�ͷ

	timer_queue 			*m_ptimer_queue;						//��ʱ������

	device					*m_device_event;
};











/******************************************************************************
 *                             END  OF  FILE
******************************************************************************/
#endif
