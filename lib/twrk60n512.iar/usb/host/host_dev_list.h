#ifndef __host_dev_list_h__
#define __host_dev_list_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: host_dev_list.h$
* $Version : 3.8.B.2$
* $Date    : Jun-12-2012$
*
* Comments:
*
*   This file defines the device list for USB class drivers.
*   +--------+                           
*   \  ROOT  \---+                       List of devices, one entry  
*   +--------+   \    +--------+           for each attached device
*                +--->\  NEXT  \---+
*                     \--------\   \    +--------+
*                  +--\ MEMLST \   +--->\  NEXT  \---+
*                  \  \--------\        \--------\   \    +--------+
*   Fixed-size     \  \ Device \     +--\ MEMLST \   +--->\  NULL  \
*     per-device   \  \ struct \     \  \--------\        \--------\
*     memory       \  +--------+     \  \ Device \     +--\ MEMLST \
*                  \                 \  \ struct \     \  \--------\
*                  \                 \  +--------+     \  \ Device \
*                  \                 \                 \  \ struct \
*                  +->+--------+     \                 \  +--------+
*   Lists of other    \  NEXT  \     \                 \
*     memory per   +--\--------\     +->+--------+     \
*     device --    \  \ LENGTH \        \  NULL  \     \
*     e.g. pipes,  \  \--------\        \--------\     +->+--------+
*     buffers,     \  \ Device \        \ LENGTH \        \  NULL  \
*     structs      \  \ malloc \        \--------\        \--------\
*                  \  \ memory \        \ Device \        \ LENGTH \
*                  \  +--------+        \ malloc \        \--------\
*                  \                    \ memory \        \ Device \
*                  \                    +--------+        \ malloc \
*                  +->+--------+                          \ memory \
*                     \  NULL  \                          +--------+
*                     \--------\
*                     \ LENGTH \  Length used to check
*                     \--------\    buffer overrun etc.
*                     \ Device \
*                     \ malloc \
*                     \ memory \
*                     +--------+
*   IMPLEMENTATION:
*   Linking list items depends on root pointers behaving like
*   a short entry containing a "next" pointer = NULL when
*   the list is empty.  So next MUST BE FIRST in the structs.
*
*END************************************************************************/
#include "host_common.h"

#if PSP_HAS_DATA_CACHE
#include "ehci_cache.h" //for USB_DMA_ALIGN
#endif

#define  MAX_DEV_INTFS  (32)

/* Types of malloc memory used with USB devices */
enum memory_type 
{
   USB_MEMTYPE_MIN_VALUE,  /* lowest value - 1     */
   USB_MEMTYPE_CONFIG,     /* device configuration */
   USB_MEMTYPE_INTERFACE,  /* Interface descr */
   USB_MEMTYPE_PIPES,      /* pipe bundle          */
   USB_MEMTYPE_BUFFER,     /* buffer               */
   USB_MEMTYPE_CLASS,      /* class driver struct  */
   USB_MEMTYPE_MAX_VALUE   /* highest value + 1    */
};
typedef enum memory_type memory_type;

typedef struct dev_memory
{
   struct dev_memory _PTR_ next;    /* next memory block or NULL */
   uint_32                 blktype; /* type of data, block usage */
   uint_32                 blksize; /* size of data area following */
   /* We store offset of payload to determine real payload location. Offset is needed for alignment.
   ** Since there is also the same byte written just before payload, the offset must be 1B wide.
   ** That determines maximum alignment of 256 Bytes.
   ** Note that this internal method to achieve alignment is not memory friendly, because it inserts
   ** dummy bytes and allocates more memory than needed
   */
   uint_8                  filler[3]; /* since offset MUST be 1B long, we have to put 3 bytes */
   uint_8                  offset;  /* number of bytes inserted after this item when payload starts */
    
   /* Note that normally here are bytes inserted to achieve requested alignement.
   ** Do not use address of payload directly, but always add offset bytes to get the address of payload.
   */
   union {   
      uint_8         data[1];    /* actually [0 ... blksize-1] */
      uchar          bufr[1];    /* uchars [0 ... blksize-1] */
      uint_32        word[1];    /* general-purpose array */
      pointer        bundle[1];  /* pipe bundle */
   } payload;                    /* payload of data */
} DEV_MEMORY, _PTR_ DEV_MEMORY_PTR;
#define  MEM_HEADER_LEN     (16)

/********************************************************************
Note that device instance structure keeps data buffers inside it. 
These buffers are passed to DMA when host does enumeration. This
means that we must ensure that buffers inside this stucture
are aligned in PSP cache line size. 
********************************************************************/

/* Fixed-length fields applicable to all devices */
typedef struct dev_instance
{
    struct dev_instance  _PTR_ next;             /* next device, or NULL */
    DEV_MEMORY_PTR             memlist;          /* list of malloc'd memory */
    _usb_host_handle           host;             /* host (several can exist) */
    uint_8                     speed;            /* device speed */
    uint_8                     hub_no;           /* hub # (root hub = 0) */
    uint_8                     port_no;          /* hub's port # (1 - 8) */
    uint_8                     address;          /* USB address, 1 - 127 */

    uint_16                    cfg_value;
    uint_8                     ctrl_retries;
    uint_8                     new_config;       /* non-zero = new config */

    uint_8                     num_of_interfaces;
    uint_8                     RESERVED0[1];

    uint_16                    state;            /* device state */
    _usb_pipe_handle           control_pipe;     /* control pipe handle */
    tr_callback                control_callback; /* control pipe callback */
    pointer                    control_callback_param; /* control callback param */

#if USB_DMA_ALIGN
    /***********************************************************
    40 bytes have been used above. Device descriptor must be
    aligned on a cache boundary so we must ensure that there
    is enough padding before it.
    ***********************************************************/
    
    uint_8                     RESERVED1[USB_DMA_ALIGN(40) - 40];
#endif // USB_DMA_ALIGN
   
    DEVICE_DESCRIPTOR          dev_descriptor;   /* device descriptor */

#if USB_DMA_ALIGN
    uint_8                     RESERVED2[USB_DMA_ALIGN(sizeof(DEVICE_DESCRIPTOR)) - sizeof(DEVICE_DESCRIPTOR)];   // complete padding
#endif // USB_DMA_ALIGN

    uchar                      buffer[9];        /* enumeration buffer */
   
#if USB_DMA_ALIGN
    uint_8                     RESERVED3[USB_DMA_ALIGN(9) - 9];
#endif // USB_DMA_ALIGN

   /* Interface/s which have been selected (pipes are open) */
   INTERFACE_DESCRIPTOR_PTR   intf_descriptor[MAX_DEV_INTFS];

} DEV_INSTANCE, _PTR_ DEV_INSTANCE_PTR;

/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS usb_dev_list_free_memlist (_usb_device_instance_handle);
USB_STATUS usb_dev_list_free_mem (DEV_INSTANCE_PTR, DEV_MEMORY_PTR);
USB_STATUS usb_dev_list_get_mem (DEV_INSTANCE_PTR, uint_32, memory_type, uint_32 align, DEV_MEMORY_PTR _PTR_);
USB_STATUS usb_dev_list_attach_device (_usb_host_handle, uint_8, uint_8, uint_8);
void usb_dev_list_close_pipe_bundle (PIPE_BUNDLE_STRUCT_PTR);
void usb_dev_list_detach_device (_usb_host_handle, uint_8, uint_8);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
