/*******************************************************************************
* File Name        : LCDUart.c
* Author             : LB&TX
* Version            : V1.0.0
* Date                :
* Description        :液晶所使用串口UartA1初始化;液晶使用的串口在LCDDriver.c中有关联
                     选择SMCLK作为时钟源，24MHz下波特率为115200
                     switch部分分支可调整为Dis_PicID=Touch_key+X
*******************************************************************************/
#define    LCDUart_GLOBALS

#include "includes.h"

#define _LCDUart_DBUG_   //wk --> 测试：是否需要输出调试信息

extern uchar read_buffer[8];
extern U8 EVEnum_old;  // --> MenuView.c
/* wk @130409 --> 变量定义 */
volatile U8 MenuSwFlg;//页面切换标志，默认值为0
volatile U8 Dis_PicID=0;
volatile U8 ViewKeyFlg;
volatile U8 SysSetKeyFlg;
volatile U8 EventKeyFlg;
volatile U8 HarmoGraphPhase;//用于选择显示相，默认值1
volatile U8 HarmoGraphRange;//用于选择显示谐波范围，默认值为1,对应1~26次，2对应25~50
volatile U8 HarmoGraphUorder;//默认值1，用于控制谐波电压具体次数的数值显示,
volatile U8 HarmoGraphIorder;//默认值1，用于控制谐波电流具体次数的数值显示，
volatile U8 HarmoListShift;//默认值0，谐波列表显示中的功能右移键
volatile U8 HarmoListPhase;//默认值1，
volatile U8 HarmoListUorI;//默认值1，
volatile U8 HarmoListRange;//默认值1，
volatile U8 HarmoListAmporRatio;////默认值1，谐波列表显示中显示幅值澹(1)或者含有率(2)的选择
volatile U8 VIEWHoldFlg;//保持键默认为0，键按下时值变为1，再次按下时值变为0；
volatile U8 EVEpage=0;//事件的页面键，上翻下翻
volatile U8 EVEline;//事件的上下移按键，确定事件的行即第几个事件。
volatile U8 EVEfunflg;//事件的上下移按键功能标志。
volatile U8 OtherSetIndex;//初值为0，仅在此C文件中使用
volatile U8 TimeSetIndex;//初值为6，仅在此C文件中使用
//volatile U8 RemovDevFlg;
//volatile U8 SoundOffOn;
//volatile U8 BlightOffOn;
//volatile struct SS SysSet;//在LCDUART.c中赋值，在MenuView.c中使用
volatile SysStr SysSet; //在LCDUART.c中赋值，在MenuView.c中使用
volatile U8 InitAck=0;
volatile U8 EvntPgUpFlg=0;
/* Uart initialization for send data*/
/* 
** 函数名： 
** 作者：
** 入口参数：
** 函数说明：串口初始化，中断模式为了接收触摸屏数据
*/
void UartTouch_init()
{
   MQX_FILE_PTR uart_touch = NULL;  // wk --> 20130107
//   ISR_UART_STRUCT uart_isr;
   
   uart_touch  = fopen( "ittyb:", NULL );  // 液晶使用的是串口 ttyb
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
** 函数名： 
** 作者：
** 入口参数：
** 函数说明：串口中断服务程序
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
      
      if(c == 0xaa || flag==1)  //第一个数据是 0xaa 开始标志才开始接收
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
      
#ifdef _UART_DBUG_        //设置事件
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
      _event_set(puart_event,0x01);  // 设置事件
#endif
    }
}

