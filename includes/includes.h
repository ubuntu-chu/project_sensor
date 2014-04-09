#ifndef _INCLUEDES_H_
#define _INCLUEDES_H_

#include    "types.h"
#include    "config.h"
#include    "macro.h"

//#define		__LINUX_OS__

#include <string.h>
#include <stdlib.h>

#ifdef	__LINUX_OS__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#else

#include "msp430x54x.h"
#endif


#include "../api/utils.h"
#include "../bsp/bsp_startup.h"
#include "../bsp/hal/hal.h"
#include "../bsp/driver/drv_interface.h"
#include "../bsp/cpu/cpu_interrupt.h"
#include "../bsp/cpu/cpu_tick.h"
#include "../bsp/cpu/cpu_gpio.h"
#include "../bsp/cpu/cpu_uart.h"
#include "../bsp/cpu/cpu_spi.h"
#include "../bsp/cpu/cpu_iic.h"
#include "../app/env_datum.h"











#endif

