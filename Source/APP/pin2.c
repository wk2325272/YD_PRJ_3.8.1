/*******************************************************************************
* File Name        : PIN2.c
* Auth or             : WK
* Vers ion            : V1.0.0
* Date                :
* Description        : 此任务用于处理 MAINTASK 与 ShellTASK 之间的转换
                        通过按键 1 中断中的事件与此任务进行通信
*******************************************************************************/

#include "includes.h"

void PIN2(uint_32 temp)
{
  pointer ppin2_event;
  
   _event_create_auto_clear("pin2_event");
   _event_open("pin2_event", &ppin2_event);
   
   while(TRUE)
   {
      _event_wait_all(ppin2_event,0x04,0);
      
      /* wk --> 启动 Shell */
      _mqx_uint oldP1=12,oldP2=10;uint_32 id;
      static uchar flag=0;
      id=0x10002;
      if( flag == 0 )
      {
        _task_set_priority(id+1,10,&oldP1);  // 切换SHELL任务的优先级为 10
        flag=1;
        printf("flag=%d\n",flag);
      }
      else if(flag == 1)
      {
//        _task_abort(0x10005);  // wk --> 销毁任务。 这里不能销毁，因为后面还学要在启动任务
       /*
        ** wk --> 将任务挂起。
        ** 注意：必须将任务挂起才能改变任务的优先级
        */
        _taskq_suspend_task(id+1,NULL);  // 将任务挂起
       
        _task_set_priority(id+1,12,&oldP2); // 将SHELL的任务的优先级降到 12
        flag=0;
        printf("flag=%d\n",flag);
      }
      
      printf("id=%d\n",id);
     /* test end */
   }
}