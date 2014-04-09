/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "drv_pin.h"

#if		1

/******************************************************************************
 *                           本文件静态函数声明
******************************************************************************/ 

static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag);
//static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);

/******************************************************************************
 *                       本文件所定义的静态数据结构
******************************************************************************/

static const DeviceAbstractInfo st_DeviceInfo_pin = {

        //设备名字
        DEVICE_NAME_PIN,
        //设备类型->字符设备
        DEVICE_CLASS_CHAR,
        //设备标识->只读设备
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
        //rx_indicate
        NULL,
        //tx_complete
        NULL,

    };

static DeviceAbstract  st_Device_pin                            = {

        //设备信息
        &st_DeviceInfo_pin,
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

portuBASE_TYPE drv_pinregister(void){

    return API_DeviceRegister(&st_Device_pin);
}

#define _PIN_VALUE  		((1UL << 4) | (1UL << 5))

static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev){
    
	//p6.4  p6.5    input pin
	P6SEL 		&= ~_PIN_VALUE;
	P6DIR 		&= ~_PIN_VALUE;
	P6REN 		|= _PIN_VALUE;

    return DEVICE_OK;
}


static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag){
    
    
    return DEVICE_OK;
}

#if 0
static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){
    
    
    return size;
}
#endif

static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
  
    uint8   *ptr    = (uint8 *)buffer;
    
    //two pin unpressed
    if ((P6IN & _PIN_VALUE) == _PIN_VALUE){

    	*ptr 	= 0;
    }else {
    	*ptr 	= 1;
    }

    return 1;
}
#endif

/*********************************************************************************
**                            End Of File
*********************************************************************************/
