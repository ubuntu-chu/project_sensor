/******************************************************************************
******************************************************************************/ 

#include    "drv_storage.h"
#include    "../../../api/log/log.h"

#define		ONE_BYTE_SUBA	                        (1)
#define		TWO_BYTE_SUBA	                        (2)
#define    X_PLUS_BYTE_SUBA	                        	(3)

#define    PAGES_PER_DEVICE			                (128)
#define    BYTES_PER_PAGE                       	(16)

#define    DEVICE_ADDR                             (0xA0)          //定义器件地址


static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag);
static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
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


static portSIZE_TYPE _I2CInnerAccess(uint8 slaAddr, uint8 subAddrType, uint16 subAddr, uint8 *acessAddr, uint32 numbBytes, portuBASE_TYPE accessCtrl)
{
    uint16   i;
    uint16   pageNumbs;
    uint16   accessBytes;
    portSIZE_TYPE  rt 	= numbBytes;

    //判断参数是否合法
    if (0 == numbBytes)
    {
        return 0;
    }
    //计算要操作的页数值
    pageNumbs                                 = (numbBytes/BYTES_PER_PAGE);
    //调整要操作的页数值 余数为零时 表示此时操作的页正好为整页值
    if (0 == (numbBytes%BYTES_PER_PAGE))
    {
        pageNumbs--;
    }
    //判断要操作的页数值是否超出了器件所允许的最大页数值
    if ((subAddr/BYTES_PER_PAGE + pageNumbs) > PAGES_PER_DEVICE)
    {
        return -1;
    }
    //subaddr should be align with page size

    for (i = 0; i <= pageNumbs; i++, subAddr += BYTES_PER_PAGE, acessAddr += BYTES_PER_PAGE)
    {
        //计算要操作的字节数
        if (numbBytes > BYTES_PER_PAGE)
        {
            numbBytes                         -= BYTES_PER_PAGE;
            accessBytes                        = BYTES_PER_PAGE;
        }
        else
        {
            accessBytes                        = numbBytes;
        }
        //I2C实际操作程序段
        {
        	switch (subAddrType){
            //子地址类型判断
        	case ONE_BYTE_SUBA:
                //子地址为单字节
        		t_iic_transfer.m_slaAddr     	= (uint8)(slaAddr);					        //器件的从地址
        		t_iic_transfer.m_subAddr    	    = subAddr;								    //器件子地址
        		t_iic_transfer.m_subAddrCnt	    = 1;								        //器件子地址为1字节
        		break;
        	case TWO_BYTE_SUBA:
        		//子地址为双字节
                t_iic_transfer.m_slaAddr     	= (uint8)(slaAddr);						    //器件的从地址
        		t_iic_transfer.m_subAddr   	 	= subAddr;								    //器件子地址
        		t_iic_transfer.m_subAddrCnt	    = 2;								        //器件子地址为2字节
        		break;
        	case X_PLUS_BYTE_SUBA:
				//子地址结构为8+X   the eight of slaaadr is r/w bit
				t_iic_transfer.m_slaAddr			= (uint8)(slaAddr + ((subAddr >> 7) & 0x0e));	//器件的从地址
        		t_iic_transfer.m_subAddr		    = subAddr & 0x0ff;						    //器件子地址
        		t_iic_transfer.m_subAddrCnt	    = 1;								        //器件子地址为8+x
        		break;
        	default:
        		break;
        	}
            t_iic_transfer.m_paccessAddr         = acessAddr;							    //数据接收缓冲区指针
        	t_iic_transfer.m_accessNumbBytes     = accessBytes;								//要读取的个数
        	t_iic_transfer.m_accessFinished      = FALSE;
            t_iic_transfer.m_accessCtrl          = accessCtrl;
            t_iic_transfer.m_timeout 			 = 800;										//wait for timeout:800 ms

            rt                                   = cpu_iic_transfer(&t_iic_transfer);
            if ((I2C_WRITE == accessCtrl) && (t_iic_transfer.m_accessNumbBytes == rt)){
                delay_ms(25);                                                               //等待器件编程
            }else if (-1 == rt){
                break;
            }
        }
    }

    return rt;
}

static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size){


	return _I2CInnerAccess(DEVICE_ADDR, X_PLUS_BYTE_SUBA, pos, (uint8 *)buffer, size, I2C_WRITE);
}

static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size){
    
	return _I2CInnerAccess(DEVICE_ADDR, X_PLUS_BYTE_SUBA, pos, (uint8 *)buffer, size, I2C_READ);
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
