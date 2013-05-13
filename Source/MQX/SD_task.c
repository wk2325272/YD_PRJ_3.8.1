/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: demo.c$
* $Version : 3.8.31.1$
* $Date    : Mar-26-2012$
*
* Comments: Provide MFS file system on external SD card.
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>

#include <sdcard.h>
#include <sdcard_spi.h>
#include <spi.h>
#include <part_mgr.h>
#include <io_gpio.h>

#include <lwevent.h>



#if defined BSP_SDCARD_ESDHC_CHANNEL
#if ! BSPCFG_ENABLE_ESDHC
#error This application requires BSPCFG_ENABLE_ESDHC defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#elif defined BSP_SDCARD_SDHC_CHANNEL

#if ! BSPCFG_ENABLE_SDHC
#error This application requires BSPCFG_ENABLE_SDHC defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#elif defined (BSP_TWRMCF51CN) || defined (BSP_TWRPXS20) || defined (BSP_TWR_K20D72M)

#if ! BSPCFG_ENABLE_SPI1
#error This application requires BSPCFG_ENABLE_SPI1 defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#else
#if ! BSPCFG_ENABLE_SPI0
#error This application requires BSPCFG_ENABLE_SPI0 defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#endif

#if defined (BSP_SDCARD_SPI_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SPI_CHANNEL
#elif defined (BSP_SDCARD_ESDHC_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_ESDHC_CHANNEL
#elif defined (BSP_SDCARD_SDHC_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SDHC_CHANNEL
#else
#error "SDCARD low level communication device not defined!"
#endif
/**************dx 20121210******************************************************/
#define BSP_SDCARD_GPIO_DETECT_INT  0
#define SD_EVENT_DETECT   0x01
LWEVENT_STRUCT SD_Event;



/*******************************************************************************/



#ifdef BSP_SDCARD_GPIO_CS

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : set_CS
* Comments  : This function sets chip select signal to enable/disable memory
*
*END*----------------------------------------------------------------------*/

static void set_CS (uint_32 cs_mask, uint_32 logic_level, pointer user_data)
{
    LWGPIO_STRUCT_PTR gpiofd = user_data;

    if (cs_mask & BSP_SDCARD_SPI_CS)
    {
        lwgpio_set_value(gpiofd, logic_level ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW);
    }
}

#endif
#if defined BSP_SDCARD_GPIO_DETECT
    #if BSP_SDCARD_GPIO_DETECT_INT
    void sd_detect_int__callback()
    {
      _lwevent_set(&SD_Event,SD_EVENT_DETECT);
      
    }

    #endif
#endif


/*TASK*-----------------------------------------------------------------
*
* Function Name  : sdcard_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void sdcard_task(uint_32 temp)
{
    boolean      inserted = TRUE, readonly = FALSE, last = FALSE;
    _mqx_int     error_code;
    _mqx_uint    param;
    MQX_FILE_PTR com_handle, sdcard_handle, filesystem_handle, partman_handle;
    char         filesystem_name[] = "a:";// �ļ�ϵͳ ����
    char         partman_name[] = "pm:";// �������� ����
#if defined BSP_SDCARD_GPIO_DETECT
    #if ! BSP_SDCARD_GPIO_DETECT_INT
    LWGPIO_STRUCT      sd_detect;
    #else 
    LWGPIO_STRUCT      sd_detect;
    /************************dx 20121210**********************************************
     */
    GPIO_PIN_STRUCT sd_detect_int[] = {
        BSP_SDCARD_GPIO_DETECT | GPIO_PIN_IRQ_FALLING | GPIO_PIN_IRQ_RISING,
        GPIO_LIST_END
    };
    
    MQX_FILE_PTR port_file_sd_detect_int;
    /***********************************************************************************/
    #endif
    
    
#endif
#if defined BSP_SDCARD_GPIO_PROTECT
    LWGPIO_STRUCT      sd_protect;
#endif
#ifdef BSP_SDCARD_GPIO_CS

    LWGPIO_STRUCT          sd_cs;
    SPI_CS_CALLBACK_STRUCT callback;

#endif
    
    
    /* Open low level communication device */
    com_handle = fopen (SDCARD_COM_CHANNEL, NULL);// ��"esdhc:"ģ��ײ����� 

    if (NULL == com_handle)
    {
        printf("Error installing communication handle.\n");
        _task_block();
    }

