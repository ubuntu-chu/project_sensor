/******************************************************************************
 * �ļ���Ϣ :  
 *
 * �� �� �� :  
 *
 * �������� : 
 * 
 * ԭʼ�汾 : 
 *     
 * �޸İ汾 :  
 *    
 * �޸����� : 
 *
 * �޸����� :
 * 
 * �� �� �� :
 *
 * ��    ע :
 *
 * ��    �� :   Դ����
 *
 * ��    Ȩ :   
 * 
******************************************************************************/

/******************************************************************************
 *                              ͷ�ļ���ʿ
******************************************************************************/ 
 
#ifndef    _HAL_H_
#define    _HAL_H_

/******************************************************************************
 *                             �����ļ�����
******************************************************************************/ 

#ifndef    _INCLUDES_H_
	#include    "../../includes/includes.h"
#endif

/******************************************************************************
 *                           �ļ��ӿ���Ϣ�궨��
******************************************************************************/ 

#define             DEVICE_NAME_MAX	                        (8)
typedef             int16                                   portSIZE_TYPE;
typedef             portSIZE_TYPE                           portOFFSET_TYPE;


/* device flags */
#define             DEVICE_FLAG_DEACTIVATE		            0x000		    /* not inited 									*/

#define             DEVICE_FLAG_RDONLY			            0x001		    /* read only 									*/
#define             DEVICE_FLAG_WRONLY			            0x002		    /* write only 									*/
#define             DEVICE_FLAG_RDWR				        0x003		    /* read and write 								*/

#define             DEVICE_FLAG_REMOVABLE		            0x004		    /* removable device 							*/
#define             DEVICE_FLAG_STANDALONE		            0x008		    /* standalone device							*/
#define             DEVICE_FLAG_ACTIVATED		            0x010		    /* device is activated 							*/
#define             DEVICE_FLAG_SUSPENDED		            0x020		    /* device is suspended 							*/
#define             DEVICE_FLAG_NONBLOCK		            0x040		    /* device is non-block 							*/


#define             DEVICE_OFLAG_CLOSE			            0x000		    /* device is closed 							*/
#define             DEVICE_OFLAG_RDONLY			            0x001		    /* read only access								*/
#define             DEVICE_OFLAG_WRONLY			            0x002		    /* write only access							*/
#define             DEVICE_OFLAG_RDWR			            0x003		    /* read and write 								*/
#define             DEVICE_OFLAG_OPEN			            0x008		    /* device is opened 							*/


enum{

	DEVICE_IOC_NONBLOCK 	= 0,
	DEVICE_IOC_BLOCK,
	DEVICE_IOC_USER,

};


//posλ�ú궨��
#define             DEVICE_SEEK_SET                                0x00
#define             DEVICE_SEEK_CUR                                0x01
#define             DEVICE_SEEK_END                                0x02

/******************************************************************************
 *                           �ļ��ӿڽṹ�嶨��
******************************************************************************/

struct LIST_NODE
{ struct LIST_NODE 				                           *m_next;	    /* point to next node. 						*/
	struct LIST_NODE 				                           *m_prev;     /* point to prev node. 						*/
};

typedef struct LIST_NODE List;

enum  DEVICE_STATUS_TYPE
{
    DEVICE_ENULL       = 0,
    DEVICE_OK,
    DEVICE_ENOSYS,
    DEVICE_EBUSY,
    DEVICE_EOPEN,
    DEVICE_EPARAM_INVALID,
    DEVICE_ECMD_INVALID,
    DEVICE_EOFLG_INVALID,
	DEVICE_EEXEC,
    DEVICE_EAGAIN,
    DEVICE_ETIMEOUT,
};

typedef            enum  DEVICE_STATUS_TYPE                   DeviceStatus_TYPE;

enum DEVICE_CLASS_TYPE
{
	DEVICE_CLASS_CHAR = 0,						                            /* character device								*/
	DEVICE_CLASS_BLOCK,							                            /* block device 								*/
	DEVICE_CLASS_NETIF,							                            /* net interface 								*/
	DEVICE_CLASS_MTD,							                            /* memory device 								*/
	DEVICE_CLASS_CAN,							                            /* CAN device 									*/
	DEVICE_CLASS_RTC,							                            /* RTC device 									*/
	DEVICE_CLASS_SOUND,							                            /* Sound device 								*/
	DEVICE_CLASS_UNKNOWN							                        /* unknown device 								*/
};

typedef            enum DEVICE_CLASS_TYPE                     DeviceClassType;

typedef            struct DEVICE_ABSTRACT *                   pDeviceAbstract;
typedef            struct DEVICE_ABSTRACT *                   portDEVHANDLE_TYPE;
  
