/******************************************************************************
******************************************************************************/ 

#include    "drv_commu.h"
#include    "../../../api/log/log.h"

static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag);
static portSSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args);

static const DeviceAbstractInfo st_DeviceInfo = {
    
        //设备名字
        DEVICE_NAME_COMMU,
        //设备类型->字符设备
        DEVICE_CLASS_CHAR,
        //设备标识->只读设备
        DEVICE_FLAG_RDWR,
        //init
        _drv_devinit,
        //open
        _drv_devopen,
        //close
        NULL,
        //read
        _drv_devread, 
        //write
        _drv_devwrite, 
        //control
        _drv_ioctl,
        //rx_indicate
        NULL,
        //tx_complete
        NULL,
        
    };

static DeviceAbstract  st_Device                          = {
    
        &st_DeviceInfo,
    };

portuBASE_TYPE drv_commuregister(void){
  
    return API_DeviceRegister((DeviceAbstract *)&st_Device);
}


static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev){
    
    uart_ctl_init();
	uart_init(UART_0, B9600);
    return DEVICE_OK;
}


static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag){

    DeviceStatus_TYPE   rt = DEVICE_OK;
    
    _drv_ioctl(pdev, COMMU_IOC_RX_ENTER, NULL);
    uart_enableIRQ(UART_0);
    return rt;
}

static portSSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){

    void *pbuf  = const_cast<void *>(buffer);
    portSSIZE_TYPE  rt = uart_tx(UART_0, reinterpret_cast<uint8 *>(pbuf), size);
    return rt;
}

static portSSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
    
	uint16	len;
    //unit:ms 
    uint16  timeout; 
    portSSIZE_TYPE      rt;  
    
    if (pdev->m_openFlag & DEVICE_FLAG_NONBLOCK){
        timeout                         = 0;
    }else {
        timeout                         = 20000; 
    }
    rt = static_cast<portSIZE_TYPE>(uart_poll(UART_0, (int8 *)buffer, &len, timeout));
    if (static_cast<portSIZE_TYPE>(-1) == rt){
        API_DeviceErrorInfoSet(DEVICE_ETIMEOUT);
    }if (static_cast<portSIZE_TYPE>(0) == rt){
        API_DeviceErrorInfoSet(DEVICE_EAGAIN);
        rt                              = -1;
    }else {
        rt                              = len;
    }

    return rt;
}

static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args)
{
	DeviceStatus_TYPE   rt = DEVICE_OK;
    
    switch (cmd) {
    
        case COMMU_IOC_RX_ENTER:
            uart_start_rx(UART_0);
            break;

        case COMMU_IOC_BAUD:
        {
        	uint32 	baud_array[] = {B2400, B4800, B9600, B19200, B38400};
        	uint32 	index 	= reinterpret_cast<uint32>(reinterpret_cast<uint32 *>(args));

			uart_ios(UART_0, baud_array[index]);
        }
            break;

        default:
            rt                 = DEVICE_ECMD_INVALID;
            break;
	}

	return rt;
}





/*********************************************************************************
**                            End Of File
*********************************************************************************/
