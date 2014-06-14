#include "timer_queue.h"

timer_queue 	t_timer_queue;

timer_queue::timer_queue()
{
	loop_ 					= NULL;
}

timer_queue::~timer_queue()
{
}

timer_queue* timer_queue::new_timerqueue(eventloop* loop)
{
	t_timer_queue.eventloop_set(loop);

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









