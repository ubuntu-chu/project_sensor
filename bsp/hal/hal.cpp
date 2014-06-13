/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 
 
#include    "hal.h"

/******************************************************************************
 *                       本文件所定义的静态全局变量
******************************************************************************/ 

//设备链表
static list                 s_DeviceList;
static DeviceStatus_TYPE    s_DeviceErrorInfo                      = DEVICE_OK;

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
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

portuBASE_TYPE hal_init(void)
{
    //设备管理器初始化
	API_DeviceManageInit();
    //驱动注册
    extern portuBASE_TYPE drv_driverregister(void);
    drv_driverregister();
    //所有硬件设备初始化
    API_DeviceInitAll();
    
    return TRUE;
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
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

void API_DeviceManageInit(void)
{
	list_init(&s_DeviceList);
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
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
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
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
 *  函数名称 :  所有设备初始化函数                                                              
 *                                                                           
 *  函数功能 :                                                               
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

DeviceStatus_TYPE API_DeviceInitAll(void)
{
	struct DEVICE_ABSTRACT *pdev;
    list                    *pnode;
    DeviceStatus_TYPE        rt;
    
    for (pnode = s_DeviceList.m_next; pnode != &s_DeviceList; pnode = pnode->m_next)
    {
        FP_pfinit           *init;
        DeviceAbstractInfo  *pdevInfo;
        
        //获取设备句柄
        pdev                                                    = (struct DEVICE_ABSTRACT*)list_entry(pnode, struct DEVICE_ABSTRACT, m_list);
        pdevInfo                                                = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
        init                                                    = pdevInfo->init;
        //调用设备初始化函数
        if ((NULL != init) && !(pdev->m_openFlag & DEVICE_FLAG_ACTIVATED))
        {
            rt                                                  = init(pdev);
            if (DEVICE_OK != rt)
            {
                //设备初始化失败 失败处理
            }
            else
            {
                //置位设备活动标志
                pdev->m_openFlag                               |= DEVICE_FLAG_ACTIVATED;
            }
        }
    }
    
    return DEVICE_OK;
}

/******************************************************************************
 *  函数名称 :  查找设备函数                                                            
 *                                                                           
 *  函数功能 :  根据设备名字查找设备并返回设备操作句柄                                                             
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

struct DEVICE_ABSTRACT *API_DeviceFind(const char* name)
{
    struct DEVICE_ABSTRACT *pdev;
    DeviceAbstractInfo      *pdevInfo;
    list                    *pnode;
    
    //在前后台系统中 没有必要对齐进行保护 因为一般而言在调用此函数时 绝大部分是在后台运行
    //portDISABLE_INTERRUPTS();
    
    for (pnode = s_DeviceList.m_next; pnode != &s_DeviceList; pnode = pnode->m_next)
    {
        //获取设备句柄
        pdev                                                    = (struct DEVICE_ABSTRACT*)list_entry(pnode, struct DEVICE_ABSTRACT, m_list);
        pdevInfo                                                = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
        if (0 == strncmp((char const *)(pdevInfo->m_name), (char const *)name, DEVICE_NAME_MAX))
        {
            //找到设备
            return (struct DEVICE_ABSTRACT *)pdev;
        }
        
    }
    
    //portENABLE_INTERRUPTS();
    //未找到设备
    return NULL;
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
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
    //调用设备初始化函数
    if ((NULL != init) && !(pdev->m_openFlag & DEVICE_FLAG_ACTIVATED))
    {
        rt                                                      = init(pdev);
        if (DEVICE_OK != rt)
        {
            //设备初始化失败 失败处理
        }
        else
        {
            //置位设备活动标志
            pdev->m_openFlag                                   |= DEVICE_FLAG_ACTIVATED;
        }
    }
    return rt;
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
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
        //设备还未进行初始化 因此需要先进行初始化
        if (init != NULL)
        {
            rt                                                  = init(pdev);
        }
        if (DEVICE_OK != rt)
        {
            //设备初始化失败 失败处理
        }
        else
        {
            //置位设备活动标志
            pdev->m_openFlag                                   |= DEVICE_FLAG_ACTIVATED;
        }
    }
    //设备标识
    deviceFlag                                                  = pdevInfo->m_flag;
    
    //设备是独占设备且已打开 返回设备忙
	if ((deviceFlag & DEVICE_FLAG_STANDALONE) && (pdev->m_openFlag & DEVICE_OFLAG_OPEN))
    {
		return DEVICE_EBUSY;
    }
    //打开设备
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
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
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

    //调用关闭函数接口
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

    //调用关闭函数接口
    if (NULL != poll){
        rt                                                      = poll(pdev);
    }else {
        rt                                                      = DEVICE_POLL_ENOSYS;
    }
    return rt;

}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
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

portSSIZE_TYPE API_DeviceRead(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size)
{
    DeviceAbstractInfo      *pdevInfo;
    FP_pfread               *read;
    
    if ((NULL == pdev) || (NULL == buffer) || (0 == size))
    {
        //设置设备驱动错误代码->参数不合法
        API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
        return -1;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    read                                                        = pdevInfo->read;
    if (NULL != read)
    {
        return (portSSIZE_TYPE)read(pdev, pos, buffer, size);
    }
    //未有此函数接口 设置错误代码
    API_DeviceErrorInfoSet(DEVICE_ENOSYS);
    
    return static_cast<portSSIZE_TYPE>(-1);
    
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
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

portSSIZE_TYPE API_DeviceWrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size)
{
    DeviceAbstractInfo      *pdevInfo;
    FP_pfwrite              *write;
    
    if ((NULL == pdev) || (NULL == buffer) || (0 == size))
    {
        //设置设备驱动错误代码->参数不合法
        API_DeviceErrorInfoSet(DEVICE_EPARAM_INVALID);
        return 0;
    }
    pdevInfo                                                    = (DeviceAbstractInfo *)(pdev->m_pdeviceAbstractInfo);
    write                                                       = pdevInfo->write;
    if (NULL != write)
    {
        return (portSSIZE_TYPE)write(pdev, pos, buffer, size);
    }
    //未有此函数接口 设置错误代码
    API_DeviceErrorInfoSet(DEVICE_ENOSYS);
    
    return static_cast<portSSIZE_TYPE>(-1);
}

/******************************************************************************
 *  函数名称 :                                                                
 *                                                                           
 *  函数功能 :                                                               
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
 *  函数名称 :                                                          
 *                                                                           
 *  函数功能 : 设备错误信息设置 获取函数                                                             
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
 *  函数名称 :                                                          
 *                                                                           
 *  函数功能 : 获取rx_indicate函数指针                                                             
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
 *  函数名称 :                                                          
 *                                                                           
 *  函数功能 : 获取tx_indicate函数指针                                                             
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

