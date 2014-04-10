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
 
#ifndef __CPU_SPI_H 
#define __CPU_SPI_H

/******************************************************************************
 *                             包含文件声明
******************************************************************************/

#ifndef INCLUDES_LOW_H
	#include    "../../includes/includes-low.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 *                            文件接口函数声明
******************************************************************************/ 

enum{
    UCB0   = 0,
};

extern void cpu_spi_init(uint8 num);
extern int8 cpu_spi_w8r8(uint8 num, uint8 w8, uint8 *r8);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
