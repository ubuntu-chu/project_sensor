/******************************************************************************
******************************************************************************/ 

#include    "drv_storage.h"
#include    "../../../api/log/log.h"

enum{
    enum_ONE_BYTE_SUBA  = 0,				//一字节地址结构
    enum_TWO_BYTE_SUBA,                     //两字节地址结构
    enum_X_PLUS_BYTE_SUBA,			        //8+x 地址结构
    enum_MAX_BYTE_SUBA,
};

struct storage_geometry t_storage_geometry = {
    0xa0,
    enum_X_PLUS_BYTE_SUBA,
    16,
    128,
};

static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag);
static portSSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
static DeviceStatus_TYPE _drv_ioctl(pDeviceAbstract pdev, uint8 cmd, void *args);

static const DeviceAbstractInfo st_DeviceInfo = {
    
        //设备名字
        DEVICE_NAME_STORAGE,
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

portuBASE_TYPE drv_storageregister(void){
  
    return API_DeviceRegister(&st_Device);
}


static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev){

    
    return DEVICE_OK;
}


static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag){

    DeviceStatus_TYPE   rt = DEVICE_OK;
    
	cpu_iic_init(enum_NUMB_IIC0);
    return rt;
}


static portSSIZE_TYPE _i2c_access(uint8 slaAddr, uint8 subAddrType, uint16 subAddr, uint8 *acessAddr, uint32 numbBytes, portuBASE_TYPE accessCtrl)
{
    uint32   capacity;
    uint16   accessBytes;
    uint16   offset;
    portSSIZE_TYPE  rt;
    portSSIZE_TYPE  tot_access_bytes   = numbBytes;
    

    //判断参数是否合法
    if (0 == numbBytes){
        return 0;
    }
    capacity            = (uint32)t_storage_geometry.m_pages_per_dev*t_storage_geometry.m_bytes_per_page;
    //判断范围
    if ((subAddr >= capacity) || ((subAddr+ numbBytes) >capacity)){
        API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
        return -1;
    }
    for (; numbBytes != 0; subAddr += accessBytes, acessAddr += accessBytes)
    {
        offset                                  = subAddr%t_storage_geometry.m_bytes_per_page;
        
        //计算要操作的字节数
        if ((offset + numbBytes) > t_storage_geometry.m_bytes_per_page){
            accessBytes                        = t_storage_geometry.m_bytes_per_page - offset;
        }else {
            accessBytes                        = numbBytes;
        }
        numbBytes                           -= accessBytes;
        //I2C实际操作程序段
        {
        	switch (subAddrType){
            //子地址类型判断
        	case enum_ONE_BYTE_SUBA:
                //子地址为单字节
        		t_iic_transfer.m_slaAddr     	= (uint8)(slaAddr);					        //器件的从地址
        		t_iic_transfer.m_subAddr    	    = subAddr;								    //器件子地址
        		t_iic_transfer.m_subAddrCnt	    = 1;								        //器件子地址为1字节
        		break;
        	case enum_TWO_BYTE_SUBA:
        		//子地址为双字节
                t_iic_transfer.m_slaAddr     	= (uint8)(slaAddr);						    //器件的从地址
        		t_iic_transfer.m_subAddr   	 	= subAddr;								    //器件子地址
        		t_iic_transfer.m_subAddrCnt	    = 2;								        //器件子地址为2字节
        		break;
        	case enum_X_PLUS_BYTE_SUBA:
				//子地址结构为8+X   the first of slaaadr is r/w bit
				t_iic_transfer.m_slaAddr		= (uint8)(slaAddr + ((subAddr >> 7) & 0x0e));	//器件的从地址
        		t_iic_transfer.m_subAddr		= subAddr & 0x0ff;						    //器件子地址
        		t_iic_transfer.m_subAddrCnt	    = 1;								        //器件子地址为8+x
        		break;
        	default:
                API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
                return -1;
        	}
            t_iic_transfer.m_paccessAddr         = acessAddr;							    //数据接收缓冲区指针
        	t_iic_transfer.m_accessNumbBytes     = accessBytes;								//要读取的个数
        	t_iic_transfer.m_accessFinished      = FALSE;
            t_iic_transfer.m_accessCtrl          = accessCtrl;
            t_iic_transfer.m_timeout 			 = 800;										//wait for timeout:800 ms

            rt                                   = cpu_iic_transfer(&t_iic_transfer);
            if ((I2C_WRITE == accessCtrl) && (t_iic_transfer.m_accessNumbBytes == rt)){
                //每一页写完之后 都要进行器件编程  在此期间 器件不响应寻址 当接收到自己的器件地址时 直接NACK
                //delay ms的时间应以实际为准
                delay_ms(25);                                                               //等待器件编程
            }else if (-1 == rt){
                API_DeviceErrorInfoSet(DEVICE_ETIMEOUT);
                break;
            }
        }
    }

    return tot_access_bytes-numbBytes;
}

static portSSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){


	return _i2c_access(t_storage_geometry.m_dev_addr, t_storage_geometry.m_suba_type, 
                            pos, (uint8 *)buffer, size, I2C_WRITE);
}

static portSSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
    
	return _i2c_access(t_storage_geometry.m_dev_addr, t_storage_geometry.m_suba_type, 
                            pos, (uint8 *)buffer, size, I2C_READ);
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
