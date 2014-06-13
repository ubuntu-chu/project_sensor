/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 

#include    "drv_timer.h"

/******************************************************************************
 *                           ���ļ���̬��������
******************************************************************************/ 

static DeviceStatus_TYPE _drv_init(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_open(pDeviceAbstract pdev, uint16 oflag);
static portSSIZE_TYPE _drv_write(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSSIZE_TYPE _drv_read(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
static DevicePoll_TYPE _drv_poll(pDeviceAbstract pdev);
//static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args);

/******************************************************************************
 *                       ���ļ�������ľ�̬���ݽṹ
******************************************************************************/

static const DeviceAbstractInfo st_DeviceInfo	= {

        //�豸����
        DEVICE_NAME_TIMER,
        //�豸����->�ַ��豸
        DEVICE_CLASS_CHAR,
        //�豸��ʶ->ֻ���豸
        DEVICE_FLAG_RDWR,
        //init
        _drv_init,
        //open
        _drv_open,
        //close
        NULL,
        //read
        _drv_read,
        //write
        _drv_write,
        //control
        NULL,//_drv_ioctl
        //poll
        _drv_poll,
        //rx_indicate
        NULL,
        //tx_complete
        NULL,
    };

static DeviceAbstract  st_Device	= {

        //�豸��Ϣ
        &st_DeviceInfo,
    };

static uint32 	s_time	= 0;

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

portuBASE_TYPE drv_timerregister(void)
{

    return API_DeviceRegister(&st_Device);
}

static DeviceStatus_TYPE _drv_init(pDeviceAbstract pdev)
{
    
    return DEVICE_OK;
}


static DeviceStatus_TYPE _drv_open(pDeviceAbstract pdev, uint16 oflag)
{
    
    return DEVICE_OK;
}


static portSSIZE_TYPE _drv_write(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size)
{
	s_time++;
    
    return sizeof(s_time);
}

static portSSIZE_TYPE _drv_read(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size)
{
	uint32 	*ptr 	= reinterpret_cast<uint32 *>(buffer);

	*ptr	        = s_time;
    s_time         = 0;

    return sizeof(s_time);
}

static DevicePoll_TYPE _drv_poll(pDeviceAbstract pdev)
{
	return (s_time != 0)?(DEVICE_POLLIN|DEVICE_POLLOUT):(DEVICE_POLLOUT);
}

#if 0
static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args)
{
	DeviceStatus_TYPE   rt = DEVICE_OK;
    
    switch (cmd){
    #if 0    
        case PWM_IOC_HEAT_DUTY_CYCLE:
            
            return rt;
    #endif
        default:
            return DEVICE_ECMD_INVALID;
	}

	return rt;
}
#endif

/*********************************************************************************
**                            End Of File
*********************************************************************************/
