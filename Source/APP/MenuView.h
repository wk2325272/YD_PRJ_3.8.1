//数据显示时使用到LCD的暂存缓冲区间为0x0000-0x0AF7
#ifndef MENUV_H
#define MENUV_H

//#include "includes.h"
#include "System.h"

#define UIS_SIZE 240               //发送的波形采样数据总数
#define size_UI 100               //波形采样数据各相总数
#define Vec_X0 170               //向量图的坐标原点X坐标
#define Vec_Y0 236               //向量图的坐标原点X坐标
#define Vec_R0 165                //向量图的半径
#define PI 3.14
#define D_y1 1
#define D_y2 2
#define UI_LINE_NUM 85
#define U_I_LINE_NUM 99
#define UI_DX 7
#define U_I_DX 6
#define Cycledot 80 //实时波形的点数
//与SPI通信相关的宏定义，表示各项数据在SPIRxChar数组中的下标
//#define UI_VIRTUAL_INDEX 0  //电压电流的有效值初始下标
//#define Harmo_INDEX 80      //A相谐波初始下标，A相谐波电流80+200,A相谐波相位80+400
#define I_VIRTUAL_INDEX 12  //电流的有效值初始下标
#define PQS_INDEX 24  //有功无功视在功率的初始下标
#define PF_INDEX 60   //功率因数初始下标
#define F_INDEX 72    //频率初始下标
#define VEC_INDEX 84   //电压相角的初始下标
#define U_ERR_INDEX 108   //电压偏差，频率偏差，畸变率，闪变的初始下标
#define UNBLA_INDEX 180   //正序负序零序，三相不平衡度的初始下标
#define Harmo_INDEX 252      //A相谐波初始下标，A相谐波电流80+200,A相谐波相位80+400
//B相谐波初始下标680，B相谐波电流680+200,B相谐波相位680+400
//C相谐波初始下标1280，C相谐波电流1280+200,C相谐波相位1280+400
#define PQSf_INDEX 1880     //三相有功视在无功-功率因素-频率的初始下标，共15个数据，60个字节
//#define VEC_INDEX 1940   //电压相角的初始下标
#define Pst_INDEX 1964      //APst-BPst-CPst-APlt-BPlt-CPlt-ε
#define UNBALANCED_INDEX 1988 //U-电压不平衡度、ABC(幅值相位)零、正、负，εI，ABC零、正、负
#define WAVEUI_INDEX 2052   //采样波形的初始下标
//使用C108指令显示浮点型数据时的格式控制
//使用C108指令显示浮点型数据时的格式控制
#define C108Mode_63 0x6304
#define C108Mode_64 0x6404
#define C108Mode_62 0x6204
#define C108FC_W  0xffff
#define C108BC_Bk  0x0000
//触摸屏暂存缓冲区的安排
//使用C104，采样波形写入触摸屏的地址，每相数据占据200个字节地址
#define UA_addr 0X0000
#define UB_addr 0X00C8
#define UC_addr 0X0190
#define IA_addr 0X0258
#define IB_addr 0X0320
#define IC_addr 0X03E8
//使用C108指令，显示浮点型数据时使用的暂存缓冲区地址

#define DMMPowerInfoAdr 0x0500//UI有效值及攻率显示时使用的地址，21个数据共需要使用147个字地址,294
#define HarmoListInfoAdr 0x0626//谐波列表数据显示时使用的暂存缓冲区地址,26需要使用182个字地址，即364个字节地址
#define HarmoGraphInfoAdr 0x0792//谐波柱状图中数值显示时使用的地址，4个数据，使用28个字地址，56个字节地址
#define ListQualityInfoAdr 0x0874//数值列表中电能质量参数14个，使用98个字地址，196个字节地址
#define Vectoraddr 0x0938      //向量图的相位数值的使用地址,6个数据，42个字地址,84个字节地址
#define WAVEUIaddr 0x098C      //波形图中UI的有效值，6个数据，84个字节地址
#define ParaSetAddr 0x0AB0
#define EventSetAddr 0x0AF7   //事件参数，12个数据，168个字节地址
#define EventMonLAddr 0x0B51    //事件监测左边列地址10个数据，140字节
#define EventMonRAddr 0x0BDD    //事件监测右地址4（BB3）
#define ListUnblanceAdr 0x8000 //三相不平衡，一共38个数据，占532个字节地址 
#define RX8025C108Addr 0x8F00//每页的时间显示，6个数据，占用84个字节
//#define EventMonLAddr 0x0B51    //事件监测左地址10
//#define EventMonRAddr 0x0B97    //事件监测右地址4（BB3）
#define Evetime 0x0B9d
/*WK @130326 --> 事件限值宏定义 */
/* wk @130326 --> 扩大了100倍 ：显示时的模式会将数据除100显示*/
#define UDeviation 100*7  
#define FDeviation 20 // 0.2*100
#define USurge 100*2
#define NUnblance 100*2
#define LngFlick 100
#define TotalHarmonic 100*5
//#define JHarmonic
#define UHarmonic 100*4 
#define IHarmonic 100*2

#define EVENTLEN 2880
#define EVEUILEN (160*2)
// REVERSE BY WK @2013-03-13
//#define EVESET_INDEX 29
#define EVESET_INDEX 25

//#define EVESEND_FLAG 85
#define EVESEND_FLAG 70
#define LINENUM 256

#define StatusAddr 0x9100 //工作状态的地址，3个数据，42个字节。

extern U8 SysFlashDataT[84];   //系统设置的数据的临时参数
extern U8 SysFlashData[84];   //wk @130326 -->写入Flash的系统设置参数
extern uchar SysDataSend[56]; // K60写给DSP的数据
//MenuV_EXT U8 NPage[115]; //写入芯片flash有关的数组。NPage[0~99]为事件nandflash的页值，NPage[100]为事件总次数，NPage[101~114]为各类事件次数
//MenuV_EXT U16 NBlock[100]; //写入芯片flash有关的数组。nandflash的block数
extern U16 EventNum[9]; // wk @130405 --> 9次事件发生次数保存，每个事件占2字节，能记录65535次
extern U32 EventAddr[100];// wk@130405 -->记录事件发生的时间：月、日、时、分、秒，每个占4字节
extern U8 USB_Flg; // wk @130407 --> USB 是否插入标志
extern U8 EVEnum;
extern U8 SavePowerFlg;

void GUI_VIEW_UI(void);
void GUI_VIEW_U(void);
void GUI_VIEW_I(void);
void GUI_VIEW_VECT(void);
void GUI_VIEW_ListMeasure();
void GUI_VIEW_ListQuality();
void GUI_VIEW_ListQuality2(U8 U_DISK);
void GUI_VIEW_HarmoGraph();
void GUI_VIEW_HarmoList();
void linemark(U16 Y_COORD, U16 U_I);
void mainloop(void);
void GUI_SYS_PARASET(void);
void GUI_SYS_EVENTSET(void); 
void GUI_EventMonitor(U8 U_DISK);
void GUI_EventList(void);
void GUI_EventWave(U8 U_DISK);
void EventSave(U8 U_DISK);
void PowerSave(void);
void GUI_INIT_SET(void);
void GUI_STATUS(U8 U_DISK);
void flg_int(void);
#endif





