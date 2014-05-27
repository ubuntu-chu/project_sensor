#ifndef _DRV_STORAGE_H_
#define _DRV_STORAGE_H_


#ifndef    _INCLUDES_H_
	#include    "../../../includes/includes.h"
#endif


#define        DEVICE_NAME_STORAGE                                  ("STORAGE")

//设备几何信息
struct storage_geometry{
    uint8           m_dev_addr;                         //定义器件地址
    uint8           m_suba_type;                        //子地址类型
    uint16          m_bytes_per_page;                   //每一页的尺寸
    uint16          m_pages_per_dev;                    //器件一共有多少页
};



extern portuBASE_TYPE drv_storageregister(void);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif
