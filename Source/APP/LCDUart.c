/*******************************************************************************
* File Name        : LCDUart.c
* Author             : LB&TX
* Version            : V1.0.0
* Date                :
* Description        :Һ����ʹ�ô���UartA1��ʼ��;Һ��ʹ�õĴ�����LCDDriver.c���й���
                     ѡ��SMCLK��Ϊʱ��Դ��24MHz�²�����Ϊ115200
                     switch���ַ�֧�ɵ���ΪDis_PicID=Touch_key+X
*******************************************************************************/
#define    LCDUart_GLOBALS

#include "includes.h"

#define _LCDUart_DBUG_   //wk --> ���ԣ��Ƿ���Ҫ���������Ϣ

extern uchar read_buffer[8];
extern U8 EVEnum_old;  // --> MenuView.c
/* wk @130409 --> �������� */
volatile U8 MenuSwFlg;//ҳ���л���־��Ĭ��ֵΪ0
volatile U8 Dis_PicID=0;
volatile U8 ViewKeyFlg;
volatile U8 SysSetKeyFlg;
volatile U8 EventKeyFlg;
volatile U8 HarmoGraphPhase;//����ѡ����ʾ�࣬Ĭ��ֵ1
volatile U8 HarmoGraphRange;//����ѡ����ʾг����Χ��Ĭ��ֵΪ1,��Ӧ1~26�Σ�2��Ӧ25~50
volatile U8 HarmoGraphUorder;//Ĭ��ֵ1�����ڿ���г����ѹ�����������ֵ��ʾ,
volatile U8 HarmoGraphIorder;//Ĭ��ֵ1�����ڿ���г�����������������ֵ��ʾ��
volatile U8 HarmoListShift;//Ĭ��ֵ0��г���б���ʾ�еĹ������Ƽ�
volatile U8 HarmoListPhase;//Ĭ��ֵ1��
volatile U8 HarmoListUorI;//Ĭ��ֵ1��
volatile U8 HarmoListRange;//Ĭ��ֵ1��
volatile U8 HarmoListAmporRatio;////Ĭ��ֵ1��г���б���ʾ����ʾ��ֵ�(1)���ߺ�����(2)��ѡ��
volatile U8 VIEWHoldFlg;//���ּ�Ĭ��Ϊ0��������ʱֵ��Ϊ1���ٴΰ���ʱֵ��Ϊ0��
volatile U8 EVEpage=0;//�¼���ҳ������Ϸ��·�
volatile U8 EVEline;//�¼��������ư�����ȷ���¼����м��ڼ����¼���
volatile U8 EVEfunflg;//�¼��������ư������ܱ�־��
volatile U8 OtherSetIndex;//��ֵΪ0�����ڴ�C�ļ���ʹ��
volatile U8 TimeSetIndex;//��ֵΪ6�����ڴ�C�ļ���ʹ��
//volatile U8 RemovDevFlg;
//volatile U8 SoundOffOn;
//volatile U8 BlightOffOn;
//volatile struct SS SysSet;//��LCDUART.c�и�ֵ����MenuView.c��ʹ��
volatile SysStr SysSet; //��LCDUART.c�и�ֵ����MenuView.c��ʹ��
volatile U8 InitAck=0;
volatile U8 EvntPgUpFlg=0;
/* Uart initialization for send data*/
/* 
** �������� 
** ���ߣ�
** ��ڲ�����
** ����˵�������ڳ�ʼ�����ж�ģʽΪ�˽��մ���������
*/
void UartTouch_init()
{
   MQX_FILE_PTR uart_touch = NULL;  // wk --> 20130107
//   ISR_UART_STRUCT uart_isr;
   
   uart_touch  = fopen( "ittyb:", NULL );  // Һ��ʹ�õ��Ǵ��� ttyb
//   uart_touch  = fopen( "ittye:", NULL );  // wk --> test event of uart
     if( uart_touch == NULL )
   {
      /* device could not be opened */
      _task_block();
   }
//   _int_install_isr(BSP_UART_INTERRUPT_VECTOR, new_uart_isr,
//                    &uart_isr);  
}

