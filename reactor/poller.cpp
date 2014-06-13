#include "poller.h"
#include "channel.h"
#include "../app/devices.h"

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

//poller  eventloop  share channels
Timestamp poller::poll(int timeoutMs, list_head_t* activeChannels)
{
	Timestamp now(Timestamp::now());
	channel 			*it;
	list_node_t 		*pos;
	class device		*pdevice;
	DevicePoll_TYPE		rt;
	int 				event_occured;

	list_for_each(pos, &m_channels){

		event_occured 				= false;
		it							= list_entry_offset(pos, class channel, it->list_node_offset_get());
		pdevice 					= it->device_get();
		rt 							= pdevice->poll();

		if (rt > DEVICE_POLLNONE){
			if ((DEVICE_POLLIN == rt) && (it->events() & POLLIN)){
				event_occured		= true;
				it->set_revents(POLLIN);
			}else if ((DEVICE_POLLOUT == rt) && (it->events() & POLLOUT)){
				event_occured		= true;
				it->set_revents(POLLOUT);
			}
			if (event_occured == true){
				list_insert_after(activeChannels, it->active_list_node_get());
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


