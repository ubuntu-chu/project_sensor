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

enum iic_numb{
    enum_NUMB_IIC0 = 0,
    enum_NUMB_IIC_MAX,
};

#if 0
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
#endif

void iic_wr_nbyte(iic_addr_mode_t addr, uint8_t *ptr, uint8_t cnt);
void iic_rd_nbyte(iic_addr_mode_t addr, uint8_t *ptr, uint8_t cnt);

enum{
    I2C_NONE                    = (0),       		//子地址已经处理或者不需要子地址
    I2C_READ                    = (1),       		//读取操作
    I2C_WRITE                   = (2),       		//写操作
    I2C_ABNORMAL                = (0xff),    		//I2C异常
    I2C_TIMEOUT                 = (0xfe), 
};

typedef struct iic_transfer
{
    uint16                              m_subAddr;                     //子地址 器件内地址
    uint16 								m_timeout;					   //unit: ms
    uint8                               m_slaAddr;                     //I2C器件从地址
    uint8                               m_accessNumbBytes;             //存取字节数
    uint8 								m_index;
    uint8 								m_lock;
    volatile uint8               		m_accessFinished;              //存取完成
    uint8                               m_accessCtrl;                  //存取控制
    uint8                               m_subAddrCnt;                  //子地址数目
    uint8                              *m_paccessAddr;                 //存取地址

}iic_transfer_t;


extern iic_transfer_t t_iic_transfer;


portBASE_TYPE cpu_iic_init(enum iic_numb numb);
portSSIZE_TYPE cpu_iic_transfer(struct iic_transfer *ptransfer);



/******************************************************************************
 *                             END  OF  FILE                                                                          
******************************************************************************/

#endif
