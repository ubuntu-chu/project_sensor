#include "poller.h"
#include "channel.h"

static class poller  t_poller;

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


	return now;
}


void poller::updateChannel(class channel* channel)
{
	if (NULL == list_find(&m_channels, &channel->m_node)){
		list_insert_after(&m_channels, &channel->m_node);
	}
}

void poller::removeChannel(channel* channel)
{
	if (NULL != list_find(&m_channels, &channel->m_node)){
		list_del(&channel->m_node);
	}
}


