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
* $FileName: i2s_audio.h$
* $Version : 3.8.3.1$
* $Date    : Feb-13-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the I2S I/O functions.
*
*END************************************************************************/


#ifndef _i2s_audio_h_
#define _i2s_audio_h_ 1

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/*
** IOCTL calls specific to audio devices
*/
#define IO_TYPE_AUDIO                           0x16
#define IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT _IO   (IO_TYPE_AUDIO, 0x01)
#define IO_IOCTL_AUDIO_GET_IO_DATA_FORMAT _IO   (IO_TYPE_AUDIO, 0x02)

/*
** Audio deformat defaults
*/
#define AUDIO_DEFAULT_ENDIAN        AUDIO_BIG_ENDIAN
#define AUDIO_DEFAULT_ALIGNMENT     AUDIO_ALIGNMENT_RIGHT
#define AUDIO_DEFAULT_BITS          AUDIO_BIT_SIZE_MAX

/*
** Audio data aligment
*/
#define AUDIO_ALIGNMENT_RIGHT   0x00
#define AUDIO_ALIGNMENT_LEFT    0x01

/*
** Audio data endianity
*/
#define AUDIO_BIG_ENDIAN    0x00
#define AUDIO_LITTLE_ENDIAN 0x01

/*
** Data bit size limits
*/
#define AUDIO_BIT_SIZE_MIN 8
#define AUDIO_BIT_SIZE_MAX 32

/*
** Audio error codes
*/
#define AUDIO_ERROR_BASE                (DRIVER_ERROR_BASE | 0x1000)
#define AUDIO_ERROR_INVALID_IO_FORMAT   (AUDIO_ERROR_BASE | 0x01)
/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

typedef struct audio_data_format
{
    /* Endian of input data */
    uint_8 ENDIAN;

    /* Aligment of input data */
    uint_8 ALIGNMENT;

    /* Bit size of input data */
    uint_8 BITS;

    /* Sample size in bytes */
    uint_8 SIZE;

    /* Number of channels */
    uint_8 CHANNELS;

} AUDIO_DATA_FORMAT, _PTR_ AUDIO_DATA_FORMAT_PTR;

#endif

/* EOF */
