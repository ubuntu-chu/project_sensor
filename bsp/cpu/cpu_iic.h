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
 
#ifndef __CPU_IIC_H
#define __CPU_IIC_H

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

#define IIC_DELAY_INTERVAL	1

//#define IIC_ADDR_MODE_TWO

#ifdef IIC_ADDR_MODE_TWO
typedef uint16_t	iic_addr_mode_t;
#else
typedef uint8_t		iic_addr_mode_t;
#endif

#define iic_wr_addr 		0xA0
#define iic_rd_addr 		0xA1


#define scl_out_pt    		P10OUT
#define sda_out_pt    		P10OUT
#define sda_in_pt     		P10IN

#define scl_dir_pt			P10DIR
#define sda_dir_pt			P10DIR

#define scl_pin     		BIT2
#define sda_pin     		BIT1

#define iic_set_sda_out()	(sda_dir_pt |= sda_pin)
#define iic_set_sda_in()	(sda_dir_pt &= ~sda_pin)

#define iic_set_sda()	(sda_out_pt |= sda_pin)
#define iic_rst_sda()	(sda_out_pt &= ~sda_pin)

#define iic_set_scl_out()	(scl_dir_pt |= scl_pin)
#define iic_set_scl()	(scl_out_pt |= scl_pin)
#define iic_rst_scl()	(scl_out_pt &= ~scl_pin)

#define iic_sda_in		(sda_in_pt & sda_pin)


void iic_wr_nbyte(iic_addr_mode_t addr, uint8_t *ptr, uint8_t cnt);
void iic_rd_nbyte(iic_addr_mode_t addr, uint8_t *ptr, uint8_t cnt);


void cpu_iic_init(void);

/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#ifdef __cplusplus
}
#endif


#endif
