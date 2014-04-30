/******************************************************************************
******************************************************************************/ 

#include    "drv_storage.h"
#include    "../../../api/log/log.h"

#define		ONE_BYTE_SUBA	                        (1)
#define		TWO_BYTE_SUBA	                        (2)
#define    X_PLUS_BYTE_SUBA	                        	(3)

#define    PAGES_PER_DEVICE			                (128)
#define    BYTES_PER_PAGE                       	(16)

#define    DEVICE_ADDR                             (0xA0)          //����������ַ


static DeviceStatus_TYPE _drv_devinit(pDeviceAbstract pdev);
static DeviceStatus_TYPE _drv_devopen(pDeviceAbstract pdev, uint16 oflag);
static portSIZE_TYPE _drv_devwrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
static portSIZE_TYPE _drv_devread(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
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


static portSIZE_TYPE _I2CInnerAccess(uint8 slaAddr, uint8 subAddrType, uint16 subAddr, uint8 *acessAddr, uint32 numbBytes, portuBASE_TYPE accessCtrl)
{
    uint16   i;
    uint16   pageNumbs;
    uint16   accessBytes;
    portSIZE_TYPE  rt 	= numbBytes;

    //�жϲ����Ƿ�Ϸ�
    if (0 == numbBytes)
    {
        return 0;
    }
    //����Ҫ������ҳ��ֵ
    pageNumbs                                 = (numbBytes/BYTES_PER_PAGE);
    //����Ҫ������ҳ��ֵ ����Ϊ��ʱ ��ʾ��ʱ������ҳ����Ϊ��ҳֵ
    if (0 == (numbBytes%BYTES_PER_PAGE))
    {
        pageNumbs--;
    }
    //�ж�Ҫ������ҳ��ֵ�Ƿ񳬳�����������������ҳ��ֵ
    if ((subAddr/BYTES_PER_PAGE + pageNumbs) > PAGES_PER_DEVICE)
    {
        return -1;
    }
    //subaddr should be align with page size

    for (i = 0; i <= pageNumbs; i++, subAddr += BYTES_PER_PAGE, acessAddr += BYTES_PER_PAGE)
    {
        //����Ҫ�������ֽ���
        if (numbBytes > BYTES_PER_PAGE)
        {
            numbBytes                         -= BYTES_PER_PAGE;
            accessBytes                        = BYTES_PER_PAGE;
        }
        else
        {
            accessBytes                        = numbBytes;
        }
        //I2Cʵ�ʲ��������
        {
        	switch (subAddrType){
            //�ӵ�ַ�����ж�
        	case ONE_BYTE_SUBA:
                //�ӵ�ַΪ���ֽ�
        		t_iic_transfer.m_slaAddr     	= (uint8)(slaAddr);					        //�����Ĵӵ�ַ
        		t_iic_transfer.m_subAddr    	    = subAddr;								    //�����ӵ�ַ
        		t_iic_transfer.m_subAddrCnt	    = 1;								        //�����ӵ�ַΪ1�ֽ�
        		break;
        	case TWO_BYTE_SUBA:
        		//�ӵ�ַΪ˫�ֽ�
                t_iic_transfer.m_slaAddr     	= (uint8)(slaAddr);						    //�����Ĵӵ�ַ
        		t_iic_transfer.m_subAddr   	 	= subAddr;								    //�����ӵ�ַ
        		t_iic_transfer.m_subAddrCnt	    = 2;								        //�����ӵ�ַΪ2�ֽ�
        		break;
        	case X_PLUS_BYTE_SUBA:
				//�ӵ�ַ�ṹΪ8+X   the eight of slaaadr is r/w bit
				t_iic_transfer.m_slaAddr			= (uint8)(slaAddr + ((subAddr >> 7) & 0x0e));	//�����Ĵӵ�ַ
        		t_iic_transfer.m_subAddr		    = subAddr & 0x0ff;						    //�����ӵ�ַ
        		t_iic_transfer.m_subAddrCnt	    = 1;								        //�����ӵ�ַΪ8+x
        		break;
        	default:
        		break;
        	}
            t_iic_transfer.m_paccessAddr         = acessAddr;							    //���ݽ��ջ�����ָ��
        	t_iic_transfer.m_accessNumbBytes     = accessBytes;								//Ҫ��ȡ�ĸ���
        	t_iic_transfer.m_accessFinished      = FALSE;
            t_iic_transfer.m_accessCtrl          = accessCtrl;
            t_iic_transfer.m_timeout 			 = 800;										//wait for timeout:800 ms

            rt                                   = cpu_iic_transfer(&t_iic_transfer);
            if ((I2C_WRITE == accessCtrl) && (t_iic_transfer.m_accessNumbBytes == rt)){
                delay_ms(25);                                                               //�ȴ��������
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
