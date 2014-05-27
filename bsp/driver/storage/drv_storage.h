#ifndef _DRV_STORAGE_H_
#define _DRV_STORAGE_H_


#ifndef    _INCLUDES_H_
	#include    "../../../includes/includes.h"
#endif


#define        DEVICE_NAME_STORAGE                                  ("STORAGE")

//�豸������Ϣ
struct storage_geometry{
    uint8           m_dev_addr;                         //����������ַ
    uint8           m_suba_type;                        //�ӵ�ַ����
    uint16          m_bytes_per_page;                   //ÿһҳ�ĳߴ�
    uint16          m_pages_per_dev;                    //����һ���ж���ҳ
};



extern portuBASE_TYPE drv_storageregister(void);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif
