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

enum {
    enum_NONE_DONE  = 0x00,
    enum_TX_DONE    = 0x01,
    enum_RX_DONE    = 0x02,
};

volatile uint8 s_commu_done_flg  = enum_NONE_DONE;
monitor_handle_type s_handle_commu;

portuBASE_TYPE drv_commuregister(void){
  
    return API_DeviceRegister(&st_Device);
}


static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev){

    //等待时间2s
    s_handle_commu = t_monitor_manage.monitor_register(2000, enum_MODE_ONESHOT, NULL, NULL);
    
    return DEVICE_OK;
}


static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag){

    DeviceStatus_TYPE   rt = DEVICE_OK;
    
    

    return rt;
}

static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){


    
    
    return size;
}

static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
    
	uint16	len;
	portBASE_TYPE 	nonblock 				= 0;
    
    if (pdev->m_openFlag & DEVICE_FLAG_NONBLOCK){
         nonblock 							= 1;
    }else {
         t_monitor_manage.monitor_start(s_handle_commu);
    }
    while (1){
		if (0 == net_queue_get((uint8 *)buffer, &len)){
			size 						= len;
			break;
		}
		if (1 == nonblock){
            size 						= (portSIZE_TYPE)-1;
            API_DeviceErrorInfoSet(DEVICE_EAGAIN);
			break;
		}else if (t_monitor_manage.monitor_expired(s_handle_commu)){
			//timeout occure
			size 						= (portSIZE_TYPE)-1;
            API_DeviceErrorInfoSet(DEVICE_ETIMEOUT);
			break;
		}
	}
	if (0 == nonblock){
		t_monitor_manage.monitor_start(s_handle_commu);
	}
    s_commu_done_flg   = enum_NONE_DONE;

    return size;
}

static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args)
{
	DeviceStatus_TYPE   rt = DEVICE_OK;
    
    switch (cmd) {

	default:
		rt                 = DEVICE_ECMD_INVALID;
		break;
	}

	return rt;
}





/*********************************************************************************
**                            End Of File
*********************************************************************************/
