#ifndef REACTOR_CHANNEL_H
#define REACTOR_CHANNEL_H

#include "../includes/service.h"
#include "event_loop.h"


typedef int (handle_channel_cb)(void *pvoid, int event_type, class buffer &buf, class Timestamp &ts);

class channel{
//class channel: noncopyable {
public:
	channel(eventloop* loop,  class device *handle);
	~channel();

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

	void event_handle_register(handle_channel_cb *cb, void *pvoid)
	{
		m_handle_cb						= cb;
		m_pvoid							= pvoid;
	}
	void handleEvent(Timestamp receiveTime);

	class device *device_get(void) { return 	m_handle; }

	list_node_t* list_node_get(void) { return &m_node; }
	static int list_node_offset_get(void) { return OFFSET(class channel, m_node); }
	list_node_t* active_list_node_get(void) { return &m_active_node; }
	static int active_list_node_offset_get(void) { return OFFSET(class channel, m_active_node); }

	void remove();

private:
	void update();

	eventloop* loop_;
	int events_;
	int revents_;

	handle_channel_cb 		*m_handle_cb;
	void 					*m_pvoid;
	bool eventHandling_;
	class device 			*m_handle;
	list_node_t 			m_node;
	list_node_t 			m_active_node;
};

#endif

