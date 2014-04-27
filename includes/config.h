#ifndef    _CONFIG_H
#define    _CONFIG_H

/*------------------------------------------------------------------
 *              cpu relative settings
**------------------------------------------------------------------*/



//******************************************************************************


#define def_FRAME_delimiter	        0x5AA5
#define def_FRAME_END_delimiter		0xAA55

/*------------------------------------------------------------------
 *              APP settings
**------------------------------------------------------------------*/

#define		def_MODEL_NAME 			("sensor")

/*------------------------------------------------------------------
 *              OS settings
**------------------------------------------------------------------*/
#define		LINUX_OS

/*------------------------------------------------------------------
 *              APIs settings
**------------------------------------------------------------------*/
//define little endian format
#define 	BIG_ENDIAN              				(0)
//define logger
#define 	LOGGER
#define 	LOG_IN_EMBEDED
//define time monitor count
#define 	def_MONITOR_MANAGE							(1)
#define 	def_MONITOR_TIME_NR						(6)
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
#define    DBG_UART								(UART_0)





#define		DEBUG_SWITCH	(0)

#define    SYS_LOG_LEV_EMERG		(0)
#define    SYS_LOG_LEV_ALERT		(1)
#define    SYS_LOG_LEV_CRIT			(2)
#define    SYS_LOG_LEV_ERR			(3)
#define    SYS_LOG_LEV_WARNING		(4)
#define    SYS_LOG_LEV_NOTICE		(5)
#define    SYS_LOG_LEV_INFO			(6)
#define    SYS_LOG_LEV_DEBUG		(7)

#define    SYS_LOG_LEV				(SYS_LOG_LEV_DEBUG)

#if(DEBUG_SWITCH > 0)

	#ifdef	__LINUX_OS__

		#define		SYS_LOG(format,...)		fprintf(stderr, "FILE: "__FILE__", FUNC: %s , LINE: %d: "format"", __func__, __LINE__, ##__VA_ARGS__)
		#define		SYS_LOG_LEV_TINY(lev, msg)
		#define		SYS_LOG_LEV_TINY_LEN(lev, msg, len)
	#else

		#define    DBG_UART					(UART_0)
		#define		SYS_LOG(format,...)
		#define		SYS_LOG_LEV_TINY(lev, msg)	do{if ((lev) <= SYS_LOG_LEV){uart_tx(DBG_UART, (uint8 *)msg, strlen((char const *)msg));}}while(0)
		#define		SYS_LOG_LEV_TINY_LEN(lev, msg, len)	do{if ((lev) <= SYS_LOG_LEV){uart_tx(DBG_UART, (uint8 *)msg, len);}}while(0)
	#endif
#else

	#define		SYS_LOG(format,...)
	#define		SYS_LOG_LEV_TINY(lev, msg)
	#define		SYS_LOG_LEV_TINY_LEN(lev, msg, len)
#endif

/******************************************************************************
 *                             END  OF  FILE
******************************************************************************/
#endif
