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
 
#ifndef _DRV_EVENT_H
#define _DRV_EVENT_H

/******************************************************************************
 *                             包含文件声明
******************************************************************************/

#ifndef    _INCLUDES_H_
	#include    "../../../includes/includes.h"
#endif

/******************************************************************************
 *                           文件接口信息宏定义
******************************************************************************/

#define         DEVICE_NAME_EVENT                                 ("EVENT")

/******************************************************************************
 *                            文件接口函数声明
******************************************************************************/ 

extern portuBASE_TYPE drv_eventregister(void);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif
