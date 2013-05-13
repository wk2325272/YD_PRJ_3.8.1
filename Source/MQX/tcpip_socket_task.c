
#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>
#include <string.h>


void tcp_socket_task( uint_32 val)
{
  sockaddr_in    laddr, raddr;
  uint_32        sock, listensock;
  uint_32        error;
  uint_16        rlen;
  uint_16        sin_port_is = 3333; 
  /* Set up the TCP port : */
  laddr.sin_family      = AF_INET;
  laddr.sin_port        = sin_port_is;
  laddr.sin_addr.s_addr = INADDR_ANY;
  /* Create a stream socket: */
  sock  = socket(PF_INET, SOCK_STREAM, 0);// 1.创建包，类型
  if (sock  == RTCS_SOCKET_ERROR) 
  {
    printf("\nFailed to create the stream socket.");
    _task_block();
  }
  /* Bind the stream socket to a TCP port: */
  error  = bind(sock, &laddr, sizeof(laddr));// 2.绑定包，端口等
  if (error  != RTCS_OK) 
  {
    printf("\nFailed to bind the stream socket - 0x%lx", error);
    _task_block();
  }
  /* Set up the stream socket to listen on the TCP port: */
  error = listen(sock, 0);// 3.被动连接即服务器,监听
  if (error != RTCS_OK) {
    printf("\nlisten() failed - 0x%lx", error);
    _task_block();
  }
  listensock = sock;
  printf("\n\nSocket Server is active on port %u.\n",(uint_16)sin_port_is);
  for (;;)
  {
    sock = RTCS_selectset(&listensock,1,0);//RTCS_selectall(0);// 4.等待任何或专用socket活动套接字,
    if (sock == listensock)
    {
      /* Connection requested; accept it. */
      rlen = sizeof(raddr);
      sock = accept(listensock, &raddr, &rlen); // 5.被动连接即服务器，接受
      if (sock == RTCS_SOCKET_ERROR) 
      {
        printf("\naccept() failed, error 0x%lx",
               RTCS_geterror(listensock));
        continue;
      }
      /* Send back a quote: */   
      send(sock, "\nYOU ARE MY BABY!! ", strlen("\nYOU ARE MY BABY! "), 0);// 6.发送包
      connect(sock, &raddr, rlen);
      _time_delay(1000);
     // int cnt=100;
#define TCPIP_SOCKET_DATA_LEN      100
      char socket_buf[TCPIP_SOCKET_DATA_LEN+1]="";
      socket_buf[TCPIP_SOCKET_DATA_LEN] = '\0';
      char socket_s[10]="send";
      while(1)
      {
        /* wk @130331 --> recv()函数是事件触发机制的，如果调用了此函数，需要外部事件触发，此任务才能激活*/
        if (recv(sock, &socket_buf, TCPIP_SOCKET_DATA_LEN, 0) == RTCS_ERROR) // 6.接收包,出错则关连接
        {
          break;
        } 
        
        send(sock, socket_s, strlen(socket_s), 0);///////////回显for test
        printf("%s",socket_buf);////////////////////回显for test
       // cnt--;
      }
      shutdown(sock, FLAG_CLOSE_TX);// 7.关包
    } 
    else
    {
      printf("\n TcpIP port socket is not right! ");
    }
    
  }
  
  
  
}

