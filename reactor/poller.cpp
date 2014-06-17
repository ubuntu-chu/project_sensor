#include "poller.h"
#include "channel.h"
#include "../app/devices.h"

class poller  t_poller;

poller::poller()
{
	loop_ 		= NULL;
	list_init(&m_channels);
}

poller::~poller()
{
}

poller* poller::newDefaultPoller(eventloop* loop)
{
	t_poller.eventloop_set(loop);
	t_poller.m_handle_timer 			= t_timer_manage.hard_timer_register(1000,
                                                SV_TIMER_FLAG_ONE_SHOT,
                                                NULL,
                                                NULL,
                                                "poller");
    ASSERT(t_poller.m_handle_timer != (timer_handle_type)-1);

	return &t_poller;
}

//poller  eventloop  share channels
Timestamp poller::poll(int timeoutMs, list_head_t* activeChannels)
{
	Timestamp now(Timestamp::now());
	channel 			*it;
	list_node_t 		*pos;
	class device		*pdevice;
	DevicePoll_TYPE		rt;
	bool 				event_occured;
	bool				poller_exit	= false;

	t_timer_manage.timer_start(m_handle_timer, timeoutMs);

	while ((false == poller_exit) && (!t_timer_manage.timer_expired(m_handle_timer))) {
		list_for_each(pos, &m_channels)
		{
			event_occured = false;
			it = list_entry_offset(pos, class channel, channel::list_node_offset_get());
			pdevice = it->device_get();
			rt = pdevice->poll();

			if (rt > DEVICE_POLLNONE) {
				if ((DEVICE_POLLIN & rt) && (it->events() & POLLIN)) {
					event_occured = true;
					it->set_revents(POLLIN);
				} else if ((DEVICE_POLLOUT & rt) && (it->events() & POLLOUT)) {
					event_occured = true;
					it->set_revents(POLLOUT);
				}
				if (event_occured == true) {
					list_insert_after(activeChannels, it->active_list_node_get());
					poller_exit					= true;
				}
			}
		}
	}

	return now;
}


void poller::updateChannel(class channel* channel)
{
	if (NULL == list_find(&m_channels, channel->list_node_get())){
		list_insert_after(&m_channels, channel->list_node_get());
	}
}

void poller::removeChannel(channel* channel)
{
	if (NULL != list_find(&m_channels, channel->list_node_get())){
		list_del(channel->list_node_get());
	}
}


