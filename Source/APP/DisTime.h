#ifndef DISTIME_H
#define DISTIME_H

extern volatile U8 TimeFlg;
extern U8 TimeCnt;
/* º¯ÊýÉùÃ÷ */
void TimeSet();
void TimeDis(U8 DisType);

static void print_mqx_time(DATE_STRUCT_PTR  time_rtc,TIME_STRUCT_PTR time_mqx);
static void print_current_time(void);
static void print_rtc_time(DATE_STRUCT_PTR  time_rtc,TIME_STRUCT_PTR time_mqx);

#endif