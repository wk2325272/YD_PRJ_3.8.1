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
* $FileName: Shell_Task.c$
* $Version : 3.0.6.0$
* $Date    : Jun-9-2009$
*
* Comments:
*
*   
*
*END************************************************************************/


#include "FTP_Task.h"
#include "Shell_Task.h"
#define _SHELL_DBUG_

const SHELL_COMMAND_STRUCT Shell_commands[] = {   
   { "cd",        Shell_cd },      //0
   { "copy",      Shell_copy },   //1
   { "create",    Shell_create }, //2
   { "del",       Shell_del },    //3     
   { "disect",    Shell_disect},    //4  
   { "dir",       Shell_dir },   //5
   { "df",        Shell_df },   //6
   { "exit",      Shell_exit },   //7
   { "format",    Shell_format },    //8  
   { "help",      Shell_help }, //9
   { "mkdir",     Shell_mkdir },   //10  
   { "pwd",       Shell_pwd },    //11   
   { "read",      Shell_read },    //12  
   { "ren",       Shell_rename },   //13 
   { "rmdir",     Shell_rmdir },  //14
   { "sh",        Shell_sh },  //15
   { "type",      Shell_type },  //16
   { "write",     Shell_write }, //17
//   { "writebuf",  Shell_write_buf },
   {"df_s",  Shell_search_file_r}, // wk @130331-->  查找文件
   {"update",  Shell_update}, // wk @130331-->  更新flash
   { "?",         Shell_command_list },    
   { NULL,        NULL } 
};


/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

void Shell_Task(uint_32 temp)
{ 

 #ifdef _SHELL_DBUG_
   printf("\n----------  Shell_Task  ----------\n");
   printf("\n----------             ----------\n");
   printf("\n----------             ----------\n");
   printf("\n----------      END    ----------\n");
   
   delay_ms(2000);
   /* wk --> test shell_create function */
//   char_ptr create[]={"test","kk3.txt"},write[]={"write","kk3.txt","10"};  // wk --> test array of USB
//         /*      wk --> test  function of USB */
//      Shell_create(2,create); 
//      Shell_write(3,write);
   
#endif  
   /* Run the shell on the serial port */
   for(;;)  {
      Shell(Shell_commands, NULL);
//       Shell(FTPd_commands, NULL);
      printf("Shell exited, restarting...\n");
     /*      wk --> test  function of USB */
//      Shell_create(2,create); 
//      Shell_write(3,write);
   }
}
 
/* EOF */