/* 
** �������� 
** ���ߣ�
** ��ڲ�����
** ����˵���������жϷ������
*/
void new_uart_isr(pointer user_uart_ptr)
{
   UART_MemMapPtr     sci_ptr; 
   sci_ptr=(UART_MemMapPtr)UART4_BASE_PTR;  //wk
   volatile int_32                        c;
   
#if 1 // wk --> data rec
 static   uchar  i=0,flag=0;
   if (sci_ptr->S1 & UART_S1_RDRF_MASK) {
      c = sci_ptr->D;
      
      if(c == 0xaa || flag==1)  //��һ�������� 0xaa ��ʼ��־�ſ�ʼ����
      {
          read_buffer[i++]= c ;
          flag=1;
      }   
   }
#endif  // wk --< data rec
   
    if(i == 8) 
    {
      i=0;flag=0;     
      pointer puart_event;
      
#ifdef _UART_DBUG_        //�����¼�
      if(_event_open("uart_event",&puart_event) != MQX_OK)
      {
        printf("\n Open event failed of serl_int_kuart.c");
      }
      else 
        printf("\n Open event OK of serl_int_kuart.c");
      
      if (_event_set(puart_event,0x01) != MQX_OK) {
         printf("\n Set Event failed of serl_int_kuart.c");
      }  
      else
         printf("\n Set Event OK of serl_int_kuart.c");
#endif
      
#ifndef _UART_DBUG_
      _event_open("uart_event",&puart_event);
      _event_set(puart_event,0x01);  // �����¼�
#endif
    }
}

/* 
** �������� 
** ���ߣ�
** ��ڲ�����
** ����˵��������������
*/
void LCDTouchSel_Task(uint_32 param)
{
#define EVENTTEST 1
  
#if EVENTTEST
  // wk --> event test
  pointer puart_event;
#ifdef _LCDUart_DBUG_ 
  printf("\n----------LCDTouchSel_Task----------\n");
  printf("\n----------             ----------\n");
  printf("\n----------             ----------\n");
  printf("\n----------      END    ----------\n");
#endif 
  
  
#ifdef _LCDUart_DBUG_
//  if(_event_create("uart_event") != MQX_OK){  // wk --> event of 1
    if(_event_create_auto_clear("uart_event") != MQX_OK){  // wk --> event of 2
    printf("\n Make uart event failed");
    _task_block();
  }
  else
    printf("\n Make event OK");
  
  if(_event_open("uart_event", &puart_event) != MQX_OK){
    printf("\nOpen uart event failed ");
    _task_block();
  }  
  else
    printf("\n Open uart event OK");
#endif  // _LCDUart_DBUG_
  
#ifndef _LCDUart_DBUG_
  _event_create("uart_event");
  _event_open("uart_event", &puart_event);
#endif // _LCDUart_DBUG_
  
  while(TRUE)
  {
  _event_wait_all(puart_event,0x01,0);
//  _event_clear(puart_event,0x01);  // wk --> event of 1
  
  #define tst_flg 0
  
#if tst_flg // test code  --> tst_flg = 1  // ��ӡ������յĴ���������
 static long cs=0,flag = 0;
  if( read_buffer[0]!=0xaa || read_buffer[1]!= 0x78 ||read_buffer[4]!=0xcc
      || read_buffer[5]!= 0x33 ||read_buffer[6]!=0xc3 ||read_buffer[7]!=0x3c)
  {_task_block();
  if(flag ==0 ) 
  {
   printf("cs = %d\n",cs);
   for(uchar i=0;i<8;i++)
  {
    printf("read_buffer[%d] = %x\n",i,read_buffer[i]);
  } 
  }
  flag=1;}
  else if(flag == 0)
  {
  for(uchar i=0;i<8;i++)
  {
    printf("read_buffer[%d] = %x\n",i,read_buffer[i]);
  } 
  printf("\n");
  cs++;
  }
#endif // end --> tst_flg
  
#if !tst_flg  // tst_flg = 0
  if(read_buffer[0] == 0xAA && read_buffer[1] == 0x78)
  {
    if(read_buffer[2] == 0x02)
    {
      ViewKeyFlg=1;
      LCDUartView(read_buffer[3]);   //������ʾ������ת������
    }
    else if(read_buffer[2] == 0x01)
    {
      SysSetKeyFlg=1;
      LCDUartSET(read_buffer[3]);   //������ʾ������ת����
    }
    else if(read_buffer[2] == 0x04)
    {
      EventKeyFlg=1;
      LCDUartEVENT(read_buffer[3]);   //�¼���ʾ������ת������
    }
  }
#endif  // end --> !tst_flg
  }
  // wk --> event test end
#endif //EVENTTEST  
  
#if !EVENTTEST        
#define tst_flg 0
#if tst_flg // test code  --> tst_flg = 1  // ��ӡ������յĴ���������
 static long cs=0,flag = 0;
  if( read_buffer[0]!=0xaa || read_buffer[1]!= 0x78 ||read_buffer[4]!=0xcc
      || read_buffer[5]!= 0x33 ||read_buffer[6]!=0xc3 ||read_buffer[7]!=0x3c)
  {_task_block();
  if(flag ==0 ) 
  {
   printf("cs = %d\n",cs);
   for(uchar i=0;i<8;i++)
  {
    printf("read_buffer[%d] = %x\n",i,read_buffer[i]);
  } 
  }
  flag=1;}
  else if(flag == 0)
  {
  for(uchar i=0;i<8;i++)
  {
    printf("read_buffer[%d] = %x\n",i,read_buffer[i]);
  } 
  printf("\n");
  cs++;
  }
#endif // end --> tst_flg
  
#if !tst_flg  // tst_flg = 0
  if(read_buffer[0] == 0xAA && read_buffer[1] == 0x78)
  {
    if(read_buffer[2] == 0x02)
    {
      ViewKeyFlg=1;
      LCDUartView(read_buffer[3]);   //������ʾ������ת������
    }
    else if(read_buffer[2] == 0x01)
    {
      SysSetKeyFlg=1;
      LCDUartSET(read_buffer[3]);   //������ʾ������ת����
    }
    else if(read_buffer[2] == 0x04)
    {
      EventKeyFlg=1;
      LCDUartEVENT(read_buffer[3]);   //�¼���ʾ������ת������
    }
  }
#endif  // end --> !tst_flg
#endif // !EVENTTEST
}


