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

class CDevice_commu:public CDevice_base{
public:
    CDevice_commu(const char *pname, uint16 oflag, class protocol *pprotocol, package_event_handler handler, void *pvoid);
    virtual ~CDevice_commu();
	portBASE_TYPE package_event_fetch(void);
private:
    CDevice_commu(const CDevice_commu &other);
    CDevice_commu &operator =(const CDevice_commu &other);
	virtual portBASE_TYPE process_read(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size);
	virtual portBASE_TYPE process_write(enum PROC_PHASE phase, char *pbuf, portSIZE_TYPE size);
	portBASE_TYPE package_recv_handle(uint8 event, uint8 func_code, uint8 *pcontinue);
    package_event_handler	m_handler;
    void 					*m_pvoid;

    //pointer to protocol
    class protocol			*m_pprotocol;

};    
    
    













#endif


