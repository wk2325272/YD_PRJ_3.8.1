/*******************************************************************************
* File Name        : PIN2.c
* Auth or             : WK
* Vers ion            : V1.0.0
* Date                :
* Description        : ���������ڴ��� MAINTASK �� ShellTASK ֮���ת��
                        ͨ������ 1 �ж��е��¼�����������ͨ��
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
      
      /* wk --> ���� Shell */
      _mqx_uint oldP1=12,oldP2=10;uint_32 id;
      static uchar flag=0;
      id=0x10002;
      if( flag == 0 )
      {
        _task_set_priority(id+1,10,&oldP1);  // �л�SHELL��������ȼ�Ϊ 10
        flag=1;
        printf("flag=%d\n",flag);
      }
      else if(flag == 1)
      {
//        _task_abort(0x10005);  // wk --> �������� ���ﲻ�����٣���Ϊ���滹ѧҪ����������
       /*
        ** wk --> ���������
        ** ע�⣺���뽫���������ܸı���������ȼ�
        */
        _taskq_suspend_task(id+1,NULL);  // ���������
       
        _task_set_priority(id+1,12,&oldP2); // ��SHELL����������ȼ����� 12
        flag=0;
        printf("flag=%d\n",flag);
      }
      
      printf("id=%d\n",id);
     /* test end */
   }
}