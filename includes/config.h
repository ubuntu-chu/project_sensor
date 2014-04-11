#ifndef    _CONFIG_H
#define    _CONFIG_H

/*------------------------------------------------------------------
 *              cpu relative settings
**------------------------------------------------------------------*/

#define MCLK_1MHz   1
#define MCLK_2MHz   2
#define MCLK_4MHz   3
#define MCLK_8MHz   4
#define MCLK_16MHz   5
#define MCLK_25MHz   6

#define def_VLOCLK	12000
#define def_WATCH_SOURCE 32768
#define SYS_MCLK     MCLK_16MHz

#if SYS_MCLK == MCLK_25MHz
#define def_MCLK 25000000

#elif SYS_MCLK == MCLK_16MHz
#define def_MCLK 16000000UL

#elif SYS_MCLK == MCLK_8MHz
#define def_MCLK    8000000
#define def_RSELX_CONFIG def_RSELX_8M
#elif SYS_MCLK == MCLK_4MHz
#define def_MCLK    4250000
#define def_RSELX_CONFIG def_RSELX_4M
#endif

#define def_WDT_SOURCE def_VLOCLK
#define def_ACLK_SOURCE def_WATCH_SOURCE
#define MAX_1MS_BASE	65
#define def_os_tick_rate 1000

//******************************************************************************
// WDT_CFG


#define def_FRAME_delimiter	        0x5AA5
#define def_FRAME_END_delimiter		0xAA55

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
#define 	MONITOR_MANAGE							(1)
#define 	def_TIME_MONITOR_NR						(2)
//define time lib
#define 	MINI_TIME_LIBRARY
#define 	circular buffer
#define    CIRCULAR_BUFFER

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
