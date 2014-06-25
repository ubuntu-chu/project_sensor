/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 

#include    "drv_pin.h"

#if		1

/******************************************************************************
 *                           ���ļ���̬��������
******************************************************************************/ 

static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag);
//static portSSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
static DevicePoll_TYPE _drv_poll(pDeviceAbstract pdev);

/******************************************************************************
 *                       ���ļ�������ľ�̬���ݽṹ
******************************************************************************/

static const DeviceAbstractInfo st_DeviceInfo_pin = {

        //�豸����
        DEVICE_NAME_PIN,
        //�豸����->�ַ��豸
        DEVICE_CLASS_CHAR,
        //�豸��ʶ->ֻ���豸
        DEVICE_FLAG_RDONLY,
        //init
        _drv_devinit,
        //open
        _drv_devopen,
        //close
        NULL,
        //repin
        _drv_devread,
        //write
        NULL,
        //control
        NULL,
        //poll
        _drv_poll,
        //rx_indicate
        NULL,
        //tx_complete
        NULL,

    };

static DeviceAbstract  st_Device_pin                            = {

        //�豸��Ϣ
        &st_DeviceInfo_pin,
    };

/******************************************************************************
 *  �������� :
 *
 *  �������� : �豸ע��
 *
 *  ��ڲ��� :
 *
 *  ���ڲ��� :
 *
 *  �� д �� :
 *
 *  ��    �� :
 *
 *  �� �� �� :
 *
 *  ��    �� :
 *
 *  ��    ע :
 *
 *
******************************************************************************/

portuBASE_TYPE drv_pinregister(void){

    return API_DeviceRegister(&st_Device_pin);
}

static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev){
    

    return DEVICE_OK;
}


static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag){
    
    
    return DEVICE_OK;
}

#if 0
static portSSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){
    
    
    return size;
}
#endif

static portSSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
  
    uint8   *ptr    = (uint8 *)buffer;
    ptr = ptr;
    
    return 1;
}
#endif

static DevicePoll_TYPE _drv_poll(pDeviceAbstract pdev)
{
	return DEVICE_POLLIN;
//	return DEVICE_POLLNONE;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
