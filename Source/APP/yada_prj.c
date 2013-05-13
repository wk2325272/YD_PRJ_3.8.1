/*******************************************************************************
* File Name       	 : 
* Author             	 : 
* Version            	 : 
* Date               	 :
* Description     	 :
*******************************************************************************/
#include "includes.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#define _GUI_DBUG_
/* Task IDs */
#define RS232_TASK 5
#define LPT_FLAG_CLOCK_SOURCE_LPO           (0x00000002) // wk @130330 --> test timer

volatile U8 RefreshFlg; // 页面刷新标志
U8 U_FLAG = 0,Time_Ref = 0;
uchar read_buffer[8]={0x12,0x23};  // wk @130403 --> uncomment

extern void YaDa(uint_32);
extern void MainLoop();
extern void USB_task(uint_32 param);
//extern void sdcard_task(uint_32 param);
extern void FTP_task(uint_32 param);
extern void Shell_Task(uint_32 param);
extern void LCDTouchSel_Task(uint_32 param);
extern void SPIDMA_Task( uint_32 param);
extern void sdcard_task(uint_32 temp);
extern void PIN2(uint_32 temp);
extern void mem_flash_app(uint_32 temp);
extern void tcp_socket_task( uint_32 val);

void flg_int();

/*  pin2 int callback function */
void int_callback(void) 
{
  pointer ppin2_event;
  _event_open("pin2_event",&ppin2_event);
  _event_set(ppin2_event,0x04);
  
  printf("Switch2 is pressed(int mode)!\n");
}
/*******************************************************************************
** Function Name	：timser_isr
** Input		：device num of timer
** Return		：void
** Author		：wk
** Version	：v1.0
** Date		：130330
** Dessription	：LPT 定时器0中断函数入口
** Reverse	：
*******************************************************************************/
static void timer_isr
    (
        pointer parameter
    )
{
    uint_32 timer = (uint_32)parameter;
    
    /* Stop the timer */
    _lpt_run (timer, FALSE);
    _lpt_clear_int (timer);

//    printf("\nhellow\n");
    SavePowerFlg =1;
//    EventKeyFlg=1; SPIEventFlg=1;// wk @130401 --> test event data save
    _lpt_init(0,3 * 1000000 , LPT_FLAG_CLOCK_SOURCE_LPO,TRUE);
}
/*******************************************************************************
** Function Name	：timser_isr
** Input		：device num of timer
** Return		：void
** Author		：wk
** Version	：v1.0
** Date		：130330
** Dessription	：LPT 定时器1中断函数入口
** Reverse	：
*******************************************************************************/

/*
** 作者：
** 时间：2013-02-27
** 说明： Task ：YADA 与 Shell_Task 的优先级问题
**　      在目前调试阶段，需要运行谁，将优先级提高
**    --> 最后完善阶段，需要通过在 YADA 任务中选择调用 Shell_Task <--   ,--> 已解决，通过PIN2任务
*/
const TASK_TEMPLATE_STRUCT  MQX_template_list[] =    //  | MQX_TIME_SLICE_TASK
{ 
   /* Task Index,   Function,         Stack,  Priority, Name,     Attributes,          Param, Time Slice */
   {1,  YaDa,       4000,   11,         "MAIN&GUI_Task", MQX_AUTO_START_TASK , 0,    1000 },  // | MQX_TIME_SLICE_TASK
   {2,  Shell_Task, 4000L,  12, "Shell_Task",   MQX_AUTO_START_TASK, 0,     1000 },  //  | MQX_TIME_SLICE_TASK
   
   {3,  FTP_task,   2000,   9L,   "FTP_Task",     MQX_AUTO_START_TASK, 0,     0 },
   {4,  tcp_socket_task,   3000-1000L,   9L,   "tcp_socket_task",     0, 0,     0 },
   {5,  USB_task,   5200L,  8L,   "USB_Task",     MQX_AUTO_START_TASK, 0,     0 },  // MQX_AUTO_START_TASK
   {6,  mem_flash_app, 4000L,  8L,   "Flash_Task",     MQX_AUTO_START_TASK, 0,     0 },
   {7,  LCDTouchSel_Task ,1000,7 , "UartTouch_Task", MQX_AUTO_START_TASK, 0 , 0},
   {8,  SPIDMA_Task , 5000, 6 , "SPIDMA_Task",MQX_AUTO_START_TASK,0 ,0 },
   //   {10,  sdcard_task,2000L,  8L, "SDcard",  MQX_AUTO_START_TASK, 0,     0 },
   {11, PIN2,       500-100L,   9L,  "PIN2_Task", MQX_AUTO_START_TASK,0 ,0 }, // 按键的处理任务
   { 0 }
};

