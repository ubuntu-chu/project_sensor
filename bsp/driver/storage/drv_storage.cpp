/******************************************************************************
******************************************************************************/ 

#include    "drv_storage.h"
#include    "../../../api/log/log.h"

enum{
    enum_ONE_BYTE_SUBA  = 0,				//һ�ֽڵ�ַ�ṹ
    enum_TWO_BYTE_SUBA,                     //���ֽڵ�ַ�ṹ
    enum_X_PLUS_BYTE_SUBA,			        //8+x ��ַ�ṹ
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
    
        //�豸����
        DEVICE_NAME_STORAGE,
        //�豸����->�ַ��豸
        DEVICE_CLASS_CHAR,
        //�豸��ʶ->ֻ���豸
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
    

    //�жϲ����Ƿ�Ϸ�
    if (0 == numbBytes){
        return 0;
    }
    capacity            = (uint32)t_storage_geometry.m_pages_per_dev*t_storage_geometry.m_bytes_per_page;
    //�жϷ�Χ
    if ((subAddr >= capacity) || ((subAddr+ numbBytes) >capacity)){
        API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
        return -1;
    }
    for (; numbBytes != 0; subAddr += accessBytes, acessAddr += accessBytes)
    {
        offset                                  = subAddr%t_storage_geometry.m_bytes_per_page;
        
        //����Ҫ�������ֽ���
        if ((offset + numbBytes) > t_storage_geometry.m_bytes_per_page){
            accessBytes                        = t_storage_geometry.m_bytes_per_page - offset;
        }else {
            accessBytes                        = numbBytes;
        }
        numbBytes                           -= accessBytes;
        //I2Cʵ�ʲ��������
        {
        	switch (subAddrType){
            //�ӵ�ַ�����ж�
        	case enum_ONE_BYTE_SUBA:
                //�ӵ�ַΪ���ֽ�
        		t_iic_transfer.m_slaAddr     	= (uint8)(slaAddr);					        //�����Ĵӵ�ַ
        		t_iic_transfer.m_subAddr    	    = subAddr;								    //�����ӵ�ַ
        		t_iic_transfer.m_subAddrCnt	    = 1;								        //�����ӵ�ַΪ1�ֽ�
        		break;
        	case enum_TWO_BYTE_SUBA:
        		//�ӵ�ַΪ˫�ֽ�
                t_iic_transfer.m_slaAddr     	= (uint8)(slaAddr);						    //�����Ĵӵ�ַ
        		t_iic_transfer.m_subAddr   	 	= subAddr;								    //�����ӵ�ַ
        		t_iic_transfer.m_subAddrCnt	    = 2;								        //�����ӵ�ַΪ2�ֽ�
        		break;
        	case enum_X_PLUS_BYTE_SUBA:
				//�ӵ�ַ�ṹΪ8+X   the first of slaaadr is r/w bit
				t_iic_transfer.m_slaAddr		= (uint8)(slaAddr + ((subAddr >> 7) & 0x0e));	//�����Ĵӵ�ַ
        		t_iic_transfer.m_subAddr		= subAddr & 0x0ff;						    //�����ӵ�ַ
        		t_iic_transfer.m_subAddrCnt	    = 1;								        //�����ӵ�ַΪ8+x
        		break;
        	default:
                API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
                return -1;
        	}
            t_iic_transfer.m_paccessAddr         = acessAddr;							    //���ݽ��ջ�����ָ��
        	t_iic_transfer.m_accessNumbBytes     = accessBytes;								//Ҫ��ȡ�ĸ���
        	t_iic_transfer.m_accessFinished      = FALSE;
            t_iic_transfer.m_accessCtrl          = accessCtrl;
            t_iic_transfer.m_timeout 			 = 800;										//wait for timeout:800 ms

            rt                                   = cpu_iic_transfer(&t_iic_transfer);
            if ((I2C_WRITE == accessCtrl) && (t_iic_transfer.m_accessNumbBytes == rt)){
                //ÿһҳд��֮�� ��Ҫ�����������  �ڴ��ڼ� ��������ӦѰַ �����յ��Լ���������ַʱ ֱ��NACK
                //delay ms��ʱ��Ӧ��ʵ��Ϊ׼
                delay_ms(25);                                                               //�ȴ��������
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
