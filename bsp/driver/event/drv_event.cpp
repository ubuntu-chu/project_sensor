/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "drv_event.h"

/******************************************************************************
 *                           本文件静态函数声明
******************************************************************************/ 

static DeviceStatus_TYPE _drv_init(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_open(pDeviceAbstract pdev, uint16 oflag);
static portSSIZE_TYPE _drv_write(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSSIZE_TYPE _drv_read(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
static DeviceStatus_TYPE _drv_poll(pDeviceAbstract pdev);
//static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args);

/******************************************************************************
 *                       本文件所定义的静态数据结构
******************************************************************************/

static const DeviceAbstractInfo st_DeviceInfo	= {

        //设备名字
        DEVICE_NAME_EVENT,
        //设备类型->字符设备
        DEVICE_CLASS_CHAR,
        //设备标识->只读设备
        DEVICE_FLAG_RDONLY,
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

        //设备信息
        &st_DeviceInfo,
    };

/******************************************************************************
 *  函数名称 :
 *
 *  函数功能 : 设备注册
 *
 *  入口参数 :
 *
 *  出口参数 :
 *
 *  编 写 者 :
 *
 *  日    期 :
 *
 *  审 核 者 :
 *
 *  日    期 :
 *
 *  附    注 :
 *
 *
******************************************************************************/

portuBASE_TYPE drv_eventregister(void)
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
    
    
    return size;
}

static portSSIZE_TYPE _drv_read(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size)
{
  
    //uint8   *ptr    = (uint8 *)buffer;

    return 1;
}

static DeviceStatus_TYPE _drv_poll(pDeviceAbstract pdev)
{
	return DEVICE_POLLIN;
//	return DEVICE_POLLNONE;
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
