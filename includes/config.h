#ifndef    _CONFIG_H
#define    _CONFIG_H

/*------------------------------------------------------------------
 *              cpu relative settings
**------------------------------------------------------------------*/

#define     PEND_SV_PRIO                            ((1<<__NVIC_PRIO_BITS) - 1)
#define     SYS_TICK_PRIO                           ((1<<__NVIC_PRIO_BITS) - 1)
#define     UART_IRQ_PRIO                           (1)
#define     I2CM_IRQ_PRIO                           (2)
#define     DMA_UART_TX_IRQ_PRIO                    (0)

//******************************************************************************

#if 0
#define     def_FRAME_delimiter	                    0x5AA5
#define     def_FRAME_END_delimiter		            0xAA55
#endif

/*------------------------------------------------------------------
 *              APP settings
**------------------------------------------------------------------*/

#define		def_MODEL_NAME 			                ("sensor")
#define     def_AD_SAMPLE_LEN                       (16)
#define     def_ADC_SOFT_FILTER
#define     def_RECORD_LEN                          (9)
#define     def_SELF_CALIB_TIME                     (8*def_TIME_1_HOUR)

//定义是否在评估板上运行程序  在传感器的板上运行时 要注释掉此宏 两个板子的硬件配置不同
//#define     def_RUN_IN_EVB

/*------------------------------------------------------------------
 *              OS settings
**------------------------------------------------------------------*/
//#define		LINUX_OS
#define     DEVICE_NAME_MAX	                        (8)
#define     OBJECT_NAME_MAX	                        (8)
#define     def_USING_SERVICE_MQ
#define     TICK_PER_SECOND						    (100)

/*------------------------------------------------------------------
 *              APIs settings
**------------------------------------------------------------------*/
//define little endian format
#define 	BIG_ENDIAN              				(0)

//define logger
#define 	LOGGER
#define 	LOG_IN_EMBEDED

//define time monitor count
#define 	def_MONITOR_MANAGE				        (1)
#define 	def_MONITOR_TIME_NR						(6)

//define timer nr   both  hard timer and soft timer
#define  	def_TIMER_NR							(6)
//define event nr
#define  	def_EVENT_NR							(6)
//define time lib
#define 	def_MINI_TIME_LIBRARY
#define     def_CIRCULAR_BUFFER
#define     def_BUF_QUEUE

//define  def_crc16
#define     def_CRC16
//#define    SUM16

//define  modbus
#define     MODBUS
//#define    RAW_MAC

//debug  define
#define     DBG_UART								 (UART_0)


/******************************************************************************
 *                             END  OF  FILE
******************************************************************************/
#endif
