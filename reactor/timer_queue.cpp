#include "timer_queue.h"
#include "channel.h"
#include "../app/timer.h"
#include "../app/devices.h"


static class channel *timer_create(eventloop *loop)
{
    static device_timer 		t_device_timer;
    static channel  timer(loop, &t_device_timer);

    t_device_timer.open(DEVICE_FLAG_RDWR);

    return &timer;
}

timer_queue::timer_queue()
{
}

timer_queue::~timer_queue()
{
}

timer_queue 	t_timer_queue;

timer_queue* timer_queue::new_timerqueue(eventloop* loop)
{
	t_timer_queue.m_pchannel_timer = timer_create(loop);
	t_timer_queue.m_pchannel_timer->enableReading();
	t_timer_queue.m_pchannel_timer->event_handle_register(&timer_queue::event_handle, &t_timer_queue);

	return &t_timer_queue;
}


timer_handle_type timer_queue::timer_add(const uint32& ms, uint8 flag, fp_void_pvoid *cb, void *pparam, const char* pname)
{
	timer_handle_type	handle;

	handle 	= t_timer_manage.soft_timer_register(ms, flag, cb, pparam, pname);
    if (handle!= (timer_handle_type)-1){
    	return handle;
    }
    t_timer_manage.timer_start(handle);

    return handle;
}

int timer_queue::timer_cancel(timer_handle_type handle)
{
	return t_timer_manage.timer_stop(handle);
}

int timer_queue::event_handle(void *pvoid, int event_type, class buffer &buf, class Timestamp &ts)
{
	timer_queue *ptimer_queue      = static_cast<timer_queue *> (pvoid);
    ptimer_queue    = ptimer_queue;

	//timer  readable
	if (event_type == POLLIN){

		t_timer_manage.soft_timer_handle();
	}

	return 0;
}







