#ifndef __io_h__
#define __io_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
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
**************************************************************************
*
* $FileName: io.h$
* $Version : 3.8.21.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   This file is the header file for the I/O subsystem interface.                        
*
*END************************************************************************/

/* Include for variable length argument functions */
#include <stdarg.h>
#include <io_rev.h>
#include "ioctl.h"

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/* Return value for end of file */
#define IO_EOF          (-1)

/* Error code returned by I/O functions */
#define IO_ERROR        (-1)

/* Task Default Stream indicators */
#define IO_STDIN        (0)
#define IO_STDOUT       (1)
#define IO_STDERR       (2)

/* Processor Default Stream indicators */
#define IO_PROC_STDIN   (3)
#define IO_PROC_STDOUT  (4)
#define IO_PROC_STDERR  (5)

/* Open mode Flag bit */
#define IO_O_RDONLY                 (0x01)
#define IO_O_WRONLY                 (0x02)
#define IO_O_RDWR                   (IO_O_RDONLY | IO_O_WRONLY)
#define IO_O_APPEND                 (0x04)
#define IO_O_TRUNC                  (0x08)
#define IO_O_EXCL                   (0x10)
#define IO_O_NONBLOCK               (0x20)
#define IO_O_CREAT                  (0x80)
#define IO_O_SYNC                   (0x100)
#define IO_O_RSYNC                  (0x200)
#define IO_O_DSYNC                  (0x400)
#define IO_O_USER_SHIFT             (16)                   

/* other I/O flags */
#define IO_PERIPHERAL_PIN_MUX_ENABLE                (0x01)
#define IO_PERIPHERAL_PIN_MUX_DISABLE               (0x02)
#define IO_PERIPHERAL_CLOCK_ENABLE                  (0x04)   
#define IO_PERIPHERAL_CLOCK_DISABLE                 (0x08)   
#define IO_PERIPHERAL_MODULE_ENABLE                 (0x10)   
#define IO_PERIPHERAL_WAKEUP_ENABLE                 (0x20)   
#define IO_PERIPHERAL_WAKEUP_SLEEPONEXIT_DISABLE    (0x40)   

/* Is this a PCB device */
#define IO_FLAG_IS_PCB_DEVICE       (0x1000)

/* Standard IOCTL commands */
#define IO_IOCTL_GET_FLAGS          _IO(IO_TYPE_MQX,0x01)
#define IO_IOCTL_SET_FLAGS          _IO(IO_TYPE_MQX,0x02)
#define IO_IOCTL_FLUSH_OUTPUT       _IO(IO_TYPE_MQX,0x03)
#define IO_IOCTL_GET_STATS          _IO(IO_TYPE_MQX,0x04)
#define IO_IOCTL_CLEAR_STATS        _IO(IO_TYPE_MQX,0x05)
#define IO_IOCTL_CHAR_AVAIL         _IO(IO_TYPE_MQX,0x06)
#define IO_IOCTL_SEEK               _IO(IO_TYPE_MQX,0x07)

/* Check whether a device can seek or not */
#define IO_IOCTL_SEEK_AVAIL         _IO(IO_TYPE_MQX,0x08)

/* Query a device to find out its properties */
#define IO_IOCTL_DEVICE_IDENTIFY    _IO(IO_TYPE_MQX,0x09)

/* Command to uninstall device */
#define IO_IOCTL_UNINSTALL          _IO(IO_TYPE_MQX,0x0A)

#define IO_IOCTL_GET_BLOCK_SIZE     _IO(IO_TYPE_MQX,0x0B)
#define IO_IOCTL_GET_NUM_SECTORS    _IO(IO_TYPE_MQX,0x0C)


#define IO_IOCTL_GET_DRIVE_PARAMS    _IO(IO_TYPE_MQX,0x15)
#define IO_IOCTL_GET_VENDOR_INFO     _IO(IO_TYPE_MQX,0x16)
#define IO_IOCTL_GET_PRODUCT_ID      _IO(IO_TYPE_MQX,0x17)
#define IO_IOCTL_GET_PRODUCT_REV     _IO(IO_TYPE_MQX,0x18)
#define IO_IOCTL_STOP_DEVICE         _IO(IO_TYPE_MQX,0x19)
#define IO_IOCTL_SET_BLOCK_MODE      _IO(IO_TYPE_MQX,0x1A)




/* 
** IO_IOCTL_DEVICE_IDENTIFY results
** The parameter used when calling this command shall be the address of
** an array of unsigneds, size 3.  After a successful call to the command,
** the array elements will be filled as follows: 
*/

