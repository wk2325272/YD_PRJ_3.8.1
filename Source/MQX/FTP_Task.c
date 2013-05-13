#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>
#include <ftpd.h>
//#include "demo.h"
#include "Shell_Task.h"
#include "FTP_Task.h"

#define _FTP_DBUG_
#include <shell.h>

void FTP_task( uint_32 temp)
{
//   _ip_address       enet_ipaddr = ENET_IPADDR;
//   _ip_address       enet_ipmask = ENET_IPMASK;
//   uint_32           enet_device = 0;
   _enet_address     address;
   _enet_handle      ehandle;
   _rtcs_if_handle   ihandle;
   uint_32           error;
  
#ifdef _FTP_DBUG_
   printf("\n----------   FTP_Task  ----------\n");
   printf("\n----------             ----------\n");
   printf("\n----------             ----------\n");
   printf("\n----------      END    ----------\n");
#endif    
   
#if DEMO_USE_POOLS && defined(DEMO_RTCS_POOL_ADDR) && defined(DEMO_RTCS_POOL_SIZE)
    /* use external RAM for RTCS buffers */
    _RTCS_mem_pool = _mem_create_pool((pointer)DEMO_RTCS_POOL_ADDR, DEMO_RTCS_POOL_SIZE);
#endif
    printf("\n welecome2 ~! \n");///////////////////////dx
   /* runtime RTCS configuration */
   _RTCSPCB_init = 4;
   _RTCSPCB_grow = 2;
   _RTCSPCB_max = 20;
   _RTCS_msgpool_init = 4;
   _RTCS_msgpool_grow = 2;
   _RTCS_msgpool_max  = 20;
   _RTCS_socket_part_init = 4;
   _RTCS_socket_part_grow = 2;
   _RTCS_socket_part_max  = 20;

   if (RTCS_create()== RTCS_OK) {
                         
   ENET_get_mac_address(BSP_DEFAULT_ENET_DEVICE,ENET_IPADDR,address);
   error = ENET_initialize(BSP_DEFAULT_ENET_DEVICE, address, 0, &ehandle);

   error = RTCS_if_add(ehandle, RTCS_IF_ENET, &ihandle);
   error = RTCS_if_bind(ihandle, ENET_IPADDR, ENET_IPMASK);
 
   // Direct call to FTP Server so that we become the FTP Server
   FTPd_init("FTP_server", 7, 5000 ); //creates new task
   _task_create(0, 4, 0);  // wk --> 建立 TCP/IP socket 任务
   }
}

char FTPd_rootdir[] = {"u:\\"};//dx  c:
const FTPd_COMMAND_STRUCT FTPd_commands[] = {


   { "abor", FTPd_unimplemented },
   { "acct", FTPd_unimplemented },
   { "cdup", FTPd_cdup },
   { "cwd",  FTPd_cd   },        
   { "feat", FTPd_feat },       
   { "help", FTPd_help },       
   { "dele", FTPd_dele },       
   { "list", FTPd_list },       
   { "mkd",  FTPd_mkdir},       
   { "noop", FTPd_noop },
   { "nlst", FTPd_nlst },       
   { "opts", FTPd_opts },
   { "pass", FTPd_pass },
   { "pasv", FTPd_pasv },
   { "port", FTPd_port },
   { "pwd",  FTPd_pwd  },       
   { "quit", FTPd_quit },
   { "rnfr", FTPd_rnfr },
   { "rnto", FTPd_rnto },
   { "retr", FTPd_retr },
   { "stor", FTPd_stor },
   { "rmd",  FTPd_rmdir},       
   { "site", FTPd_site },
   { "size", FTPd_size },
   { "syst", FTPd_syst },
   { "type", FTPd_type },
   { "user", FTPd_user },
   { "xcwd", FTPd_cd    },        
   { "xmkd", FTPd_mkdir },       
   { "xpwd", FTPd_pwd   },       
   { "xrmd", FTPd_rmdir },       
   { NULL,   NULL } 
};