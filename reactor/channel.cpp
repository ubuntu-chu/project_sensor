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

void channel::handleEvent(Timestamp &receiveTime)
{
    if (NULL != m_handle_cb){
        int8    				data[100];
        portSSIZE_TYPE 			rt_len;
		class buffer 			buffer(data, sizeof(data));
        
        eventHandling_ = true;

        if (revents_ & (POLLIN)) {
        	rt_len 				= m_handle->read(0, buffer.peek(), sizeof(data));
        	buffer.hasWritten(rt_len);
			m_handle_cb(m_pvoid, POLLIN, buffer, receiveTime);
        }
        if (revents_ & POLLOUT) {
            
            m_handle_cb(m_pvoid, POLLOUT, buffer, receiveTime);
        }
        eventHandling_ = false;
    }
}