/* 
** 函数名： 
** 作者：
** 入口参数：
** 函数说明：触摸屏任务
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
  
#if tst_flg // test code  --> tst_flg = 1  // 打印输出接收的触摸屏数据
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
      LCDUartView(read_buffer[3]);   //数据显示按键跳转函数。
    }
    else if(read_buffer[2] == 0x01)
    {
      SysSetKeyFlg=1;
      LCDUartSET(read_buffer[3]);   //设置显示按键跳转函数
    }
    else if(read_buffer[2] == 0x04)
    {
      EventKeyFlg=1;
      LCDUartEVENT(read_buffer[3]);   //事件显示按键跳转函数。
    }
  }
#endif  // end --> !tst_flg
  }
  // wk --> event test end
#endif //EVENTTEST  
  
#if !EVENTTEST        
#define tst_flg 0
#if tst_flg // test code  --> tst_flg = 1  // 打印输出接收的触摸屏数据
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
      LCDUartView(read_buffer[3]);   //数据显示按键跳转函数。
    }
    else if(read_buffer[2] == 0x01)
    {
      SysSetKeyFlg=1;
      LCDUartSET(read_buffer[3]);   //设置显示按键跳转函数
    }
    else if(read_buffer[2] == 0x04)
    {
      EventKeyFlg=1;
      LCDUartEVENT(read_buffer[3]);   //事件显示按键跳转函数。
    }
  }
#endif  // end --> !tst_flg
#endif // !EVENTTEST
}


/*******************************************************************************
* 函  数  名      : LCDUartView
* 描      述      : LCDUart触摸屏中断数据显示按键判断
* 输      入      : Touch_key.
* 返      回      : 无.
*******************************************************************************/
void LCDUartView(U8 Touch_key)   // wk --> 数据显示 按键跳转函数。
{
    if(Touch_key<10|| Touch_key==255)  //  wk --> 内部页面切换或者是返回首页
    {
        MenuSwFlg=1; //  wk -->页面切换标志 
        TimeFlg=1; //页面切换后完全显示时间
        VIEWHoldFlg=0; //防止页面切换后，保持功能仍存在但不显示保持标志
    }
    switch(Touch_key)//可调整为Dis_PicID=Touch_key+X
    {
    case 0:
        Dis_PicID=MenuViewWavVolCur;  // wk --> 电压电流波形显示
        break;
    case 1:
        Dis_PicID=MenuViewWavVol; //  wk --> 电压波形显示
        break;
    case 2:
        Dis_PicID=MenuViewWavCur;  // wk --> 电流波形显示
        break;
    case 3:
        Dis_PicID=MenuViewVector;  // wk --> 矢量图显示
        break;
    case 4:
        Dis_PicID=MenuViewListMeasure;  // wk --> 电能计量参数
        break;
    case 5:
        Dis_PicID=MenuViewListQuality1;  // wk --> 电能质量参数 1
        break;
    case 6:
        Dis_PicID=MenuViewHarmoGraph1;  // wk --> 谐波柱状图
        break;
    case 7:
        Dis_PicID=MenuViewHarmoList1; // wk --> 谐波列表显示
        break;
    case 8:
        Dis_PicID=MenuViewListQuality2; // wk --> 电能质量参数 2
        break;   
    case 9:
        Dis_PicID = MenuStatus; // wk -- > 工作状态
        break;
   /* WK --> 数据显示谐波柱状图按键  */  
    case 0x60: // wk --> 相位选择
      HarmoGraphPhase<3 ? (++HarmoGraphPhase): (HarmoGraphPhase=1);  
      break;
    case 0x61: // wk --> 谐波选择
      HarmoGraphRange==1 ? (HarmoGraphRange=2):(HarmoGraphRange=1); //WK --> 谐波在区间1-25和26-50之间切换
      /* WK --> 在区间切换时，谐波次数相应切换*/
      HarmoGraphRange==1 ? (HarmoGraphUorder=1):(HarmoGraphUorder=25);  
      HarmoGraphRange==1 ? (HarmoGraphIorder=1):(HarmoGraphIorder=25);
      break;
    case 0x62: // wk --> 电压谐波次数功能 + 键
      HarmoGraphUorder++;
            if (HarmoGraphRange==1)  //WK --> 谐波在 1-25区间
            {
                if(HarmoGraphUorder==27)
                {
                    HarmoGraphUorder=1;
                }
            }
            else   //WK --> 谐波在 26-50 区间
            {
                if(HarmoGraphUorder==51)
                {
                    HarmoGraphUorder=25;
                }
            }
      break;
    case 0x63: //wk -- > 电压谐波次数功能 - 键
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
    case 0x64: // wk --> 电流谐波次数功能 + 键
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
    case 0x65: // wk --> 电流谐波次数功能 - 键
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
      /* wk --> 谐波柱状图 END */
      
    /* WK --> 数据显示谐波柱状图按键  */ 
    case 0x70:  // WK --> U/I
      (HarmoListUorI==1) ? (HarmoListUorI=2):(HarmoListUorI=1); // 1--> U 2-->I
      break;
    case 0x71:  // WK --> 类型  1:幅值澹  2:含有率
      (HarmoListAmporRatio==1)?(HarmoListAmporRatio=2):(HarmoListAmporRatio=1);
      break;
    case 0x72:  // WK --> 区间   1:1-26   2:25-50
      (HarmoListRange==1)? (HarmoListRange=2):(HarmoListRange=1); 
       MenuSwFlg=1; // 两页之间切换
      (Dis_PicID==MenuViewHarmoList1)?(Dis_PicID=MenuViewHarmoList2):(Dis_PicID=MenuViewHarmoList1);
      break;
    case 0x73: // WK --> 相选
       HarmoListPhase<3 ? (++HarmoListPhase): (HarmoListPhase=1);
      break;
    /* WK --> 数据显示谐波柱状图按键  END*/ 
        
    case 254:       // wk --> hold 按键
        if (VIEWHoldFlg==0)
        {
            VIEWHoldFlg=1;
            ViewKeyFlg=0;
            YADA_98(510,453,0x22,0x81,0x02,0xf800,0x0000,"√",0);
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
* 函  数  名      : LCDUartSET
* 描      述      : LCDUart触摸屏中断系统设置按键判断
* 输      入      : Touch_key.
* 返      回      : 无.
*
* 修       改     : WK
* 时       间     : 2013-03-13
* 描       述     : 基于2013-03-08界面 -->保存键 、恢复出厂设置   还没有完成
*******************************************************************************/
void LCDUartSET(U8 Touch_key)   // wk --> 系统设置 键号跳转函数
{
    if(Touch_key<3||Touch_key==255)
    {
        MenuSwFlg=1;//切换页面
        TimeFlg=1;//页面切换后完全显示时间
         //系统设置按键均使发送完成标志清零
    }         
    switch(Touch_key)
    {
    case 0: //系参设置//对SwFlg、FuncFlg、DataFlg、SaveFlg、ParaIndex、DataCnt赋初值
        // VIEWHoldFlg=0;
        Dis_PicID=MenuParaSET;
        OtherSetIndex=0; // WK --> ?
        TimeSetIndex=9;  // WK --> ?
        SysSet.SwFlg=1;
        SysSet.ParaIndex=0;
        SysSet.DataCnt=0;
        break;
    case 1:                       /*事件设置*/
        //VIEWHoldFlg=0;
        Dis_PicID=MenuEventSET;
        SysSet.SwFlg=1;
        SysSet.EvntIndex=0;
        SysSet.DataCnt=0; 
        break;
        
    case 2:  /* 恢复出厂设置 */
        Dis_PicID=MenuInitSET;
        SysSet.SwFlg=1;
        break;
        
//    case 18:   // wk --> 恢复出厂设置
    case 0x31:                  /*事件设置清除键*/
        InitAck=1;
        break;
        
    case 0x32:                  /*事件设置清除键*/
        //InitNoAck=1;
        MenuSwFlg=1; //  wk -->页面切换标志
        TimeFlg=1; //页面切换后完全显示时间
        Dis_PicID=MenuParaSET;
        break;
        /**********************************************************************
        ** WK --> 系统参数设置界面按键  
        **********************************************************************/
    case 16:                       /*系统参数右移切换键*/
        OtherSetIndex++ ;
        SysSet.FuncFlg=1;  // WK --> 有光标移动
        if(OtherSetIndex==9||TimeSetIndex!=9)//从时间项切换回其余项时，光标需回到第一项
        {
            OtherSetIndex=0;
            TimeSetIndex=9;  //实际只需在TimeSetIndex!=6时将其值置为6
        }
        SysSet.ParaIndex=OtherSetIndex;
       
        break;
    case 17:
        SysSet.FuncFlg=1;
        if(OtherSetIndex==0||TimeSetIndex!=9)//从时间项切换回其余项时，光标需回到第五项
        {
            OtherSetIndex=9;
            TimeSetIndex=9;
        }
        OtherSetIndex-- ;
        SysSet.ParaIndex=OtherSetIndex;
        
        break;
     case 19:
       SysSet.ParaSaveFlg=1;      // WK --> 具体实现带研究
       break;
     
    case 0x60:  // WK --> TCP/IP开关      0 关闭  1 开启
      SysSet.SwitchSet[0]==0?(SysSet.SwitchSet[0]=1):(SysSet.SwitchSet[0]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x61:  // WK --> 485上行
      SysSet.SwitchSet[1]==0?(SysSet.SwitchSet[1]=1):(SysSet.SwitchSet[1]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x62:  // WK --> 485下行
      SysSet.SwitchSet[2]==0?(SysSet.SwitchSet[2]=1):(SysSet.SwitchSet[2]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x63:  // WK --> GPS对时
      SysSet.SwitchSet[3]==0?(SysSet.SwitchSet[3]=1):(SysSet.SwitchSet[3]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x64:  // WK --> 声音设置
      SysSet.SwitchSet[4]==0?(SysSet.SwitchSet[4]=1):(SysSet.SwitchSet[4]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x65:  // WK --> 背光设置
      SysSet.SwitchSet[5]==0?(SysSet.SwitchSet[5]=1):(SysSet.SwitchSet[5]=0);
      SysSet.SwitchSet[7]=1;
      break;
    case 0x66:  // WK --> 移动存储
      SysSet.SwitchSet[6]==0?(SysSet.SwitchSet[6]=1):(SysSet.SwitchSet[6]=0);
      SysSet.SwitchSet[7]=1;
      break;        
     /******************************** end *********************************/
       
     /**********************************************************************
      ** WK --> 系统事件设置界面按键  
      **********************************************************************/  
    case 64:               // WK --> 事件设置下移       
        // WK --> 待补充  
        SysSet.FuncFlg=1;
        if(SysSet.EvntIndex <7 )
          (SysSet.EvntIndex==6)? (SysSet.EvntIndex=0): (++SysSet.EvntIndex);  
        else
         (SysSet.EvntIndex==10)? (SysSet.EvntIndex=7):(++SysSet.EvntIndex);
        break;
    case 65:              // WK --> 事件设置上移         
          SysSet.FuncFlg=1;
          if(SysSet.EvntIndex <7 )
            (SysSet.EvntIndex==0)? (SysSet.EvntIndex=6):(--SysSet.EvntIndex);      
          else
            (SysSet.EvntIndex==10)?(SysSet.EvntIndex=7):(--SysSet.EvntIndex);   
        break;
    case 66:             // WK --> 事件设置右移         
        SysSet.FuncFlg=1;
        (SysSet.EvntIndex < 7) ? (SysSet.EvntIndex=7 ) : (SysSet.EvntIndex=0);
        break;     
    case 67:                       /*保存键*/
        SysSet.EventSaveFlg=1;      // WK --> 具体实现带研究
        break;
     /************************************* end******************************/
        
    case 24:
        break;               /*参数设置已去除小数点键*/
    case 72:                  /*事件设置小数点键*/
        if(SysSet.DataCnt)
        {
            SysSet.Data[SysSet.DataCnt++]='.';
        }
        break;
    case 25:                  /*参数设置清除键*/
    case 73:                  /*事件设置清除键*/
        SysSet.DataFlg=1;
        SysSet.DataCnt=0;
        break;
    case 255:
        Dis_PicID=MenuTop;   // wk --> 返回主界面
        break;
        
    default:
        if(Touch_key>31&&Touch_key<42)//系统参数设置下的小键盘0-9
        {
            SysSet.DataFlg=1;
            if(SysSet.DataCnt<(SysSet.ParaIndex<4?(SysSet.ParaIndex==3?4:5):2))  // wk --> 最多输入 5 位数
            {
                SysSet.Data[SysSet.DataCnt++]=Touch_key+16;
            }
            else
            {
                SysSet.DataCnt=0;    //相当于清除键
            }
        }
        if(Touch_key>79&&Touch_key<90)//系统事件设置下的小键盘0-9
        {
            SysSet.DataFlg=1;
            if(SysSet.DataCnt<5)
            {
                SysSet.Data[SysSet.DataCnt++]=Touch_key-32;
            }
            else
            {
                SysSet.DataCnt=0;    //相当于清除键
            }
        }
        break;
    }
}
/*******************************************************************************
* 函  数  名      : LCDUartEVENT
* 描      述      : LCDUart触摸屏中断系统设置按键判断
* 输      入      : Touch_key.
* 返      回      : 无.
*******************************************************************************/
void LCDUartEVENT(U8 Touch_key)   //事件显示按键跳转函数。
{
    if(Touch_key<3||Touch_key==255)
    {
        MenuSwFlg=1;//切换页面
        TimeFlg=1;//页面切换后完全显示时间
    }
    switch(Touch_key)
    {
    case 0:
        Dis_PicID=MenuEventMON;   //事件监测
        break;
    case 1:
        Dis_PicID=MenuEventList;  //事件列表
        EVEnum_old=0;
        break;
    case 2:
        Dis_PicID= MenuEventWave;  //事件波形
        break;
    case 32:            //事件列表中的上翻页
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
    case 33:             //事件列表中的下翻页
        if(EVEpage==8)
        {
            EVEpage=0;
        }
        EVEpage++;
        MenuSwFlg=1;
        break;
    case 34:             //事件列表中的上移
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
    case 35:           //事件列表中的下移
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
