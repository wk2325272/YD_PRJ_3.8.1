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
* $FileName: men_ram.c$
* $Version : $
* $Date    : 20130106$
*
* Comments:
*
*   
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>
#include <Sh_mfs.h>
#include <part_mgr.h>
#include <io_gpio.h>
#include <lwevent.h>
#include <string.h>


/////////////////////////////////////////////////////////////////////////////////#dx 201314
#if 0//defined (BSP_EXTERNAL_MRAM_RAM_BASE) && defined (BSP_EXTERNAL_MRAM_RAM_SIZE)
#warning Please check that the MRAM memory is available on your EVB (Tower). 
#define RAM_DISK_BASE   BSP_EXTERNAL_MRAM_RAM_BASE 
#define RAM_DISK_SIZE   BSP_EXTERNAL_MRAM_RAM_SIZE
#else 
#warning Modify the RAM_DISK_SIZE if you need more or less ramdisk space. 
//#define RAM_DISK_SIZE   0x4000 //#32KB=0x8000 #16KB=0x4000 #10KB=0x2800 #6KB= 0x1800 //default-->
#define RAM_DISK_SIZE   0x4000 //#32KB=0x8000 #16KB=0x4000 #10KB=0x2800 #6KB= 0x1800 // wk -->
   static uchar   ram_disk[RAM_DISK_SIZE];
#define RAM_DISK_BASE   &ram_disk[0]
#endif
//////////////////////////////////////////////////////////////////////#dx 201314

/* function prototypes */
void mem_ram_app(uint_32);

/*TASK*-----------------------------------------------------
* 
* Task Name    : mem_ram_app
* Comments     :
*
*END*-----------------------------------------------------*/
void mem_ram_app
   (
      uint_32 initial_data
   )
{
  ////////////////////////////////////////////////////////////////#dx 201314   

   MQX_FILE_PTR               dev_handle1,
   a_fd_ptr;
   int_32                     error_code;
   _mqx_uint                  mqx_status;
   a_fd_ptr = 0;
   char  ramdisk_filesystem_name[] = "r:";///////////////////°²×°ÅÌ·û
   /* install device */
   mqx_status = _io_mem_install("mfs_ramdisk:", (uchar_ptr)RAM_DISK_BASE,
                                (_file_size)RAM_DISK_SIZE);
   /* Number of sectors is returned by ioctl IO_IOCTL_GET_NUM_SECTORS function */
   /* If another disc structure is desired, use MFS_FORMAT_DATA structure to   */
   /* define it and call standart format function instead default_format       */   
   if ( mqx_status != MQX_OK ) {
     printf("\nError installing memory device (0x%x)", mqx_status);
     _task_block();
   } /* Endif */
   
   /* Open the device which MFS will be installed on */
   dev_handle1 = fopen("mfs_ramdisk:", 0);
   if ( dev_handle1 == NULL ) {
     printf("\nUnable to open Ramdisk device");
     _task_block();
   } /* Endif */
   
   /* Install MFS  */
   mqx_status = _io_mfs_install(dev_handle1, ramdisk_filesystem_name, (_file_size)0);
   if (mqx_status != MFS_NO_ERROR) {
     printf("\nError initializing%s",ramdisk_filesystem_name);
     _task_block();
   } else {
     printf("\nInitialized Ram Disk to %s\\",ramdisk_filesystem_name);
   } /* Endif */
   
   /* Open the filesystem and format detect, if format is required */
   a_fd_ptr = fopen(ramdisk_filesystem_name, NULL);
   error_code    = ferror(a_fd_ptr);
   if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK))
   {
     printf("\nError while opening %s\\ (%s)", ramdisk_filesystem_name, MFS_Error_text((uint_32)(uint_32)error_code));
     _task_block();
   } /* Endif */
   if ( error_code == MFS_NOT_A_DOS_DISK ) {
     char_ptr argv[]={"format",ramdisk_filesystem_name};///////////////////////////ramdisk_filesystem_name
     //strcpy(argv,ramdisk_filesystem_name);
     Shell_format(2,argv);
     printf("\nNOT A DOS DISK! You must format to continue.");
   } /* Endif */
////////////////////////////////////////////////////////////////#dx 201314
  
    _task_block();    
   // _task_destroy(_task_get_id());
}

/* EOF */