/*TASK*-----------------------------------------------------
* 
* Task Name    : YaDa
* Comments     :
*    
*
*END*-----------------------------------------------------*/
void YaDa
   (
      uint_32 initial_data
   )
{
#ifdef _GUI_DBUG_
   printf("\n----------MAIN&GUI_Task----------\n");
   printf("\n----------             ----------\n");
   printf("\n----------             ----------\n");
   printf("\n----------      END    ----------\n");
#endif 
  UartLCD_init();  // uart initialization
  UartTouch_init();
  flg_int(); // wk --> 初始化一些标志 !  
  spi2_dma_int(); // dsp2k60 spi2 初始化
  
  YADA_70(PageStart); //必要的初始化后进入首页
  delay_ms(1000); // wk -->test  延时1s
//  delay_ms(4000); // wk -->test  延时4s，等待U盘启动完成
//  YADA_E4();  // wk --> 屏幕校正
  YADA_70(MenuTop);  // 进入菜单首页
  
  RefreshFlg = 0; //页面无刷新 
    
    SHELL_CONTEXT_PTR    shell_ptr;
    shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
    _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
    uint_32 file_size;  uchar status;
   /* wk @130401 --> 在 flash中 新建 sysset 用于系统变量保存 */
    shell_ptr->ARGC = 2;
    shell_ptr->ARGV[0]="cd";
    shell_ptr->ARGV[1]="f:\\"; 
    Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
    
//      shell_ptr->ARGC = 2;
//      shell_ptr->ARGV[0]="df_s";
    shell_ptr->ARGV[1]="SYSSET";   //wk --> 注意：查找的文件名暂时必须要是大写
    status=Shell_search_file_r1(shell_ptr->ARGC, shell_ptr->ARGV,&file_size);
    if(status==0)
    {
//        shell_ptr->ARGC = 2;
//        shell_ptr->ARGV[0]="mkdir";
      shell_ptr->ARGV[1]="SYSSET"; 
      Shell_mkdir(shell_ptr->ARGC, shell_ptr->ARGV);
    }
  _mem_free(shell_ptr);
    
  /* button1 into interrupt for shell or maingui task change */
   GPIO_PIN_STRUCT pins_int[] = {
            BSP_BUTTON1 | GPIO_PIN_IRQ_RISING ,
            GPIO_LIST_END
        };
    MQX_FILE_PTR port_file4;        
         /* 这是按键1 上升沿中断*/
   port_file4 = fopen("gpio:read", (char_ptr) &pins_int );
   ioctl(port_file4, GPIO_IOCTL_SET_IRQ_FUNCTION, (pointer)int_callback);        
  /* end */
  /* wk @130330 -->timer of lpt */
   /* wk @130504 --> 调试事件，先关闭 */
   _lpt_install (0,3 * 1000000 , LPT_FLAG_CLOCK_SOURCE_LPO, 11, timer_isr, TRUE);//3 * 1000000  --> 3秒   
  /* wk @130330 -->timer end */
   
//   delay_ms(4000); 
   _rtc_init ( RTC_INIT_FLAG_CLEAR); // wk @130510 --> 在应用程序中再初始化并打开
   _rtc_init ( RTC_INIT_FLAG_ENABLE); /* wk@130511-->程序有时可能因为此处影响触摸屏的应用 */
//   TimeSet();
  while(1)
  {   
      if(SysFlashData[5])                           //背光标志，1为开背光，0为关。
        {
            YADA_5F(0x3f);                            //背光全开
        }
        else
        {
            YADA_5F(0x08);                             //背光部分开
        }
      
      MainLoop(); //循环主程序
  }
}

/*******************************************************************************
* 函  数  名      : MainLoop
* 描      述      : main函数中的菜单主函数
* 输      入      : 无
* 返      回      : 无
*******************************************************************************/

