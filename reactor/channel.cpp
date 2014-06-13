#include "channel.h"

channel::channel(eventloop* loop, class device *handle)
  : loop_(loop),
    events_(POLLNONE),
    revents_(POLLNONE),
    eventHandling_(false),
    m_handle(handle)
{
	list_init(&m_node);
	list_init(&m_active_node);
}

channel::~channel()
{
}

void channel::update()
{
  loop_->updateChannel(this);
}

void channel::remove()
{
  loop_->removeChannel(this);
}

int channel::write(const int8 *pbuf, portSIZE_TYPE len)
{
	return 0;
}

int channel::read(class buffer buf, class Timestamp ts)
{
	return 0;
}

void channel::handleEvent(Timestamp receiveTime) {
	eventHandling_ = true;

	if (revents_ & (POLLIN)) {

	}
	if (revents_ & POLLOUT) {

	}
	eventHandling_ = false;
}

