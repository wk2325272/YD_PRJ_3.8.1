//#ifndef _SEHLL_TASK_h_
//#define _SEHLL_TASK_h_
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
* $FileName: MFS_USB.h$
* $Version : 3.0.4.0$
* $Date    : Nov-24-2008$
*
* Comments:
*
*   
*
*END************************************************************************/

//#include <mqx.h>
//#include <bsp.h>
//#include <lwevent.h>
//#include <message.h>
//#include <mfs.h>
//#include <part_mgr.h>
//#include <usbmfs.h>
#include <rtcs.h>
#include <ftpd.h> 

  
#ifndef ENET_IPADDR
   #define ENET_IPADDR  IPADDR(49,123,72,248) //IPADDR(169,254,3,3)   //dx // 
#endif

#ifndef ENET_IPMASK
   #define ENET_IPMASK  IPADDR(255,255,255,0) //dx   // IPADDR(255,255,0,0) 
#endif
extern void FTP_Task(uint_32);

//#endif /* _demo_h_ */



