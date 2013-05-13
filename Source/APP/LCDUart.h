#ifndef LCDUART_H
#define LCDUART_H

#include "includes.h"

//�궨������޸�
#define PageIcon 2
#define PageStart 1
#define MenuTop 0
#define MenuSysPara  10
#define MenuSysEvent  11
#define MenuInitSET 18
#define MenuViewWavVolCur 20  // wk : �л���������ҳ����ʾ --> ��ѹ����������ʾ
#define MenuViewWavVol 21  
#define MenuViewWavCur 22
#define MenuViewVector 23
#define MenuViewListMeasure 24
#define MenuViewListQuality1 25
//#define MenuView3Unbalanced 26
#define MenuViewListQuality2 26  // wk -->
#define MenuViewHarmoGraph1 27
#define MenuViewHarmoGraph2 3  // WK -->
#define MenuViewHarmoList1 28//��ʾ1-26��г��
#define MenuViewHarmoList2 29//��ʾ25-50��г��
// WK --> ����״̬��ͼƬ��ʱ��δ��д��ȥ������ 30����
#define MenuStatus 9 // wk --> ����״̬

#define MenuEvent
#define MenuParaSET 10
#define MenuEventSET 11
#define MenuEventMON 40
#define MenuEventList 41
#define MenuEventWave 42

typedef struct SS //ϵͳ�������ü��¼�����ҳ����ʹ�õı���
{
    U8 SwFlg;//ҳ���л���־
    U8 FuncFlg;//���ܼ����л��������±�־��Ĭ��0
    U8 DataFlg;//��ֵ���̰��±�־
    U8 ParaSaveFlg;//�����־
    U8 EventSaveFlg;
volatile U8 EventSendFlg; //wk@130508--> 0��ʾû�в�����1��ʾ�����ͣ�2��ʾ�������
    U8 ParaIndex;//0-11ϵͳ��������ҳ��
    U8 EvntIndex;//0-11.�¼�����ҳ��
    U8 DataCnt;//����������ֵλ��
    U8 Data[5];//ϵͳ���ý����µ�С����
    
    U8 SwitchSet[8]; // wk --> ϵͳ�������ý���� 7 ����ť
}SysStr;

//LCDUart_EXT volatile U8 DisTimeOnce;
extern volatile U8 MenuSwFlg;//ҳ���л���־��Ĭ��ֵΪ0
extern volatile U8 Dis_PicID;
extern volatile U8 ViewKeyFlg;
extern volatile U8 SysSetKeyFlg;
extern volatile U8 EventKeyFlg;
extern volatile U8 HarmoGraphPhase;//����ѡ����ʾ�࣬Ĭ��ֵ1
extern volatile U8 HarmoGraphRange;//����ѡ����ʾг����Χ��Ĭ��ֵΪ1,��Ӧ1~26�Σ�2��Ӧ25~50
extern volatile U8 HarmoGraphUorder;//Ĭ��ֵ1�����ڿ���г����ѹ�����������ֵ��ʾ,
extern volatile U8 HarmoGraphIorder;//Ĭ��ֵ1�����ڿ���г�����������������ֵ��ʾ��
extern volatile U8 HarmoListShift;//Ĭ��ֵ0��г���б���ʾ�еĹ������Ƽ�
extern volatile U8 HarmoListPhase;//Ĭ��ֵ1��
extern volatile U8 HarmoListUorI;//Ĭ��ֵ1��
extern volatile U8 HarmoListRange;//Ĭ��ֵ1��
extern volatile U8 HarmoListAmporRatio;////Ĭ��ֵ1��г���б���ʾ����ʾ��ֵ�(1)���ߺ�����(2)��ѡ��
extern volatile U8 VIEWHoldFlg;//���ּ�Ĭ��Ϊ0��������ʱֵ��Ϊ1���ٴΰ���ʱֵ��Ϊ0��
extern volatile U8 EVEpage;//�¼���ҳ������Ϸ��·�
extern volatile U8 EVEline;//�¼��������ư�����ȷ���¼����м��ڼ����¼���
extern volatile U8 EVEfunflg;//�¼��������ư������ܱ�־��
extern volatile U8 OtherSetIndex;//��ֵΪ0�����ڴ�C�ļ���ʹ��
extern volatile U8 TimeSetIndex;//��ֵΪ6�����ڴ�C�ļ���ʹ��
extern volatile U8 RemovDevFlg;
extern volatile U8 SoundOffOn;
extern volatile U8 BlightOffOn;
//extern volatile struct SS SysSet;//��LCDUART.c�и�ֵ����MenuView.c��ʹ��
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


