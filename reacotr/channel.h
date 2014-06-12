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
		return events_ == kNoneEvent;
	}

	void enableReading() {
		events_ |= kReadEvent;
		update();
	}
	// void disableReading() { events_ &= ~kReadEvent; update(); }
	void enableWriting() {
		events_ |= kWriteEvent;
		update();
	}
	void disableWriting() {
		events_ &= ~kWriteEvent;
		update();
	}
	void disableAll() {
		events_ = kNoneEvent;
		update();
	}
	bool isWriting() const {
		return events_ & kWriteEvent;
	}

	eventloop* ownerLoop() {
		return loop_;
	}
	void remove();

	list_node_t 			m_node;
private:
	void update();
//	void handleEventWithGuard(Timestamp receiveTime);

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;

	eventloop* loop_;
	int events_;
	int revents_;

	bool eventHandling_;
	portDEVHANDLE_TYPE 		m_handle;
};

#endif