/*******************************************************************************
* ��  ��  ��      : LCDUartView
* ��      ��      : LCDUart�������ж�������ʾ�����ж�
* ��      ��      : Touch_key.
* ��      ��      : ��.
*******************************************************************************/
void LCDUartView(U8 Touch_key)   // wk --> ������ʾ ������ת������
{
    if(Touch_key<10|| Touch_key==255)  //  wk --> �ڲ�ҳ���л������Ƿ�����ҳ
    {
        MenuSwFlg=1; //  wk -->ҳ���л���־ 
        TimeFlg=1; //ҳ���л�����ȫ��ʾʱ��
        VIEWHoldFlg=0; //��ֹҳ���л��󣬱��ֹ����Դ��ڵ�����ʾ���ֱ�־
    }
    switch(Touch_key)//�ɵ���ΪDis_PicID=Touch_key+X
    {
    case 0:
        Dis_PicID=MenuViewWavVolCur;  // wk --> ��ѹ����������ʾ
        break;
    case 1:
        Dis_PicID=MenuViewWavVol; //  wk --> ��ѹ������ʾ
        break;
    case 2:
        Dis_PicID=MenuViewWavCur;  // wk --> ����������ʾ
        break;
    case 3:
        Dis_PicID=MenuViewVector;  // wk --> ʸ��ͼ��ʾ
        break;
    case 4:
        Dis_PicID=MenuViewListMeasure;  // wk --> ���ܼ�������
        break;
    case 5:
        Dis_PicID=MenuViewListQuality1;  // wk --> ������������ 1
        break;
    case 6:
        Dis_PicID=MenuViewHarmoGraph1;  // wk --> г����״ͼ
        break;
    case 7:
        Dis_PicID=MenuViewHarmoList1; // wk --> г���б���ʾ
        break;
    case 8:
        Dis_PicID=MenuViewListQuality2; // wk --> ������������ 2
        break;   
    case 9:
        Dis_PicID = MenuStatus; // wk -- > ����״̬
        break;
   /* WK --> ������ʾг����״ͼ����  */  
    case 0x60: // wk --> ��λѡ��
      HarmoGraphPhase<3 ? (++HarmoGraphPhase): (HarmoGraphPhase=1);  
      break;
    case 0x61: // wk --> г��ѡ��
      HarmoGraphRange==1 ? (HarmoGraphRange=2):(HarmoGraphRange=1); //WK --> г��������1-25��26-50֮���л�
      /* WK --> �������л�ʱ��г��������Ӧ�л�*/
      HarmoGraphRange==1 ? (HarmoGraphUorder=1):(HarmoGraphUorder=25);  
      HarmoGraphRange==1 ? (HarmoGraphIorder=1):(HarmoGraphIorder=25);
      break;
    case 0x62: // wk --> ��ѹг���������� + ��
      HarmoGraphUorder++;
            if (HarmoGraphRange==1)  //WK --> г���� 1-25����
            {
                if(HarmoGraphUorder==27)
                {
                    HarmoGraphUorder=1;
                }
            }
            else   //WK --> г���� 26-50 ����
            {
                if(HarmoGraphUorder==51)
                {
                    HarmoGraphUorder=25;
                }
            }
      break;
    case 0x63: //wk -- > ��ѹг���������� - ��
      HarmoGraphUorder--;
            if (HarmoGraphRange==1)
            {
                if(HarmoGraphUorder==0)
                {
                    HarmoGraphUorder=26;
                }
            }
            else
            {
                if(HarmoGraphUorder==24)
                {
                    HarmoGraphUorder=50;
                }
            }
      break;
    case 0x64: // wk --> ����г���������� + ��
      HarmoGraphIorder++;
            if (HarmoGraphRange==1)
            {
                if(HarmoGraphIorder==27)
                {
                    HarmoGraphIorder=1;
                }
            }
            else
            {
                if(HarmoGraphIorder==51)
                {
                    HarmoGraphIorder=25;
                }
            }
      break;
    case 0x65: // wk --> ����г���������� - ��
      HarmoGraphIorder--;
            if (HarmoGraphRange==1)
            {
                if(!HarmoGraphIorder)
                {
                    HarmoGraphIorder=26;
                }
            }
            else
            {
                if(HarmoGraphIorder==24)
                {
                    HarmoGraphIorder=50;
                }
            }
      break;
      /* wk --> г����״ͼ END */
      
    /* WK --> ������ʾг����״ͼ����  */ 
    case 0x70:  // WK --> U/I
      (HarmoListUorI==1) ? (HarmoListUorI=2):(HarmoListUorI=1); // 1--> U 2-->I
      break;
    case 0x71:  // WK --> ����  1:��ֵ�  2:������
      (HarmoListAmporRatio==1)?(HarmoListAmporRatio=2):(HarmoListAmporRatio=1);
      break;
    case 0x72:  // WK --> ����   1:1-26   2:25-50
      (HarmoListRange==1)? (HarmoListRange=2):(HarmoListRange=1); 
       MenuSwFlg=1; // ��ҳ֮���л�
      (Dis_PicID==MenuViewHarmoList1)?(Dis_PicID=MenuViewHarmoList2):(Dis_PicID=MenuViewHarmoList1);
      break;
    case 0x73: // WK --> ��ѡ
       HarmoListPhase<3 ? (++HarmoListPhase): (HarmoListPhase=1);
      break;
    /* WK --> ������ʾг����״ͼ����  END*/ 
        
    case 254:       // wk --> hold ����
        if (VIEWHoldFlg==0)
        {
            VIEWHoldFlg=1;
            ViewKeyFlg=0;
            YADA_98(510,453,0x22,0x81,0x02,0xf800,0x0000,"��",0);
        }
        else
        {
            VIEWHoldFlg=0;
            YADA_71(Dis_PicID,510,453,532,475,510,453);
        }
        break;
    case 255:
        Dis_PicID=MenuTop;
        break;
    default:
        break;
    }
}
/*******************************************************************************
* ��  ��  ��      : LCDUartSET
* ��      ��      : LCDUart�������ж�ϵͳ���ð����ж�
* ��      ��      : Touch_key.
* ��      ��      : ��.
*
* ��       ��     : WK
* ʱ       ��     : 2013-03-13
* ��       ��     : ����2013-03-08���� -->����� ���ָ���������   ��û�����
*******************************************************************************/
void LCDUartSET(U8 Touch_key)   // wk --> ϵͳ���� ������ת����
{
    if(Touch_key<3||Touch_key==255)
    {
        MenuSwFlg=1;//�л�ҳ��
        TimeFlg=1;//ҳ���л�����ȫ��ʾʱ��
         //ϵͳ���ð�����ʹ������ɱ�־����
    }         
    switch(Touch_key)
    {
    case 0: //ϵ������//��SwFlg��FuncFlg��DataFlg��SaveFlg��ParaIndex��DataCnt����ֵ
        // VIEWHoldFlg=0;
        Dis_PicID=MenuParaSET;
        OtherSetIndex=0; // WK --> ?
        TimeSetIndex=9;  // WK --> ?
        SysSet.SwFlg=1;
        SysSet.ParaIndex=0;
        SysSet.DataCnt=0;
        break;
    case 1:                       /*�¼�����*/
        //VIEWHoldFlg=0;
        Dis_PicID=MenuEventSET;
        SysSet.SwFlg=1;
        SysSet.EvntIndex=0;
        SysSet.DataCnt=0; 
        break;
        
    case 2:  /* �ָ��������� */
        Dis_PicID=MenuInitSET;
        SysSet.SwFlg=1;
        break;
        
//    case 18:   // wk --> �ָ���������
    case 0x31:                  /*�¼����������*/
        InitAck=1;
        break;
        
    case 0x32:                  /*�¼����������*/
        //InitNoAck=1;
        MenuSwFlg=1; //  wk -->ҳ���л���־
        TimeFlg=1; //ҳ���л�����ȫ��ʾʱ��
        Dis_PicID=MenuParaSET;
        break;
        /**********************************************************************
        ** WK --> ϵͳ�������ý��水��  
        **********************************************************************/
    case 16:                       /*ϵͳ���������л���*/
        OtherSetIndex++ ;
        SysSet.FuncFlg=1;  // WK --> �й���ƶ�
        if(OtherSetIndex==9||TimeSetIndex!=9)//��ʱ�����л���������ʱ�������ص���һ��
        {
            OtherSetIndex=0;
            TimeSetIndex=9;  //ʵ��ֻ����TimeSetIndex!=6ʱ����ֵ��Ϊ6
        }
        SysSet.ParaIndex=OtherSetIndex;
       
        break;
    case 17:
        SysSet.FuncFlg=1;
        if(OtherSetIndex==0||TimeSetIndex!=9)//��ʱ�����л���������ʱ�������ص�������
        {
            OtherSetIndex=9;
            TimeSetIndex=9;
        }
        OtherSetIndex-- ;
        SysSet.ParaIndex=OtherSetIndex;
        
        break;
     case 19:
       SysSet.ParaSaveFlg=1;      // WK --> ����ʵ�ִ��о�
       break;
     
    case 0x60:  // WK --> TCP/IP����      0 �ر�  1 ����
      SysSet.SwitchSet[0]==0?(SysSet.SwitchSet[0]=1):(SysSet.SwitchSet[0]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x61:  // WK --> 485����
      SysSet.SwitchSet[1]==0?(SysSet.SwitchSet[1]=1):(SysSet.SwitchSet[1]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x62:  // WK --> 485����
      SysSet.SwitchSet[2]==0?(SysSet.SwitchSet[2]=1):(SysSet.SwitchSet[2]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x63:  // WK --> GPS��ʱ
      SysSet.SwitchSet[3]==0?(SysSet.SwitchSet[3]=1):(SysSet.SwitchSet[3]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x64:  // WK --> ��������
      SysSet.SwitchSet[4]==0?(SysSet.SwitchSet[4]=1):(SysSet.SwitchSet[4]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x65:  // WK --> ��������
      SysSet.SwitchSet[5]==0?(SysSet.SwitchSet[5]=1):(SysSet.SwitchSet[5]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x66:  // WK --> �ƶ��洢
      SysSet.SwitchSet[6]==0?(SysSet.SwitchSet[6]=1):(SysSet.SwitchSet[6]=0);
      SysSet.SwitchSet[7]=1;
      break;        
     /******************************** end *********************************/
       
     /**********************************************************************
      ** WK --> ϵͳ�¼����ý��水��  
      **********************************************************************/  
    case 64:               // WK --> �¼���������       
        // WK --> ������  
        SysSet.FuncFlg=1;
        if(SysSet.EvntIndex <7 )
          (SysSet.EvntIndex==6)? (SysSet.EvntIndex=0): (++SysSet.EvntIndex);  
        else
         (SysSet.EvntIndex==10)? (SysSet.EvntIndex=7):(++SysSet.EvntIndex);
        break;
    case 65:              // WK --> �¼���������         
          SysSet.FuncFlg=1;
          if(SysSet.EvntIndex <7 )
            (SysSet.EvntIndex==0)? (SysSet.EvntIndex=6):(--SysSet.EvntIndex);      
          else
            (SysSet.EvntIndex==10)?(SysSet.EvntIndex=7):(--SysSet.EvntIndex);   
        break;
    case 66:             // WK --> �¼���������         
        SysSet.FuncFlg=1;
        (SysSet.EvntIndex < 7) ? (SysSet.EvntIndex=7 ) : (SysSet.EvntIndex=0);
        break;     
    case 67:                       /*�����*/
        SysSet.EventSaveFlg=1;      // WK --> ����ʵ�ִ��о�
        break;
     /************************************* end******************************/
        
    case 24:
        break;               /*����������ȥ��С�����*/
    case 72:                  /*�¼�����С�����*/
        if(SysSet.DataCnt)
        {
            SysSet.Data[SysSet.DataCnt++]='.';
        }
        break;
    case 25:                  /*�������������*/
    case 73:                  /*�¼����������*/
        SysSet.DataFlg=1;
        SysSet.DataCnt=0;
        break;
    case 255:
        Dis_PicID=MenuTop;   // wk --> ����������
        break;
        
    default:
        if(Touch_key>31&&Touch_key<42)//ϵͳ���������µ�С����0-9
        {
            SysSet.DataFlg=1;
            if(SysSet.DataCnt<(SysSet.ParaIndex<4?(SysSet.ParaIndex==3?4:5):2))  // wk --> ������� 5 λ��
            {
                SysSet.Data[SysSet.DataCnt++]=Touch_key+16;
            }
            else
            {
                SysSet.DataCnt=0;    //�൱�������
            }
        }
        if(Touch_key>79&&Touch_key<90)//ϵͳ�¼������µ�С����0-9
        {
            SysSet.DataFlg=1;
            if(SysSet.DataCnt<5)
            {
                SysSet.Data[SysSet.DataCnt++]=Touch_key-32;
            }
            else
            {
                SysSet.DataCnt=0;    //�൱�������
            }
        }
        break;
    }
}
/*******************************************************************************
* ��  ��  ��      : LCDUartEVENT
* ��      ��      : LCDUart�������ж�ϵͳ���ð����ж�
* ��      ��      : Touch_key.
* ��      ��      : ��.
*******************************************************************************/
void LCDUartEVENT(U8 Touch_key)   //�¼���ʾ������ת������
{
    if(Touch_key<3||Touch_key==255)
    {
        MenuSwFlg=1;//�л�ҳ��
        TimeFlg=1;//ҳ���л�����ȫ��ʾʱ��
    }
    switch(Touch_key)
    {
    case 0:
        Dis_PicID=MenuEventMON;   //�¼����
        break;
    case 1:
        Dis_PicID=MenuEventList;  //�¼��б�
        EVEnum_old=0;
        break;
    case 2:
        Dis_PicID= MenuEventWave;  //�¼�����
        break;
    case 32:            //�¼��б��е��Ϸ�ҳ
        if(EVEpage>0)
        {
            EVEpage--;
        }
        else
        {
            EVEpage=0;
        }
        MenuSwFlg=1;
        EvntPgUpFlg=1;
        break;
    case 33:             //�¼��б��е��·�ҳ
        if(EVEpage==8)
        {
            EVEpage=0;
        }
        EVEpage++;
        MenuSwFlg=1;
        break;
    case 34:             //�¼��б��е�����
        if(EVEline>0)
        {
            EVEline--;
        }
        else
        {
            EVEline=0;
        }
        EVEfunflg=1;
        break;
    case 35:           //�¼��б��е�����
        if(EVEline==14)
        {
            EVEline=0;
        }
        EVEline++;
        EVEfunflg=1;
        break;
    case 255:
        Dis_PicID=MenuTop;
        break;
    default:
        break;
    }
}
