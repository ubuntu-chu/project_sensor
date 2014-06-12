#include "channel.h"


const int channel::kNoneEvent = 0;
const int channel::kReadEvent = POLLIN;
const int channel::kWriteEvent = POLLOUT;

channel::channel(eventloop* loop,  portDEVHANDLE_TYPE handle)
  : loop_(loop),
    events_(0),
    revents_(0),
    eventHandling_(false)
{
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
