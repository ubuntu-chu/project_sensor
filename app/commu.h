#ifndef _APP_COMMU_H_
#define _APP_COMMU_H_

#include "../includes/includes.h"
#include "devices.h"
#include "protocol.h"

enum{

	EVENT_ACK	= 0,
	EVENT_CMD,
};

typedef portBASE_TYPE (*package_event_handler)(void *pvoid, class protocol_info *pinfo);

class device_commu:public device{
public:
    device_commu(class protocol *pprotocol);
    virtual ~device_commu();
	portBASE_TYPE package_event_fetch(void);
    virtual enum PROC_CMD_STAT process_command(enum PROC_CMD dir, enum PROC_PHASE phase, struct device_buffer &device_buffer);
private:
    device_commu(const device_commu &other);
    device_commu &operator =(const device_commu &other);
	portBASE_TYPE package_recv_handle(uint8 event, uint8 func_code, uint8 *pcontinue);

    //pointer to protocol
    class protocol			*m_pprotocol;

};    
    
    













#endif


