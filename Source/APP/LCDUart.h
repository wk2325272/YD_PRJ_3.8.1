#ifndef LCDUART_H
#define LCDUART_H

#include "includes.h"

//宏定义便于修改
#define PageIcon 2
#define PageStart 1
#define MenuTop 0
#define MenuSysPara  10
#define MenuSysEvent  11
#define MenuInitSET 18
#define MenuViewWavVolCur 20  // wk : 切换到主数据页面显示 --> 电压电流波形显示
#define MenuViewWavVol 21  
#define MenuViewWavCur 22
#define MenuViewVector 23
#define MenuViewListMeasure 24
#define MenuViewListQuality1 25
//#define MenuView3Unbalanced 26
#define MenuViewListQuality2 26  // wk -->
#define MenuViewHarmoGraph1 27
#define MenuViewHarmoGraph2 3  // WK -->
#define MenuViewHarmoList1 28//显示1-26次谐波
#define MenuViewHarmoList2 29//显示25-50次谐波
// WK --> 工作状态的图片暂时还未烧写进去，先用 30代替
#define MenuStatus 9 // wk --> 工作状态

#define MenuEvent
#define MenuParaSET 10
#define MenuEventSET 11
#define MenuEventMON 40
#define MenuEventList 41
#define MenuEventWave 42

typedef struct SS //系统参数设置及事件设置页面中使用的变量
{
    U8 SwFlg;//页面切换标志
    U8 FuncFlg;//功能键（切换键）按下标志，默认0
    U8 DataFlg;//数值键盘按下标志
    U8 ParaSaveFlg;//保存标志
    U8 EventSaveFlg;
volatile U8 EventSendFlg; //wk@130508--> 0表示没有操作，1表示待发送，2表示发送完成
    U8 ParaIndex;//0-11系统参数设置页下
    U8 EvntIndex;//0-11.事件设置页下
    U8 DataCnt;//控制输入数值位数
    U8 Data[5];//系统设置界面下的小键盘
    
    U8 SwitchSet[8]; // wk --> 系统参数设置界面的 7 个按钮
}SysStr;

//LCDUart_EXT volatile U8 DisTimeOnce;
extern volatile U8 MenuSwFlg;//页面切换标志，默认值为0
extern volatile U8 Dis_PicID;
extern volatile U8 ViewKeyFlg;
extern volatile U8 SysSetKeyFlg;
extern volatile U8 EventKeyFlg;
extern volatile U8 HarmoGraphPhase;//用于选择显示相，默认值1
extern volatile U8 HarmoGraphRange;//用于选择显示谐波范围，默认值为1,对应1~26次，2对应25~50
extern volatile U8 HarmoGraphUorder;//默认值1，用于控制谐波电压具体次数的数值显示,
extern volatile U8 HarmoGraphIorder;//默认值1，用于控制谐波电流具体次数的数值显示，
extern volatile U8 HarmoListShift;//默认值0，谐波列表显示中的功能右移键
extern volatile U8 HarmoListPhase;//默认值1，
extern volatile U8 HarmoListUorI;//默认值1，
extern volatile U8 HarmoListRange;//默认值1，
extern volatile U8 HarmoListAmporRatio;////默认值1，谐波列表显示中显示幅值澹(1)或者含有率(2)的选择
extern volatile U8 VIEWHoldFlg;//保持键默认为0，键按下时值变为1，再次按下时值变为0；
extern volatile U8 EVEpage;//事件的页面键，上翻下翻
extern volatile U8 EVEline;//事件的上下移按键，确定事件的行即第几个事件。
extern volatile U8 EVEfunflg;//事件的上下移按键功能标志。
extern volatile U8 OtherSetIndex;//初值为0，仅在此C文件中使用
extern volatile U8 TimeSetIndex;//初值为6，仅在此C文件中使用
extern volatile U8 RemovDevFlg;
extern volatile U8 SoundOffOn;
extern volatile U8 BlightOffOn;
//extern volatile struct SS SysSet;//在LCDUART.c中赋值，在MenuView.c中使用
extern volatile SysStr SysSet;
extern volatile U8 InitAck;
extern volatile U8 EvntPgUpFlg;

void LCDUartView(U8 Touch_key);
void LCDUartSET(U8 Touch_key);
void LCDUartEVENT(U8 Touch_key);
void UartTouch_init(void);  // wk --> uart initialization for touch 
void LCDTouchSel_Task(uint_32 param);
void new_uart_isr(pointer user_uart_ptr);

/* wk --> 20130117 */
#define BSP_UART_INTERRUPT_VECTOR INT_UART4_RX_TX
typedef struct isr_uart_struct
{
  pointer ISR_DATA;
  U8   data;
} ISR_UART_STRUCT, _PTR_  ISR_UART_STRUCT_PTR;
#endif


