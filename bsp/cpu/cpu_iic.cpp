/******************************************************************************
 *                          本文件所引用的头文件
******************************************************************************/ 

#include    "cpu_iic.h"
#include "../../includes/macro.h"

/******************************************************************************
 *                       本文件所定义的静态全局变量
******************************************************************************/ 

#ifndef DEF_LOW
	#define DEF_LOW                 	(uint8_t)0
#endif

#ifndef DEF_HIGH
	#define DEF_HIGH                   (uint8_t)1
#endif

void iic_delay_us(uint16_t val) {
	_delay_us(val);
}

void iic_delay_ms(uint16_t val)
{
	_delay_ms(val);
}

inline void iic_pin_init(void)
{
	iic_set_scl_out();
	iic_set_sda_out();

	iic_set_sda();
	iic_set_scl();
}

void iic_init(void)
{
	iic_pin_init();
}

void cpu_iic_init(void)
{
	iic_init();
}

void iic_set_sda_dir(uint8_t dir)
{
	if(dir == 1){
		sda_dir_pt |= sda_pin;
	}else{
		sda_dir_pt &= ~sda_pin;
	}
}

void iic_start(void)
{
	iic_set_sda_out();
	iic_set_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);
	iic_set_sda();
	iic_delay_us(IIC_DELAY_INTERVAL);
	iic_rst_sda();
	iic_delay_us(IIC_DELAY_INTERVAL);
}

void iic_stop(void)
{
	iic_set_sda_out();
	iic_rst_sda();
	iic_delay_us(IIC_DELAY_INTERVAL);
	iic_set_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);
	iic_set_sda();
	iic_delay_us(IIC_DELAY_INTERVAL);
}

uint8_t iic_ack(void)
{
	iic_set_sda_in();

	uint8_t re_val = DEF_HIGH;
	iic_rst_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);
	iic_set_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);
	if(iic_sda_in){
		re_val = DEF_HIGH;
	}else{
		re_val = DEF_LOW;
	}
	iic_rst_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);

	iic_set_sda_out();

	return re_val;
}

void iic_ack_set(uint8_t val)
{
    iic_set_sda_out();

	iic_rst_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);
	if(val == DEF_LOW){
		iic_rst_sda();
	}else{
		iic_set_sda();
	}
	iic_delay_us(IIC_DELAY_INTERVAL);
	iic_set_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);
	iic_rst_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);
}

void iic_noack(void)
{
	iic_set_sda_out();
    
	iic_set_sda();
	iic_delay_us(IIC_DELAY_INTERVAL);
	iic_rst_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);
	iic_set_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);
	iic_rst_scl();
	iic_delay_us(IIC_DELAY_INTERVAL);
}

void iic_wr_byte(uint8_t val)
{
	uint8_t	i;

	iic_set_sda_out();
	for(i = 0; i < 8; i++)
	{
		iic_rst_scl();
		iic_delay_us(IIC_DELAY_INTERVAL);
		if(val & 0x80){
			iic_set_sda();
		}else{
			iic_rst_sda();
		}
		iic_delay_us(IIC_DELAY_INTERVAL);
		iic_set_scl();
		iic_delay_us(IIC_DELAY_INTERVAL);
		val <<= 1;
	}
}


uint8_t iic_rd_byte(void)
{
	uint8_t i;
	uint8_t val = 0;

	iic_set_sda_in();
	iic_rst_scl();
	for( i = 0; i < 8; i++)
	{
		val  <<= 1;
		iic_set_scl();
		iic_delay_us(IIC_DELAY_INTERVAL);
		if(iic_sda_in){
			val |= 0x01;
		}
		iic_delay_us(IIC_DELAY_INTERVAL);
		iic_rst_scl();
		iic_delay_us(IIC_DELAY_INTERVAL);
	}
	return val;
}

void iic_wr_nbyte(iic_addr_mode_t addr, uint8_t *ptr, uint8_t cnt)
{
	uint8_t i = 0;
	iic_set_sda_out();
	iic_start();
	iic_wr_byte(iic_wr_addr);
	iic_ack();
#ifdef IIC_ADDR_MODE_TWO
	iic_wr_byte((uint8_t )addr>>8);
	iic_ack();
#endif
	iic_wr_byte((uint8_t)(addr & 0x00FF));
	iic_ack();
	for(i = 0; i < cnt; i++)
	{
		iic_wr_byte(*ptr);
		ptr++;
		iic_ack();
	}

	iic_stop();
    iic_delay_us(IIC_DELAY_INTERVAL);
}

void iic_wr_cmd(uint8_t cmd)
{
	iic_set_sda_out();
	iic_start();
	iic_wr_byte(iic_wr_addr);

	iic_wr_byte(cmd);
	iic_ack();
	iic_stop();
    iic_delay_us(IIC_DELAY_INTERVAL);
}

void iic_rd_nbyte(iic_addr_mode_t addr, uint8_t *ptr, uint8_t cnt)
{
	uint8_t	i = 0;

	iic_start();
	iic_wr_byte(iic_wr_addr);
	iic_ack();
#ifdef IIC_ADDR_MODE_TWO
	iic_wr_byte((uint8_t)addr>>8);
	iic_ack();
#endif
	iic_wr_byte((uint8_t)(addr & 0x00FF));
	iic_ack();
    
    iic_delay_us(IIC_DELAY_INTERVAL);
	iic_start();
	iic_wr_byte(iic_rd_addr);
	iic_ack();
	for( i = 1; i < cnt; i++)
	{
		*ptr = iic_rd_byte();
		ptr++;
		iic_ack_set(DEF_LOW);
	}
	*ptr = iic_rd_byte();
	iic_noack();
	iic_stop();
}


/*********************************************************************************
**                            End Of File
*********************************************************************************/