/* 
** Element 0: DeviceTypePhysical 
** set to one of the following 
*/
#define IO_DEV_TYPE_PHYS_CAN                 (0x0001)
#define IO_DEV_TYPE_PHYS_ENET                (0x0002)
#define IO_DEV_TYPE_PHYS_FIREWIRE            (0x0003)
#define IO_DEV_TYPE_PHYS_FLASH               (0x0004)
#define IO_DEV_TYPE_PHYS_FLOPPY              (0x0005)
#define IO_DEV_TYPE_PHYS_HDLC                (0x0006)
#define IO_DEV_TYPE_PHYS_IDE                 (0x0007)
#define IO_DEV_TYPE_PHYS_MEMIO               (0x0008)
#define IO_DEV_TYPE_PHYS_PCCARD              (0x0009) 
#define IO_DEV_TYPE_PHYS_PCFLASH             (0x000A)
#define IO_DEV_TYPE_PHYS_PCI                 (0x000B)
#define IO_DEV_TYPE_PHYS_SCSI                (0x000C)
#define IO_DEV_TYPE_PHYS_SERIAL_EE           (0x000D)
#define IO_DEV_TYPE_PHYS_SERIAL_INTERRUPT    (0x000E)
#define IO_DEV_TYPE_PHYS_SERIAL_POLLED       (0x000F)
#define IO_DEV_TYPE_PHYS_SPI                 (0x0010)
#define IO_DEV_TYPE_PHYS_USB                 (0x0011)
#define IO_DEV_TYPE_PHYS_ADV_PCCARD          (0x0012) 
#define IO_DEV_TYPE_PHYS_FLASHX              (0x0013)
#define IO_DEV_TYPE_PHYS_USB_MFS             (0x0014)
#define IO_DEV_TYPE_PHYS_ADC                 (0x0015)
#define IO_DEV_TYPE_PHYS_I2C_INTERRUPT       (0x0016)
#define IO_DEV_TYPE_PHYS_I2C_POLLED          (0x0017)
#define IO_DEV_TYPE_PHYS_QSPI_POLLED         (0x0019)
#define IO_DEV_TYPE_PHYS_SDCARD              (0x001A)
#define IO_DEV_TYPE_PHYS_NANDFLASH           (0x001B)
#define IO_DEV_TYPE_PHYS_ESDHC               (0x001C)
#define IO_DEV_TYPE_PHYS_TCHSCR              (0x001D)
#define IO_DEV_TYPE_PHYS_USB_DCD_INTERRUPT   (0x001E)
#define IO_DEV_TYPE_PHYS_USB_DCD_POLLED      (0x001F)

/* 
** Element 1: DeviceTypeLogical
** Bitmask describing what product the device is intended to work with 
*/
#define IO_DEV_TYPE_LOGICAL_LAPB             (0x0001)
#define IO_DEV_TYPE_LOGICAL_LAPD             (0x0002)
#define IO_DEV_TYPE_LOGICAL_MFS              (0x0004)
#define IO_DEV_TYPE_LOGICAL_PPP              (0x0008)
#define IO_DEV_TYPE_LOGICAL_RTCS             (0x0010)
#define IO_DEV_TYPE_LOGICAL_SDLC             (0x0020)

/* 
** Element 2: DeviceAttributes
** Bitmask describing the capabilities of the device 
*/
#define IO_DEV_ATTR_ERASE                    (0x0001)
#define IO_DEV_ATTR_INTERRUPT                (0x0002)
#define IO_DEV_ATTR_POLL                     (0x0004)
#define IO_DEV_ATTR_READ                     (0x0008)
#define IO_DEV_ATTR_REMOVE                   (0x0010)
#define IO_DEV_ATTR_SEEK                     (0x0020)
#define IO_DEV_ATTR_WRITE                    (0x0040)
#define IO_DEV_ATTR_SW_FLOW_CONTROL          (0x0080)
#define IO_DEV_ATTR_HW_FLOW_CONTROL          (0x0100)

#define IO_DEV_ATTR_BLOCK_MODE               (0x0200)

/* Element defines for ID array */
#define IO_IOCTL_ID_PHY_ELEMENT          (0)
#define IO_IOCTL_ID_LOG_ELEMENT          (1)
#define IO_IOCTL_ID_ATTR_ELEMENT         (2)

