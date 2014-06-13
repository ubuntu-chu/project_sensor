/******************************************************************************
 *                          ���ļ������õ�ͷ�ļ�
******************************************************************************/ 
 
#include    "hal.h"

/******************************************************************************
 *                       ���ļ�������ľ�̬ȫ�ֱ���
******************************************************************************/ 

//�豸����
static list                 s_DeviceList;
static DeviceStatus_TYPE    s_DeviceErrorInfo                      = DEVICE_OK;

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

portuBASE_TYPE hal_init(void)
{
    //�豸��������ʼ��
	API_DeviceManageInit();
    //����ע��
    extern portuBASE_TYPE drv_driverregister(void);
    drv_driverregister();
    //����Ӳ���豸��ʼ��
    API_DeviceInitAll();
    
    return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

void API_DeviceManageInit(void)
{
	list_init(&s_DeviceList);
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

portuBASE_TYPE API_DeviceRegister(const pDeviceAbstract pdev)
{
	//portCPSR_TYPE   level;
    
    if (NULL == pdev) 
    {
        return FALSE;
    }

	/* lock interrupt */
	//level                                               = portDISABLE_INTERRUPTS(); 
	/* insert object into information object list */
	list_insert_after(&s_DeviceList, &(pdev->m_list));
	/* unlock interrupt */
    //portENABLE_INTERRUPTS(level);

	return TRUE;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

portuBASE_TYPE API_DeviceUnregister(pDeviceAbstract pdev)
{
	//portCPSR_TYPE   level;
    
    if (NULL == pdev) 
    {
        return FALSE;
    }

    /* lock interrupt */
    //level                                               = portDISABLE_INTERRUPTS();
    /* insert object into information object list */
    list_del(&(pdev->m_list));
    /* unlock interrupt */
    //portENABLE_INTERRUPTS(level);
    
	return TRUE;
}

/******************************************************************************
 *  �������� :  �����豸��ʼ������                                                              
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

DeviceStatus_TYPE API_DeviceInitAll(void)
{
	struct DEVICE_ABSTRACT *pdev;
    list                    *pnode;
    DeviceStatus_TYPE        rt;
    
    for (pnode = s_DeviceList.m_next; pnode != &s_DeviceList; pnode = pnode->m_next)
    {
        FP_pfinit           *init;
        DeviceAbstractInfo  *pdevInfo;
        
        //��ȡ�豸���
        pdev                                                    = (struct DEVICE_ABSTRACT*)list_entry(pnode, struct DEVICE_ABSTRACT, m_list);
        pdevInfo                                                = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
        init                                                    = pdevInfo->init;
        //�����豸��ʼ������
        if ((NULL != init) && !(pdev->m_openFlag & DEVICE_FLAG_ACTIVATED))
        {
            rt                                                  = init(pdev);
            if (DEVICE_OK != rt)
            {
                //�豸��ʼ��ʧ�� ʧ�ܴ���
            }
            else
            {
                //��λ�豸���־
                pdev->m_openFlag                               |= DEVICE_FLAG_ACTIVATED;
            }
        }
    }
    
    return DEVICE_OK;
}

/******************************************************************************
 *  �������� :  �����豸����                                                            
 *                                                                           
 *  �������� :  �����豸���ֲ����豸�������豸�������                                                             
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

struct DEVICE_ABSTRACT *API_DeviceFind(const char* name)
{
    struct DEVICE_ABSTRACT *pdev;
    DeviceAbstractInfo      *pdevInfo;
    list                    *pnode;
    
    //��ǰ��̨ϵͳ�� û�б�Ҫ������б��� ��Ϊһ������ڵ��ô˺���ʱ ���󲿷����ں�̨����
    //portDISABLE_INTERRUPTS();
    
    for (pnode = s_DeviceList.m_next; pnode != &s_DeviceList; pnode = pnode->m_next)
    {
        //��ȡ�豸���
        pdev                                                    = (struct DEVICE_ABSTRACT*)list_entry(pnode, struct DEVICE_ABSTRACT, m_list);
        pdevInfo                                                = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
        if (0 == strncmp((char const *)(pdevInfo->m_name), (char const *)name, DEVICE_NAME_MAX))
        {
            //�ҵ��豸
            return (struct DEVICE_ABSTRACT *)pdev;
        }
        
    }
    
    //portENABLE_INTERRUPTS();
    //δ�ҵ��豸
    return NULL;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

DeviceStatus_TYPE API_DeviceInit(pDeviceAbstract pdev)
{
    DeviceAbstractInfo      *pdevInfo;
    DeviceStatus_TYPE        rt                                 = DEVICE_OK;
    FP_pfinit               *init;
    
    if (NULL == pdev)
    {
        return DEVICE_ENULL;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    init                                                        = pdevInfo->init;
    //�����豸��ʼ������
    if ((NULL != init) && !(pdev->m_openFlag & DEVICE_FLAG_ACTIVATED))
    {
        rt                                                      = init(pdev);
        if (DEVICE_OK != rt)
        {
            //�豸��ʼ��ʧ�� ʧ�ܴ���
        }
        else
        {
            //��λ�豸���־
            pdev->m_openFlag                                   |= DEVICE_FLAG_ACTIVATED;
        }
    }
    return rt;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

DeviceStatus_TYPE API_DeviceOpen(pDeviceAbstract pdev, uint16 oflag)
{
    DeviceAbstractInfo      *pdevInfo;
    DeviceStatus_TYPE        rt;
    FP_pfinit               *init;
    FP_pfopen               *open;
    uint16                   deviceFlag;
    
    if (NULL == pdev)
    {
        return DEVICE_ENULL;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    init                                                        = pdevInfo->init;
    if (!(pdev->m_openFlag & DEVICE_FLAG_ACTIVATED))
    {
        rt                                                      = DEVICE_OK;
        //�豸��δ���г�ʼ�� �����Ҫ�Ƚ��г�ʼ��
        if (init != NULL)
        {
            rt                                                  = init(pdev);
        }
        if (DEVICE_OK != rt)
        {
            //�豸��ʼ��ʧ�� ʧ�ܴ���
        }
        else
        {
            //��λ�豸���־
            pdev->m_openFlag                                   |= DEVICE_FLAG_ACTIVATED;
        }
    }
    //�豸��ʶ
    deviceFlag                                                  = pdevInfo->m_flag;
    
    //�豸�Ƕ�ռ�豸���Ѵ� �����豸æ
	if ((deviceFlag & DEVICE_FLAG_STANDALONE) && (pdev->m_openFlag & DEVICE_OFLAG_OPEN))
    {
		return DEVICE_EBUSY;
    }
    //���豸
    open                                                        = pdevInfo->open;
    if (open != NULL)
	{
        rt                                                      = open(pdev, oflag);
	}
	else
	{
		/* no this interface in device driver */
		rt                                                      = DEVICE_ENOSYS;
	}

	/* set open flag */
	if ((rt == DEVICE_OK) || (rt == DEVICE_ENOSYS))
    {
		pdev->m_openFlag                                        = oflag | DEVICE_OFLAG_OPEN;
    }

	return rt;
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

DeviceStatus_TYPE API_DeviceClose(pDeviceAbstract pdev)
{
    DeviceAbstractInfo      *pdevInfo;
    DeviceStatus_TYPE        rt;
    FP_pfclose              *close;
    
    if (NULL == pdev)
    {
        return DEVICE_ENULL;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    close                                                       = pdevInfo->close;

    //���ùرպ����ӿ�
    if (NULL != close)
    {
        rt                                                      = close(pdev);
    }
    else
    {
        rt                                                      = DEVICE_ENOSYS;
    }
    if ((rt == DEVICE_OK) || (rt == DEVICE_ENOSYS))
    {
		pdev->m_openFlag                                        = DEVICE_OFLAG_CLOSE;
    }
    return rt;
}


DevicePoll_TYPE hal_poll(pDeviceAbstract pdev)
{
    DeviceAbstractInfo      *pdevInfo;
    DevicePoll_TYPE 		rt;
    FP_pfpoll              *poll;

    if (NULL == pdev)
    {
        return DEVICE_POLL_ENULL;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    poll                                                       = pdevInfo->poll;

    //���ùرպ����ӿ�
    if (NULL != poll){
        rt                                                      = poll(pdev);
    }else {
        rt                                                      = DEVICE_POLL_ENOSYS;
    }
    return rt;

}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

portSSIZE_TYPE API_DeviceRead(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size)
{
    DeviceAbstractInfo      *pdevInfo;
    FP_pfread               *read;
    
    if ((NULL == pdev) || (NULL == buffer) || (0 == size))
    {
        //�����豸�����������->�������Ϸ�
        API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
        return -1;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    read                                                        = pdevInfo->read;
    if (NULL != read)
    {
        return (portSSIZE_TYPE)read(pdev, pos, buffer, size);
    }
    //δ�д˺����ӿ� ���ô������
    API_DeviceErrorInfoSet(DEVICE_ENOSYS);
    
    return static_cast<portSSIZE_TYPE>(-1);
    
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

portSSIZE_TYPE API_DeviceWrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size)
{
    DeviceAbstractInfo      *pdevInfo;
    FP_pfwrite              *write;
    
    if ((NULL == pdev) || (NULL == buffer) || (0 == size))
    {
        //�����豸�����������->�������Ϸ�
        API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
        return 0;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    write                                                       = pdevInfo->write;
    if (NULL != write)
    {
        return (portSSIZE_TYPE)write(pdev, pos, buffer, size);
    }
    //δ�д˺����ӿ� ���ô������
    API_DeviceErrorInfoSet(DEVICE_ENOSYS);
    
    return static_cast<portSSIZE_TYPE>(-1);
}

/******************************************************************************
 *  �������� :                                                                
 *                                                                           
 *  �������� :                                                               
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

DeviceStatus_TYPE API_DeviceControl(pDeviceAbstract pdev, uint8 cmd, void *args)
{
    DeviceAbstractInfo      *pdevInfo;
    FP_pfcontrol            *control;
    
    if (NULL == pdev)
    {
        return DEVICE_ENULL;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    switch (cmd){
    case DEVICE_IOC_BLOCK:

    	pdev->m_openFlag 				&= ~DEVICE_FLAG_NONBLOCK;
    	break;

    case DEVICE_IOC_NONBLOCK:

    	pdev->m_openFlag 				|= DEVICE_FLAG_NONBLOCK;
    	break;

    default:

		control                                                     = pdevInfo->control;
		return (NULL != control)?(control(pdev, cmd, args)):(DEVICE_ENOSYS);
    }
    return DEVICE_OK;;
}

/******************************************************************************
 *  �������� :                                                          
 *                                                                           
 *  �������� : �豸������Ϣ���� ��ȡ����                                                             
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

void API_DeviceErrorInfoSet(DeviceStatus_TYPE errorInfo)
{
    s_DeviceErrorInfo                   = errorInfo;
}

DeviceStatus_TYPE API_DeviceErrorInfoGet(void)
{
    DeviceStatus_TYPE   rt              =  s_DeviceErrorInfo; 
    
    s_DeviceErrorInfo                   = DEVICE_OK;
    return rt;
}

/******************************************************************************
 *  �������� :                                                          
 *                                                                           
 *  �������� : ��ȡrx_indicate����ָ��                                                             
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

DeviceStatus_TYPE API_DeviceRxIndicatePtrGet(pDeviceAbstract pdev, FP_pfrx_indicate **rx_indicate)
{
    DeviceAbstractInfo      *pdevInfo;
    
    (*rx_indicate)                                              = NULL;
    if (NULL == pdev)
    {
        return DEVICE_ENULL;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    (*rx_indicate)                                              = pdevInfo->rx_indicate;
    
    return (NULL == (*rx_indicate))?DEVICE_ENOSYS:DEVICE_OK;
}

/******************************************************************************
 *  �������� :                                                          
 *                                                                           
 *  �������� : ��ȡtx_indicate����ָ��                                                             
 *                                                                           
 *  ��ڲ��� :                                                                
 *                                                                             
 *  ���ڲ��� :                                                                
 *                                                                              
 *  �� д �� :                                                                
 *                                                                                 
 *  ��    �� :                                                                 
 *                                                                              
 *  �� �� �� :                                                                                                                               
 *                                                                             
 *  ��    �� :                                                                
 *                                                                                                                                       
 *  ��    ע :                                                                
 *                                                                            
 *                                                                            
******************************************************************************/

DeviceStatus_TYPE API_DeviceTxIndicatePtrGet(pDeviceAbstract pdev, FP_pftx_complete **tx_complete)
{
    DeviceAbstractInfo      *pdevInfo;
    
    (*tx_complete)                                              = NULL;
    if (NULL == pdev)
    {
        return DEVICE_ENULL;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    (*tx_complete)                                              = pdevInfo->tx_complete;
    
    return (NULL == (*tx_complete))?DEVICE_ENOSYS:DEVICE_OK;
}

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