#ifdef BSP_SDCARD_GPIO_CS

    /* Open GPIO file for SPI CS signal emulation */
    error_code = lwgpio_init(&sd_cs, BSP_SDCARD_GPIO_CS, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);// ��װ����������spiƬѡ
    if (!error_code)
       {
           printf("Initializing GPIO with associated pins failed.\n");
           _task_block();
       }
    lwgpio_set_functionality(&sd_cs,BSP_SDCARD_CS_MUX_GPIO);
    lwgpio_set_attribute(&sd_cs, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
    /* Set CS callback */
    callback.MASK = BSP_SDCARD_SPI_CS;
    callback.CALLBACK = set_CS;
    callback.USERDATA = &sd_cs;
    if (SPI_OK != ioctl (com_handle, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback))
    {
        printf ("Setting CS callback failed.\n");
        _task_block();
    }

#endif

#if defined BSP_SDCARD_GPIO_DETECT
    #if ! BSP_SDCARD_GPIO_DETECT_INT
    /* Init GPIO pins for other SD card signals */
    error_code = lwgpio_init(&sd_detect, BSP_SDCARD_GPIO_DETECT, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);// ��װ���������ڲ�����
       if (!error_code)
       {
           printf("Initializing GPIO with sdcard detect pin failed.\n");
           _task_block();
       }
       /*Set detect and protect pins as GPIO Function */
       lwgpio_set_functionality(&sd_detect,BSP_SDCARD_DETECT_MUX_GPIO);
       lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
   #else 
       /******************dx 20121210***********************************************/
        if (NULL == (port_file_sd_detect_int = fopen("gpio:read", (char_ptr) &sd_detect_int )))
        {
           printf("Opening port_file_sd_detect_int GPIO with associated button1_int failed.\n");
          _task_block();
        }
        ioctl(port_file_sd_detect_int, GPIO_IOCTL_SET_IRQ_FUNCTION, (pointer)sd_detect_int__callback);
         _lwevent_create(&SD_Event,0); // �����������¼����ڼ��SD����
       /****************************************************************************/
    
   #endif
    
      
#endif

#if defined BSP_SDCARD_GPIO_PROTECT
    /* Init GPIO pins for other SD card signals */
    error_code = lwgpio_init(&sd_protect, BSP_SDCARD_GPIO_PROTECT, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);// ��װ����������ֻ�����
       if (!error_code)
       {
           printf("Initializing GPIO with sdcard protect pin failed.\n");
           _task_block();
       }
       /*Set detect and protect pins as GPIO Function */
       lwgpio_set_functionality(&sd_protect,BSP_SDCARD_PROTECT_MUX_GPIO);
       lwgpio_set_attribute(&sd_protect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    /* Install SD card device */
    error_code = _io_sdcard_install("sdcard:", (pointer)&_bsp_sdcard0_init, com_handle);// ��װSD������������"esdhc:"ģ��
    if ( error_code != MQX_OK )
    {
        printf("Error installing SD card device (0x%x)\n", error_code);
        _task_block();
    }
      /*ʹ���ڲ�IOCTL�������SD���Ƿ��Ѿ����� dx20121223*/
      uint_32  sd_param;
     
      sd_param = 0;
      if (ESDHC_OK != ioctl (com_handle, IO_IOCTL_ESDHC_GET_CARD, &sd_param))
      {
         printf("Error get SD card device type.\n");
          _task_block();
      }
      if (ESDHC_CARD_NONE != sd_param)
      {
         inserted = TRUE; // �Ѳ���SD��
         printf("inserted = 1 -> %d\n",(int)sd_param);
      }else 
      {
         inserted = FALSE; // ��SD��
         printf("inserted = 0 -> %d\n",(int)sd_param);
         
      }

    for (;;)/////////////////////////////////////////////////////////////////////////////ѭ��
    {


#if defined BSP_SDCARD_GPIO_PROTECT
        /* Get value of protect pin */
        readonly = lwgpio_get_value(&sd_protect);// �������Ƿ�ֻ��////////////////////////////////////////
#endif
#ifdef BSP_MPC8308RDB
        /* Set function as SD_CD which indicate that card is present in Present State Register */
        lwgpio_set_functionality(&sd_detect,BSP_SDCARD_DETECT_MUX_SD_CD);
        lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif
        if (last != inserted)
        {
            if (inserted)// �������
            {
               // _time_delay (200);// ������������ָ���ĺ�����
                /* Open the device which MFS will be installed on */
                sdcard_handle = fopen("sdcard:", 0);// ��SD��������
                if ( sdcard_handle == NULL )
                {
                    printf("Unable to open SD card device.\n");
                    _task_block();
                }

                /* Set read only flag as needed */
                param = 0;
                if (readonly)
                {
                    param = IO_O_RDONLY;
                }
                if (IO_OK != ioctl(sdcard_handle, IO_IOCTL_SET_FLAGS, (char_ptr) &param))
                {
                    printf("Setting device read only failed.\n");
                    _task_block();
                }

                /* Install partition manager over SD card driver */
                error_code = _io_part_mgr_install(sdcard_handle, partman_name, 0);// ��������
                if (error_code != MFS_NO_ERROR)
                {
                    printf("Error installing partition manager: %s\n", MFS_Error_text((uint_32)error_code));
                    _task_block();
                }

                /* Open partition manager */
                partman_handle = fopen(partman_name, NULL);// �򿪷���
                if (partman_handle == NULL)
                {
                    error_code = ferror(partman_handle);
                    printf("Error opening partition manager: %s\n", MFS_Error_text((uint_32)error_code));
                    _task_block();
                }

                /* Validate partition 1 */
                param = 1;
                error_code = _io_ioctl(partman_handle, IO_IOCTL_VAL_PART, &param);// ���Ʒ���
                if (error_code == MQX_OK)
                {

                    /* Install MFS over partition 1 */
                    error_code = _io_mfs_install(partman_handle, filesystem_name, param);/////��װ�ļ�ϵͳ�����ڷ���
                    if (error_code != MFS_NO_ERROR)
                    {
                        printf("Error initializing MFS over partition: %s\n", MFS_Error_text((uint_32)error_code));
                        _task_block();
                    }

                } else {

                    /* Install MFS over SD card driver */
                    error_code = _io_mfs_install(sdcard_handle, filesystem_name, (_file_size)0);////////��װ�ļ�ϵͳ���޷���
                    if (error_code != MFS_NO_ERROR)
                    {
                        printf("Error initializing MFS: %s\n", MFS_Error_text((uint_32)error_code));
                        _task_block();
                    }

                }

                /* Open file system */
                filesystem_handle = fopen(filesystem_name, NULL);// ���ļ�ϵͳ
                error_code = ferror (filesystem_handle);
                if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK))
                {
                    printf("Error opening filesystem: %s\n", MFS_Error_text((uint_32)error_code));
                    _task_block();
                }
                if ( error_code == MFS_NOT_A_DOS_DISK )
                {
                    printf("NOT A DOS DISK! You must format to continue.\n");
                }

                printf ("SD card installed to %s\n", filesystem_name);
                if (readonly)
                {
                    printf ("SD card is locked (read only).\n");
                }
            }
            else // �γ�����
            {
                /* Close the filesystem */
                if (MQX_OK != fclose (filesystem_handle))
                {
                    printf("Error closing filesystem.\n");
                    _task_block();
                }
                filesystem_handle = NULL;

                /* Uninstall MFS  */
                error_code = _io_dev_uninstall(filesystem_name);
                if (error_code != MFS_NO_ERROR)
                {
                    printf("Error uninstalling filesystem.\n");
                    _task_block();
                }

                /* Close partition manager */
                if (MQX_OK != fclose (partman_handle))
                {
                    printf("Unable to close partition manager.\n");
                    _task_block();
                }
                partman_handle = NULL;

                /* Uninstall partition manager  */
                error_code = _io_dev_uninstall(partman_name);
                if (error_code != MFS_NO_ERROR)
                {
                    printf("Error uninstalling partition manager.\n");
                    _task_block();
                }

                /* Close the SD card device */
                if (MQX_OK != fclose (sdcard_handle))
                {
                    printf("Unable to close SD card device.\n");
                    _task_block();
                }
                sdcard_handle = NULL;

                printf ("SD card uninstalled.\n");
                printf ("sd unOK dx.\n");/////////////////////////dx
            }
        }

        last = inserted;
       // _time_delay (200);// ������������ָ���ĺ����� 
        
//        #if defined BSP_SDCARD_GPIO_DETECT
//            #if BSP_SDCARD_GPIO_DETECT_INT
//            /****************dx 20121211************************************************************/
//             _lwevent_wait_ticks(&SD_Event,SD_EVENT_DETECT,FALSE,0);///////////�ȴ��¼�USB_EVENT
//             _lwevent_clear(&SD_Event,SD_EVENT_DETECT);
//           /****************************************************************************************/    
//            #endif
//        #endif
      /*ʹ��SD�ⲿ�˿ڲ���SD���Ƿ��Ѿ�����*/
#if defined BSP_SDCARD_GPIO_DETECT
        #ifdef BSP_MPC8308RDB
        /* Set function as GPIO to detect sdcard */
        lwgpio_set_functionality(&sd_detect,BSP_SDCARD_DETECT_MUX_GPIO);
        lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
        #endif
        #if ! BSP_SDCARD_GPIO_DETECT_INT
        
        inserted = !lwgpio_get_value(&sd_detect);// �������Ƿ����/////////////////////////////////////////
        #else
        /****************dx 20121211************************************************************/
         _lwevent_wait_ticks(&SD_Event,SD_EVENT_DETECT,FALSE,0);///////////�ȴ��¼�USB_EVENT
          _lwevent_clear(&SD_Event,SD_EVENT_DETECT);
         ioctl(port_file_sd_detect_int, GPIO_IOCTL_READ, (char_ptr) &sd_detect_int);
         inserted = !( sd_detect_int[0] & GPIO_PIN_STATUS);
       /****************************************************************************************/    
        #endif
            
#endif
        
       
    }/////////////////////////////////////////////////////////////////////////////ѭ��
}

/* EOF */
