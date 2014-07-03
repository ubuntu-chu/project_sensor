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
    if (handle == (timer_handle_type)-1){
    	return handle;
    }
    t_timer_manage.timer_start(handle);

    return handle;
}

int timer_queue::timer_cancel(timer_handle_type handle)
{
	return t_timer_manage.timer_stop(handle);
}

portBASE_TYPE timer_queue::timer_ioctl(timer_handle_type handle, enum timer_ioc ioc, void *pparam)
{
	portBASE_TYPE	rt	= 0;

	switch (ioc){
	case enum_TIMER_IOC_RESTART:

		rt 	= t_timer_manage.timer_restart(handle);
		break;

	case enum_TIMER_IOC_RESTART_WITH_TIME:

		rt 	= t_timer_manage.timer_restart(handle, reinterpret_cast<uint32>(pparam));
		break;
    
    case enum_TIMER_IOC_TIMEOUT:

		rt 	= t_timer_manage.timer_timeout(handle, reinterpret_cast<uint32>(pparam));
		break;

	case enum_TIMER_IOC_STOP:

		rt 	= t_timer_manage.timer_stop(handle);
		break;

	case enum_TIMER_IOC_UNREGISTER:

		rt 	= t_timer_manage.timer_unregister(handle);
		break;

	default:
		rt	= -1;
		break;
	}

	return rt;
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







