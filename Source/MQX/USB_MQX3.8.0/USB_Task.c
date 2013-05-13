/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
* $FileName: USB_Task.c$
* $Version : 3.6.13.0$
* $Date    : Aug-25-2010$
*
* Comments:
*
*   This file is the main file for filesystem demo. Note that this example
*   is a multi tasking example and needs an operating system to run. This 
*   means that customers who are not using MQX should change the operating system
*   dependent code. An attempt has been made to comment out the code
*   however, a programmer must review all lines of code to ensure that
*   it correctly compiles with their libraries of operating system and
*   targetcompiler. This program has been compiled and tested for ARC AA3
*   processor with MQX real time operating system.
*
*END************************************************************************/

#include <string.h>
#include <usb.h>
#include <mqx_dll.h>
#include <lwevent.h>

#include <hostapi.h>
#include <mqx_host.h>
#include <host_ch9.h>
#include <host_common.h>
#include <usb_desc.h>
#include <host_dev_list.h>
#include <usb_host_msd_bo.h>
#include <usb_host_hub_sm.h>

#include "usb_task.h"
#include "usb_file.h"

#include <mfs.h>
#include "shell.h" // shell function
#include "sh_prv.h"// shell function

//#include "demo.h"
//#include "FTP_Task.h"

#include "MenuView.h"

#define USB_EVENT 0x01

#define _USB_DBUG_

LWEVENT_STRUCT USB_Event;
LWSEM_STRUCT   USB_Stick;
pointer        USB_handle = NULL;

volatile DEVICE_STRUCT device = { 0 }; 

/* Table of driver capabilities this application want to use */
static const USB_HOST_DRIVER_INFO ClassDriverInfoTable[] =
{  
   /* Vendor ID Product ID Class Sub-Class Protocol Reserved Application call back */
   /* Floppy drive */
   {{0x00,0x00}, {0x00,0x00}, USB_CLASS_MASS_STORAGE, USB_SUBCLASS_MASS_UFI, USB_PROTOCOL_MASS_BULK, 0, usb_host_mass_device_event },

   /* USB 2.0 hard drive */
   {{0x00,0x00}, {0x00,0x00}, USB_CLASS_MASS_STORAGE, USB_SUBCLASS_MASS_SCSI, USB_PROTOCOL_MASS_BULK, 0, usb_host_mass_device_event},

   /* USB hub */
   {{0x00,0x00}, {0x00,0x00}, USB_CLASS_HUB, USB_SUBCLASS_HUB_NONE, USB_PROTOCOL_HUB_LS, 0, usb_host_hub_device_event},

   /* End of list */
   {{0x00,0x00}, {0x00,0x00}, 0,0,0,0, NULL}
};


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_device_event
* Returned Value : None
* Comments       :
*     called when a mass storage device has been attached, detached, etc.
*END*--------------------------------------------------------------------*/

void usb_host_mass_device_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint_32           event_code
   )
{
   INTERFACE_DESCRIPTOR_PTR   intf_ptr =
      (INTERFACE_DESCRIPTOR_PTR)intf_handle;

   switch (event_code) {
      case USB_CONFIG_EVENT:
         /* Drop through into attach, same processing */
      case USB_ATTACH_EVENT:
         if (device.STATE == USB_DEVICE_IDLE ||
            device.STATE == USB_DEVICE_DETACHED)
         {
            device.DEV_HANDLE = dev_handle;
            device.INTF_HANDLE = intf_handle;
            device.STATE = USB_DEVICE_ATTACHED;
            device.SUPPORTED = TRUE;
            _lwevent_set(&USB_Event,USB_EVENT);
         }
         break;
      case USB_INTF_EVENT:
         device.STATE = USB_DEVICE_INTERFACED;
         break;
      case USB_DETACH_EVENT:
         device.DEV_HANDLE = NULL;
         device.INTF_HANDLE = NULL;
         device.STATE = USB_DEVICE_DETACHED;
         _lwevent_set(&USB_Event,USB_EVENT);
         break;
      default:
         device.STATE = USB_DEVICE_IDLE;
         break;
   } 
} 


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_task
* Returned Value : None
* Comments       :
*     First function called. This rouine just transfers control to host main
*END*--------------------------------------------------------------------*/