/*
** Error codes 
*/
#define IO_OK                      (0)
#define IO_DEVICE_EXISTS           (IO_ERROR_BASE|0x00)
#define IO_DEVICE_DOES_NOT_EXIST   (IO_ERROR_BASE|0x01)
#define IO_ERROR_READ              (IO_ERROR_BASE|0x02)
#define IO_ERROR_WRITE             (IO_ERROR_BASE|0x03)
#define IO_ERROR_SEEK              (IO_ERROR_BASE|0x04)
#define IO_ERROR_WRITE_PROTECTED   (IO_ERROR_BASE|0x05)
#define IO_ERROR_READ_ACCESS       (IO_ERROR_BASE|0x06)
#define IO_ERROR_WRITE_ACCESS      (IO_ERROR_BASE|0x07)
#define IO_ERROR_SEEK_ACCESS       (IO_ERROR_BASE|0x08)
#define IO_ERROR_INVALID_IOCTL_CMD (IO_ERROR_BASE|0x09)
#define IO_ERROR_DEVICE_BUSY       (IO_ERROR_BASE|0x0A)
#define IO_ERROR_DEVICE_INVALID    (IO_ERROR_BASE|0x0B)

#define IO_ERROR_TIMEOUT           (IO_ERROR_BASE|0x10)
#define IO_ERROR_INQUIRE           (IO_ERROR_BASE|0x11)


/* 
** Seek parameters 
*/
#define IO_SEEK_SET (1) /* Seek from start */
#define IO_SEEK_CUR (2) /* Seek from current location */
#define IO_SEEK_END (3) /* Seek from end */

/*
** The delimiter to use for a device name
*/
#define IO_DEV_DELIMITER (':')

/*--------------------------------------------------------------------------*/
/*
**                        MACRO DECLARATIONS
*/


/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

/*
**
** IO DEVICE STRUCT
**
** This is the structure used to store device information for an
** installed I/O driver
*/
typedef struct io_device_struct
{

   /* Used to link io_device_structs together */
   QUEUE_ELEMENT_STRUCT QUEUE_ELEMENT;

   /*
   ** A string that identifies the device.  This string is matched
   ** by fopen, then the other information is used to initialize a
   ** FILE struct for standard I/O.  This string is also provided in
   ** the kernel initialization record for the default I/O channel
   */
   char_ptr             IDENTIFIER;
  
   /* The I/O init function */
   _mqx_int (_CODE_PTR_ IO_OPEN)(MQX_FILE_PTR, char _PTR_, char _PTR_);

   /* The I/O deinit function */
   _mqx_int (_CODE_PTR_ IO_CLOSE)(MQX_FILE_PTR);

   /* The I/O read function */
   _mqx_int (_CODE_PTR_ IO_READ)(MQX_FILE_PTR, char_ptr, _mqx_int);

   /* The I/O write function */
   _mqx_int (_CODE_PTR_ IO_WRITE)(MQX_FILE_PTR, char_ptr, _mqx_int);

   /* The I/O ioctl function */
   _mqx_int (_CODE_PTR_ IO_IOCTL)(MQX_FILE_PTR, _mqx_uint, pointer);

   /* The function to call when uninstalling this device */
   _mqx_int (_CODE_PTR_ IO_UNINSTALL)(struct io_device_struct _PTR_);
   
   /* The I/O channel specific initialization data */
   pointer              DRIVER_INIT_PTR;

    /* Type of driver for this device */
    _mqx_uint           DRIVER_TYPE;

} IO_DEVICE_STRUCT, _PTR_ IO_DEVICE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                      FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern pointer   _io_get_handle(_mqx_uint);
extern _mqx_uint _io_init(void);
extern _mqx_uint _io_dev_install(
   char_ptr, 
   _mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char_ptr, char_ptr),
   _mqx_int (_CODE_PTR_)(MQX_FILE_PTR),
   _mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char_ptr, _mqx_int),
   _mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char_ptr, _mqx_int),
   _mqx_int (_CODE_PTR_)(MQX_FILE_PTR, _mqx_uint, pointer),
   pointer);
extern _mqx_uint _io_dev_install_ext(
   char_ptr, 
   _mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char _PTR_, char _PTR_),
   _mqx_int (_CODE_PTR_)(MQX_FILE_PTR),
   _mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char_ptr,  _mqx_int),
   _mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char_ptr,  _mqx_int),
   _mqx_int (_CODE_PTR_)(MQX_FILE_PTR, _mqx_uint, pointer),
   _mqx_int (_CODE_PTR_)(IO_DEVICE_STRUCT_PTR),
   pointer);
extern _mqx_int _io_dev_uninstall(char_ptr);   
extern pointer  _io_set_handle(_mqx_uint, pointer);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