void MainLoop()
{
  if (SPIPowerFlg) //接收到电能数据
  {
    if (VIEWHoldFlg == 0)
    {
      RefreshFlg = 2;
      ViewKeyFlg = 0; //VIEWHoldFlg为0时，可能有其余键按下，需要清零
    }
    
    SPIPowerFlg = 0; // 清除标志
  }
  else if(ViewKeyFlg) //  ? ViewKeyFlg ?
  {
    RefreshFlg = 2;
    ViewKeyFlg = 0;
  }
  /* wk @130401 --> 基本电能质量数据保存 */
  if(SavePowerFlg)
  {
    PowerSave(); 
    SavePowerFlg=0;
  }
  /* wk @130401 --> end */
#if 1 // WK --> 数据存储  待完善
  if (SPIEventFlg || EventKeyFlg) //接收到事件数据或者事件记录相关页有键按下时需要刷新
  {
    RefreshFlg = 3;
    if (SPIEventFlg)
    {
      EventSave(U_FLAG);
    }
    SPIEventFlg = 0;
    EventKeyFlg = 0;
  }
#endif
  
  if(SysSetKeyFlg) //系统设置页有键按下时需要刷新
  {
     SysSetKeyFlg = 0;
     RefreshFlg = 1; 
  }
  
  if (MenuSwFlg == 1) //不能放在switch中，功能键按下时会整体刷新一次;页面切换必须放判断刷新之后，否则出现频繁操作后，页面没有数据的现象
  {
    MenuSwFlg = 0 ;
    YADA_70(Dis_PicID);
    
    YADA_70(Dis_PicID); //有时会出现切屏不及时，冗余思想
  }
  
  if (Dis_PicID != MenuTop)//时间显示需放在界面切换之后，放数据刷新之后的话延时更长才显示
    {

       if(TimeFlg==1)
       {
         TimeDis(1);
         TimeFlg=0;
//         _rtc_init ( RTC_INIT_FLAG_ENABLE);
       }
       else if(TimeFlg==2)
       {
         TimeDis(0);
         TimeFlg=0;
       }
      
        if(Time_Ref==1)//设置8025固定中断，一分钟才刷新，但秒比较闪
        {
//            //U8 Config =0x20;
//            Write8025(&Config,15,1);//使/INTA="L"设置为ＯＦＦ
          Time_Ref=0;
//          DISTIME(1);
        }
        
        if(USB_Flg) //U盘图标显示
        {
            YADA_71(PageIcon,55,450,102,475,55,450);
        }
        else
        {
            YADA_71(PageIcon,55,400,102,425,55,450);
        }
    }
  /* ……*/
  switch(RefreshFlg)
  {
  case 1:
    RefreshFlg = 0;
    switch (Dis_PicID)
    {
      case MenuParaSET:
        GUI_SYS_PARASET();
        break;
      case MenuEventSET:
        GUI_SYS_EVENTSET();  // 涉及 U盘数据  --> wk 
        break;
      case MenuInitSET:
        GUI_INIT_SET();
        break;
      default:
        break;
    }
    break;
    
  case 2:
    RefreshFlg = 0;
    switch (Dis_PicID)
    {
      case MenuViewWavVolCur:
      GUI_VIEW_UI();
      break;
    case MenuViewWavVol:
      GUI_VIEW_U();
      break;
    case MenuViewWavCur:
      GUI_VIEW_I();
      break;
    case MenuViewVector:
      GUI_VIEW_VECT();
      break;
    case MenuViewListMeasure:
      GUI_VIEW_ListMeasure();
      break;
    case MenuViewListQuality1:
      GUI_VIEW_ListQuality();
      break;
    case MenuViewHarmoGraph1:
      GUI_VIEW_HarmoGraph();
      break;
    case MenuViewHarmoList1:
    case MenuViewHarmoList2:
      GUI_VIEW_HarmoList();
      break;
    case MenuViewListQuality2: // wk --> 
      GUI_VIEW_ListQuality2(U_FLAG);
      break;
      
    case MenuStatus:
      GUI_STATUS(U_FLAG);
      break;  
      
    default:
      break; 
    }
    break;
    
  case 3:
    RefreshFlg = 0;
    switch (Dis_PicID)
    {
    case MenuEventList:
      GUI_EventList();   // 涉及 U 盘数据  --> wk 
      break;
    case MenuEventWave:
      GUI_EventWave(U_FLAG);   // 涉及 U 盘数据  --> wk
      break;
    case MenuEventMON:
      GUI_EventMonitor(U_FLAG);  // 涉及 U 盘数据  --> wk
      break;
      
    default:
      break;  
    }
    break;
    
  default:
    break;
  }
}
 
