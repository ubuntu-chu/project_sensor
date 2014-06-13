/******************************************************************************
 * 文件信息 :  
 *
 * 创 建 者 :  
 *
 * 创建日期 : 
 * 
 * 原始版本 : 
 *     
 * 修改版本 :  
 *    
 * 修改日期 : 
 *
 * 修改内容 :
 * 
 * 审 核 者 :
 *
 * 附    注 :
 *
 * 描    述 :   源代码
 *
 * 版    权 :   
 * 
******************************************************************************/

/******************************************************************************
 *                              头文件卫士
******************************************************************************/ 
 
#ifndef    _HAL_H_
#define    _HAL_H_

/******************************************************************************
 *                             包含文件声明
******************************************************************************/ 

#ifndef    _INCLUDES_H_
	#include    "../../includes/includes-low.h"
    #include    "../../includes/service.h"
#endif

/******************************************************************************
 *                           文件接口信息宏定义
******************************************************************************/ 



/******************************************************************************
 *                            文件接口函数声明
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

extern portSSIZE_TYPE API_DeviceRead(pDeviceAbstract pdev, portOFFSET_TYPE pos, void* buffer, portSIZE_TYPE size);
#define hal_deviceread            API_DeviceRead

extern portSSIZE_TYPE API_DeviceWrite(pDeviceAbstract pdev, portOFFSET_TYPE pos, const void* buffer, portSIZE_TYPE size);
#define hal_devicewrite            API_DeviceWrite

extern DeviceStatus_TYPE API_DeviceControl(pDeviceAbstract pdev, uint8 cmd, void *args);
#define hal_devicectrl            API_DeviceControl

extern DevicePoll_TYPE hal_poll(pDeviceAbstract pdev);

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


