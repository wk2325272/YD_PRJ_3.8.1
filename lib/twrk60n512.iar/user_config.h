/*
**
**  !!! GENERATED FILE !!! DO NOT CHANGE !!!
**
*/

/**HEADER**********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
*******************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*
* $FileName: user_config.h$
* $Version : 3.8.B.3$
* $Date    : Jun-27-2012$
*
* Comments:
*
*   User configuration for MQX components
*
*END**************************************************************************/

#ifndef __user_config_h__
#define __user_config_h__

/* mandatory CPU identification */
#define MQX_CPU                 PSP_CPU_MK60DN512Z

/* Silicon version number */
#define MK60_REV_1_0             1

/* wk@130512--> */
#define BSPCFG_ENABLE_GPIODEV    1
#define BSPCFG_ENABLE_RTCDEV     1

#define BSPCFG_ENABLE_TTYA       1
#define BSPCFG_ENABLE_ITTYA      1
#define BSPCFG_ENABLE_TTYB       1
#define BSPCFG_ENABLE_ITTYB      1
#define BSPCFG_ENABLE_TTYC       1
#define BSPCFG_ENABLE_ITTYC      1
#define BSPCFG_ENABLE_TTYD       1
#define BSPCFG_ENABLE_ITTYD      1
#define BSPCFG_ENABLE_TTYE       1
#define BSPCFG_ENABLE_ITTYE      1
#define BSPCFG_ENABLE_TTYF       1
#define BSPCFG_ENABLE_ITTYF      1

#define BSPCFG_ENABLE_SPI0       1
#define BSPCFG_ENABLE_SPI1       1

#define MQX_USE_TIMER 1 // wk @130401--> add for timer

#define BSPCFG_ENABLE_TCHSRES 1 // wk @130401--> add
//#define MQX_HAS_TIME_SLICE 1  // wk --> 2012-1-2

#define BSP_SDCARD_SDHC_CHANNEL   1

#define BSPCFG_ENABLE_SPI2       1 
#define BSPCFG_ENABLE_ISPI0      1 
#define BSPCFG_ENABLE_ISPI1      1
#define BSPCFG_ENABLE_ISPI2      1
#define BSPCFG_ENABLE_FLASHX     1 

/* wk@130512--> end */

/* MGCT: <generated_code> */
//#define BSPCFG_ENABLE_TTYA       0
//#define BSPCFG_ENABLE_ITTYA      0
//#define BSPCFG_ENABLE_TTYB       0
//#define BSPCFG_ENABLE_ITTYB      0
//#define BSPCFG_ENABLE_TTYC       0
//#define BSPCFG_ENABLE_ITTYC      0
//#define BSPCFG_ENABLE_TTYD       1
//#define BSPCFG_ENABLE_ITTYD      0
//#define BSPCFG_ENABLE_TTYE       0
//#define BSPCFG_ENABLE_ITTYE      0
//#define BSPCFG_ENABLE_TTYF       1
//#define BSPCFG_ENABLE_ITTYF      0

#define BSPCFG_ENABLE_I2C0       1
#define BSPCFG_ENABLE_II2C0      1
#define BSPCFG_ENABLE_I2C1       0
#define BSPCFG_ENABLE_II2C1      0

//#define BSPCFG_ENABLE_SPI0       0
//#define BSPCFG_ENABLE_ISPI0      0
//#define BSPCFG_ENABLE_SPI1       0
//#define BSPCFG_ENABLE_ISPI1      0
//#define BSPCFG_ENABLE_SPI2       0
//#define BSPCFG_ENABLE_ISPI2      0
//#define BSPCFG_ENABLE_GPIODEV    0
//#define BSPCFG_ENABLE_RTCDEV     1

#define BSPCFG_ENABLE_PCFLASH    0
#define BSPCFG_ENABLE_ADC0       0
#define BSPCFG_ENABLE_ADC1       1
//#define BSPCFG_ENABLE_FLASHX     0
#define BSPCFG_ENABLE_ESDHC      1
#define BSPCFG_ENABLE_IODEBUG    0
#define BSPCFG_ENABLE_II2S0      0

#define MQX_USE_IDLE_TASK        1
#define MQX_ENABLE_LOW_POWER     0

#define RTCSCFG_ENABLE_ICMP      1
#define RTCSCFG_ENABLE_UDP       1
#define RTCSCFG_ENABLE_TCP       1
#define RTCSCFG_ENABLE_STATS     1
#define RTCSCFG_ENABLE_GATEWAYS  1
#define FTPDCFG_USES_MFS         1
#define RTCSCFG_ENABLE_SNMP      0

#define TELNETDCFG_NOWAIT        FALSE 

#define MQX_TASK_DESTRUCTION     1

#define HTTPDCFG_POLL_MODE       0
#define HTTPDCFG_STATIC_TASKS    0
#define HTTPDCFG_DYNAMIC_TASKS   1
/* MGCT: </generated_code> */

/*
** include common settings
*/

/* use the rest of defaults from small-RAM-device profile */
#include "small_ram_config.h"

/* and enable verification checks in kernel */
#include "verif_enabled_config.h"

#endif /* __user_config_h__ */