void USB_task(uint_32 param)
{ 
   _usb_host_handle     host_handle;
   USB_STATUS           error;
   pointer              usb_fs_handle = NULL;
   
#ifdef _USB_DBUG_
   printf("\n----------   USB_Task  ----------\n");
   printf("\n----------             ----------\n");
   printf("\n----------             ----------\n");
   printf("\n----------      END    ----------\n");
#endif 

#if DEMO_USE_POOLS && defined(DEMO_MFS_POOL_ADDR) && defined(DEMO_MFS_POOL_SIZE)
   _MFS_pool_id = _mem_create_pool((pointer)DEMO_MFS_POOL_ADDR, DEMO_MFS_POOL_SIZE);
#endif

   _lwsem_create(&USB_Stick,0);
   _lwevent_create(&USB_Event,0);

   USB_lock();
   _int_install_unexpected_isr();
   _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER,  (pointer) &_bsp_usb_host_callback_table);

   error = _usb_host_init(USBCFG_DEFAULT_HOST_CONTROLLER, 4, &host_handle);
   if (error == USB_OK) {
      error = _usb_host_driver_info_register(host_handle, (pointer)ClassDriverInfoTable);
      if (error == USB_OK) {
         error = _usb_host_register_service(host_handle, USB_SERVICE_HOST_RESUME,NULL);
      }
   }

   USB_unlock();

   if (error == USB_OK) {
      
      for ( ; ; ) {
         // Wait for insertion or removal event
         _lwevent_wait_ticks(&USB_Event,USB_EVENT,FALSE,0);

         if ( device.STATE== USB_DEVICE_ATTACHED) {
           

            if (device.SUPPORTED)  {
               error = _usb_hostdev_select_interface(device.DEV_HANDLE,
               device.INTF_HANDLE, (pointer)&device.CLASS_INTF);
               if(error == USB_OK) {
                  device.STATE = USB_DEVICE_INTERFACED;

                  USB_handle = (pointer)&device.CLASS_INTF;

                  // Install the file system
                  usb_fs_handle = usb_filesystem_install( USB_handle, "USB:","PM_C1:" , "u:" );  //  "PM_C1:" --> ����    NULL --> �޷���
                  if (usb_fs_handle) {
                     // signal the application
                     _lwsem_post(&USB_Stick);
                  }
/* wk @130405 --> make some dirs */
      SHELL_CONTEXT_PTR    shell_ptr;
      shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
      _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
      uint_32 file_size;
      uchar status;
     
      /* wk @130401 --> �½� power/event���ڻ�����������/�¼����ݱ��� */
      shell_ptr->ARGC = 2;
      shell_ptr->ARGV[0]="cd";
      shell_ptr->ARGV[1]="u:\\"; 
      Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
      
//      shell_ptr->ARGC = 2;
//      shell_ptr->ARGV[0]="df_s";
      shell_ptr->ARGV[1]="POWER";   //wk --> ע�⣺���ҵ��ļ�����ʱ����Ҫ�Ǵ�д
      status=Shell_search_file_r1(shell_ptr->ARGC, shell_ptr->ARGV,&file_size);
      if(status==0)
      {
//        shell_ptr->ARGC = 2;
//        shell_ptr->ARGV[0]="mkdir";
        shell_ptr->ARGV[1]="POWER"; 
        Shell_mkdir(shell_ptr->ARGC, shell_ptr->ARGV);
      }
//      shell_ptr->ARGC = 2;
//      shell_ptr->ARGV[0]="df_s";
      shell_ptr->ARGV[1]="EVENT";   //wk --> ע�⣺���ҵ��ļ�����ʱ����Ҫ�Ǵ�д
      status=Shell_search_file_r1(shell_ptr->ARGC, shell_ptr->ARGV,&file_size);
     
      if(status==0)
      { 
//        shell_ptr->ARGC = 2;
//        shell_ptr->ARGV[0]="mkdir";
        shell_ptr->ARGV[1]="EVENT"; 
        Shell_mkdir(shell_ptr->ARGC, shell_ptr->ARGV);
      }
      
      _mem_free(shell_ptr);
                        
      USB_Flg=1; // wk @130407 --> USB ����
  /* wk @130405 --> make some dirs <-- end */    
      
               }
            } else {
                device.STATE = USB_DEVICE_INTERFACED;
            }
         } else if ( device.STATE==USB_DEVICE_DETACHED) {
            _lwsem_wait(&USB_Stick);
            // remove the file system
           usb_filesystem_uninstall(usb_fs_handle);
           
           USB_Flg=0; // wk @130407 --> USB �γ�
         }
         
         // clear the event
         _lwevent_clear(&USB_Event,USB_EVENT);
      }
   }
}

/* EOF */
