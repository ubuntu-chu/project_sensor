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
	m_handle_cb 		= NULL;
}

channel::~channel()
{
}

void channel::update()
{
  loop_->update_channel(this);
}

void channel::remove()
{
  loop_->remove_channel(this);
}

void channel::handleEvent(Timestamp receiveTime) 
{
    if (NULL != m_handle_cb){
        int8    data[100];
        class buffer 	buf(data, sizeof(data)/sizeof(data[0]));
        
        eventHandling_ = true;

        if (revents_ & (POLLIN)) {
            //

            m_handle_cb(m_pparam, POLLIN, buf, receiveTime);
        }
        if (revents_ & POLLOUT) {
            m_handle_cb(m_pparam, POLLOUT, buf, receiveTime);
        }
        eventHandling_ = false;
    }
}

