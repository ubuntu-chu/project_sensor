#include "poller.h"
#include "channel.h"

class poller  t_poller;

poller::poller()
{
	ownerLoop_ 		= NULL;
	list_init(&m_channels);
}

poller::~poller()
{
}

poller* poller::newDefaultPoller(eventloop* loop)
{
	t_poller.eventloop_set(loop);

	return &t_poller;
}

extern DeviceStatus_TYPE hal_poll(pDeviceAbstract pdev);

//poller  eventloop  share channels
Timestamp poller::poll(int timeoutMs, list_head_t* activeChannels)
{
	Timestamp now(Timestamp::now());
	channel 	*it;
	list_node_t 	*pos;
	int			rt;

	list_for_each(pos, &m_channels){

		it		= list_entry_offset(pos, class channel, it->list_node_offset_get());
		rt 		= hal_poll(it->handle_get());

		if ((DEVICE_POLLIN == rt) && (it->events() & POLLIN)){
			it->set_revents(POLLIN);
			list_insert_after(activeChannels, it->active_list_node_get());
		}else if ((DEVICE_POLLOUT == rt) && (it->events() & POLLOUT)){
			it->set_revents(POLLOUT);
			list_insert_after(activeChannels, it->active_list_node_get());
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


