/******************************************************************************
******************************************************************************/ 

#include    "drv_commu.h"
#include    "../../../api/log/log.h"

static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag);
static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
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

static DeviceAbstract  st_Device                            = {
    
        &st_DeviceInfo,
    };

portuBASE_TYPE drv_commuregister(void){
  
    return API_DeviceRegister(&st_Device);
}


static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev){
    
    return DEVICE_OK;
}


static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag){

    DeviceStatus_TYPE   rt = DEVICE_OK;
    
    

    return rt;
}

static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){

    void *pbuf  = const_cast<void *>(buffer);
    
    return uart_tx(UART_0, reinterpret_cast<uint8 *>(pbuf), size);
}

static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
    
	uint16	len;
    //unit:ms 
    uint16  timeout;             
    
    if (pdev->m_openFlag & DEVICE_FLAG_NONBLOCK){
        timeout                         = 0;
    }else {
        timeout                         = 20000; 
    }
    size = static_cast<portSIZE_TYPE>(uart_poll(UART_0, (int8 *)buffer, &len, timeout));
    if (static_cast<portSIZE_TYPE>(-1) == size){
        API_DeviceErrorInfoSet((timeout == 0)?(DEVICE_EAGAIN):(DEVICE_ETIMEOUT));
    }else {
        size                            = len;
    }

    return size;
}

static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args)
{
	DeviceStatus_TYPE   rt = DEVICE_OK;
    
    switch (cmd) {
    
        case COMMU_IOC_RX_ENTER:
            uart_start_rx(UART_0);
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
