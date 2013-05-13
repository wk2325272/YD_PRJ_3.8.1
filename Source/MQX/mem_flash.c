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
* $FileName: mem_flash.c$  512kb flash
* $Version : 3.8.31.4$
* $Date    : May-2-2012$
*
* Comments:
*
*   This file contains the source for the FlashX example program.
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

#if (BSP_M51EMDEMO || BSP_TWRMCF51MM || BSP_TWRMCF51JE)
    #if !BSPCFG_ENABLE_FLASHX2
    #error This application requires BSPCFG_ENABLE_FLASHX1 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
    #define         FLASH_NAME "flashx2:bank1"
#elif BSP_M54455EVB
    #if !BSPCFG_ENABLE_FLASHX0
    #error This application requires BSPCFG_ENABLE_FLASHX0 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
    #define         FLASH_NAME "flashx0:bank0"
#else
    #if !BSPCFG_ENABLE_FLASHX
    #error This application requires BSPCFG_ENABLE_FLASHX defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
    #define         FLASH_NAME "flashx:bank0"
#endif

/* function prototypes */
void mem_flash_app(uint_32);

/*TASK*-----------------------------------------------------
* 
* Task Name    : mem_flash_app
* Comments     :
*    This task finds a string at 'the end' of flash memory
*    then it tries to rewrite it. The successfull rewrite
*    can be proved by re-executing this example application.
*
*END*-----------------------------------------------------*/
void mem_flash_app
   (
      uint_32 initial_data
   )
{
    ///////////////////////////////////////////////////////////// Flashx mfs demo =0x00060000~0x00080000#dx 20130103
#define FLASH_NAME_B       "flashx:"   //FLASH_NAME
    char  flashx_filesystem_name[] = "f:",flashx_partman_name[]="pmb:";///////////////////安装盘符
    MQX_FILE_PTR flashx_handle,flashx_file_handle,flashx_partman_handle;
    _mqx_uint  param; 
    uint_32 error;
    printf("\nMQX Flash Task.");
    
    /* wk --> open flash device */
    flashx_handle = fopen(FLASH_NAME_B, NULL); // "flashx:bank0"
    
     /*wk@130330 -->  test mfs function */
    uchar result;   
//    char pathname[261];
//    ioctl(mfs_fs_ptr, IO_IOCTL_GET_CURRENT_DIR,(uint_32_ptr) pathname);
//    printf("The current directory is: %s\n", pathname);  
   
    if (flashx_handle == NULL) {
      printf("\nUnable to open file %s", FLASH_NAME_B);
      _task_block();
   } else {
      printf("\nFlash file %s opened", FLASH_NAME_B);
   } 
   
   ioctl(flashx_handle, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);/////////////////////重要
//   int len;
//   len = write(flashx_handle, "world",strlen("world"));
//   fseek(flashx_handle, 0, IO_SEEK_SET);
//   
//   fseek(flashx_handle, -32, IO_SEEK_END);
//   len = write(flashx_handle, "hello",strlen("hello"));

   /* wk --> 安装分区 */
    error = _io_part_mgr_install(flashx_handle, flashx_partman_name, 0);// 分区管理
    if (error!= MFS_NO_ERROR)
    {
      printf("\nError installing partition manager: %s", MFS_Error_text((uint_32)error));
      _task_block();
    }
    
    /* Open partition manager */
    flashx_partman_handle = fopen(flashx_partman_name, NULL);// 打开分区
    if (flashx_partman_handle == NULL)
    {
      error = ferror(flashx_partman_handle);
      printf("\nError opening partition manager: %s", MFS_Error_text((uint_32)error));
      _task_block();
    }
    /* Validate partition 1 */
    param = 1;
    error = _io_ioctl(flashx_partman_handle, IO_IOCTL_VAL_PART, &param);// 控制分区
    if (error == MQX_OK)
    { 
      /* Install MFS over Flashx driver */
      error = _io_mfs_install(flashx_partman_handle, flashx_filesystem_name, param);// 安装文件系统，基于分区
      if (error != MFS_NO_ERROR)
      {
        printf("\nError initializing MFS: %s", MFS_Error_text((uint_32)error));
        _task_block();
      }
    } else
    { 
      /* Install MFS over SD card driver */
      error = _io_mfs_install(flashx_handle, flashx_filesystem_name, (_file_size)0);// 安装文件系统，无分区
      if (error != MFS_NO_ERROR)
      {
        printf("\nError initializing MFS: %s", MFS_Error_text((uint_32)error));
        _task_block();
      }
    }

   /* Open file system */
   flashx_file_handle = fopen(flashx_filesystem_name, NULL);// 打开文件系统
   error = ferror (flashx_file_handle);
   if ((error != MFS_NO_ERROR) && (error != MFS_NOT_A_DOS_DISK))
   {
     printf("\nError opening filesystem: %s", MFS_Error_text((uint_32)error));
     _task_block();
   }
   if ( error == MFS_NOT_A_DOS_DISK )//////////////////
   {
     printf("\nNOT A DOS DISK! You must format to continue.");
     
   }
  
   printf ("\nFlashx  installed to %s", flashx_filesystem_name);
    ////////////////////////////////////////////////////////////////#dx 20130103

   _task_block();
   //_task_destroy(_task_get_id());
   
}

/* EOF */
