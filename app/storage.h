#ifndef _APP_STORAGE_
#define _APP_STORAGE_

#include "../includes/includes.h"
#include "devices.h"



class CDevice_storage:public CDevice_base{
public:
    CDevice_storage(const char *pname, uint16 oflag);
    virtual ~CDevice_storage();
private:
    CDevice_storage(const CDevice_storage &other);
    CDevice_storage &operator =(const CDevice_storage &other);
};    
    
    













#endif