//�豸ע�ắ��ָ��
typedef     portuBASE_TYPE     (FP_pfregister)   (void);

//�ӿں���ָ�붨��  ��̬�Ľӿڱ��ڴ�ͬһ���������������Ķ������
/* common device interface */
typedef     DeviceStatus_TYPE  (FP_pfinit)	     (pDeviceAbstract pdev);
typedef     DeviceStatus_TYPE  (FP_pfopen)	     (pDeviceAbstract pdev, uint16 oflag);
typedef     DeviceStatus_TYPE  (FP_pfclose)      (pDeviceAbstract pdev);
typedef     portSIZE_TYPE      (FP_pfread)	     (pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
typedef     portSIZE_TYPE      (FP_pfwrite)      (pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
typedef     DeviceStatus_TYPE  (FP_pfcontrol)    (pDeviceAbstract pdev, uint8 cmd, void *args);
    
/* device call back */
typedef     DeviceStatus_TYPE  (FP_pfrx_indicate)(pDeviceAbstract pdev, portSIZE_TYPE size);
typedef     DeviceStatus_TYPE  (FP_pftx_complete)(pDeviceAbstract pdev, void* buffer);

struct DEVIVE_ABSTRACT_INFO
{
    //�豸����
    int8      	                                                m_name[DEVICE_NAME_MAX];
	DeviceClassType                                             m_type;
    /* device flag*/
	uint16                                                      m_flag;

	/* common device interface */
	FP_pfinit                                                  *init;
    FP_pfopen                                                  *open;
    FP_pfclose                                                 *close;
    FP_pfread                                                  *read;
    FP_pfwrite                                                 *write;
    FP_pfcontrol                                               *control;
    
    /* device call back */
    FP_pfrx_indicate                                           *rx_indicate;
    FP_pftx_complete                                           *tx_complete;
};

typedef            struct DEVIVE_ABSTRACT_INFO                DeviceAbstractInfo;

//��RAM��С��Ӧ�û����� �豸��Ϣһ����Զ��ǹ̻���FLASH�е� �漰���䶯�Ļ����С
struct DEVICE_ABSTRACT
{
	const struct DEVIVE_ABSTRACT_INFO                        *m_pdeviceAbstractInfo;
    
    //�豸����
    List	                                                    m_list;
    uint16                                                      m_openFlag;
    /* device private data */
	void                                                      *m_private;
};

typedef            struct DEVICE_ABSTRACT                      DeviceAbstract;
typedef            struct DEVICE_ABSTRACT                      device_t;


/******************************************************************************
 *                            �ļ��ӿں�������
******************************************************************************/

extern portuBASE_TYPE hal_init(void);

extern void API_DeviceManageInit(void);

extern portuBASE_TYPE API_DeviceRegister(const pDeviceAbstract pdev);
#define hal_deviceregister         API_DeviceRegister 

extern DeviceStatus_TYPE API_DeviceInitAll(void);
#define hal_deviceinitall         API_DeviceInitAll 

extern struct DEVICE_ABSTRACT *API_DeviceFind(const char* name);
#define hal_devicefind            API_DeviceFind 

extern DeviceStatus_TYPE API_DeviceInit(pDeviceAbstract pdev);
#define hal_deviceinit            API_DeviceInit

extern DeviceStatus_TYPE API_DeviceOpen(pDeviceAbstract pdev, uint16 oflag);
#define hal_deviceopen            API_DeviceOpen

extern DeviceStatus_TYPE API_DeviceClose(pDeviceAbstract pdev);
#define hal_deviceclose            API_DeviceClose

extern portSIZE_TYPE API_DeviceRead(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
#define hal_deviceread            API_DeviceRead

extern portSIZE_TYPE API_DeviceWrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
#define hal_devicewrite            API_DeviceWrite

extern DeviceStatus_TYPE API_DeviceControl(pDeviceAbstract pdev, uint8 cmd, void *args);
#define hal_devicectrl            API_DeviceControl

extern void API_DeviceErrorInfoSet(DeviceStatus_TYPE errorInfo);
#define hal_device_errinfoset            API_DeviceErrorInfoSet

extern DeviceStatus_TYPE API_DeviceErrorInfoGet(void);
#define hal_device_errinfoget            API_DeviceErrorInfoGet

extern DeviceStatus_TYPE API_DeviceRxIndicatePtrGet(pDeviceAbstract pdev, FP_pfrx_indicate **rx_indicate);
#define hal_device_rxindicateptrget            API_DeviceRxIndicatePtrGet

extern DeviceStatus_TYPE API_DeviceTxIndicatePtrGet(pDeviceAbstract pdev, FP_pftx_complete **tx_complete);
#define hal_device_txindicateptrget            API_DeviceTxIndicatePtrGet

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif


