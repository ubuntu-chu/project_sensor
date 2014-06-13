#ifndef REACTOR_CHANNEL_H
#define REACTOR_CHANNEL_H

#include "../includes/service.h"
#include "event_loop.h"

class channel{
//class channel: noncopyable {
public:
	channel(eventloop* loop,  portDEVHANDLE_TYPE handle);
	~channel();

	int write(const int8 *pbuf, portSIZE_TYPE len);
	int read(class buffer buf, class Timestamp ts);

	int events() const {
		return events_;
	}
	void set_revents(int revt) {
		revents_ = revt;
	} // used by pollers
	// int revents() const { return revents_; }
	bool isNoneEvent() const {
		return events_ == POLLNONE;
	}

	void enableReading() {
		events_ |= POLLIN;
		update();
	}
	void disableReading()
	{
		events_ &= ~POLLIN; update();
	}
	void enableWriting() {
		events_ |= POLLOUT;
		update();
	}
	void disableWriting() {
		events_ &= ~POLLOUT;
		update();
	}
	void disableAll() {
		events_ = POLLNONE;
		update();
	}

	void handleEvent(Timestamp receiveTime);

	portDEVHANDLE_TYPE handle_get(void) { return 	m_handle; }

	list_node_t* list_node_get(void) { return &m_node; }
	int list_node_offset_get(void) { return OFFSET(class channel, m_node); }
	list_node_t* active_list_node_get(void) { return &m_active_node; }
	int active_list_node_offset_get(void) { return OFFSET(class channel, m_active_node); }

	void remove();

private:
	void update();
//	void handleEventWithGuard(Timestamp receiveTime);

	eventloop* loop_;
	int events_;
	int revents_;

	bool eventHandling_;
	portDEVHANDLE_TYPE 		m_handle;
	list_node_t 			m_node;
	list_node_t 			m_active_node;
};

#endif
