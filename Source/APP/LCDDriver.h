#ifndef LCDD_H
#define LCDD_H

#include "includes.h"
#include "includes.h"

#define RS232_CHANNEL "ttye:"

typedef struct
{ 
    U8 style;
    U16 X_0;
    U16 Y_0;
    U8 R_0;
} ARC;


/* º¯ÊýÉêÃ÷ */
 void UartLCD_init();

 void send_byte(U8 i);
 void send_word(U16 i);
 void send_xy(U16 x,U16 y);
 void send_area(U16 xs,U16 ys,U16 xe,U16 ye);
 void send_U16str(U16 *q,U8 size);
 void send_U8str(U8 *p,U8 size);
 void send_end();
 
 void YADA_00 ( void );
 void YADA_40 (U16 FrontC,U16 BackC);
 void YADA_41 (U8 D_X,U8 D_Y);
 void YADA_42 (U16 X,U16 Y);
 void YADA_43 (U16 X,U16 Y);
 void YADA_44 (U8 EN,U16 X,U16 Y,U8 Width,U8 Height);
 void YADA_53 (U16 X,U16 Y,U8 *P,U8 Size);
 void YADA_54 (U16 X,U16 Y,U8 *P,U8 Size);
 void YADA_55 (U16 X,U16 Y,U8 *P,U8 Size);
 void YADA_6E (U16 X,U16 Y,U8 *P,U8 Size);
 void YADA_6F (U16 X,U16 Y,U8 *P,U8 Size);
 void YADA_98 (U16 X,U16 Y,U8 Lib_ID,U8 C_Mode,U8 C_Dots,U16 FrontC,U16 BackC,U8 *P,U8 Size);
 void YADA_45(U16 *P,U8 Flag);
 void YADA_50 (U16 *P,U8 Size);
 void YADA_51 (U16 *P,U8 Size);
 void YADA_74 (U16 X,U16 YS,U16 YE,U16 BC,U16 *P,U8 Size);
 void YADA_56 (U16 *P,U8 Size);
 void YADA_5D (U16 *P,U8 Size);
 void YADA_75 (U16 X,U16 Y,U8 H_Max,U8 *P,U8 Size);
 void YADA_76 (U16 X,U8 X_Dis,U16 *P,U8 Size);
 void YADA_78 (U16 X,U16 Y,U8 *P,U8 Size);
 void YADA_57 (ARC *P,U8 Size);
 void YADA_5704 (U16 X,U16 Y,U16 R,U16 AS,U16 AE);
 void YADA_59 (U16 *P,U8 Size);
 void YADA_69 (U16 *P,U8 Size);
 void YADA_64 (U16 X,U16 Y,U16 Color);
 void YADA_52 (void);
 void YADA_5A (U16 *P,U8 Size);
 void YADA_5B (U16 *P,U8 Size);
 void YADA_5C (U16 *P,U8 Size);
 void YADA_60 (U16 *P,U8 C_Dots,U8 Size);
 void YADA_61 (U16 *P,U8 C_Dots,U8 Size);
 void YADA_62 (U16 *P,U8 C_Dots,U8 Size);
 void YADA_63 (U16 *P,U8 C_Dots,U8 Size);
 void YADA_70(U16 ID);
 void YADA_7B (U16 ID);
 void YADA_71 (U16 ID,U16 XS,U16 YS,U16 XE,U16 YE,U16 X,U16 Y);
 void YADA_9C (U16 ID,U16 XS,U16 YS,U16 XE,U16 YE,U16 X,U16 Y);
 void YADA_9D (U16 ID,U16 XS,U16 YS,U16 XE,U16 YE,U16 X,U16 Y);
 void YADA_E2 (U16 ID);
 void YADA_99 (U16 *P,U8 Size);
 void YADA_D0 ( void );
 void YADA_C0 (U16 Addr,U16 *P,U8 Size);
 void YADA_C2 (U16 Addr,U16 Data_Length);
 void YADA_C101 (U16 Addr,U16 Pixel_Number);
 void YADA_C102 (U16 Addr,U16 Line_Number);
 void YADA_C103 (U16 Addr,U16 X,U16 Y,U16 Line_Number,U8 D_X,U8 Dis_X,U8 K_y,U16 Color);
 
 void YADA_C104 (U16 Addr1,U16 X,U16 Y,U16 Line_Number,U8 Dis_X,U16 Color1,U16 Addr0,U16 Color0);

 void YADA_C105 (U16 Addr,U16 X,U16 Y,U16 Line_Number,U8 D_X,U8 Dis_X,U8 M_y,U8 D_y,U16 Color);
 void YADA_C106 (U16 Addr,U16 X,U16 Y,U16 Line_Number,U8 D_X,U8 Dis_X,U8 M_y,U8 D_y,U16 Color,U16 Ymin,U16 Ymax);
 void YADA_C10700 (U16 Addr,U16 X_Len,U16 Y_Len);
 void YADA_C10701 (U16 Addr,U16 X_Len,U16 Y_Len,U16 X,U16 Y,U16 Color,U8 MODE,U16 *P,U8 Size);
 void YADA_C10702 (U16 Addr,U16 X_Len,U16 Y_Len,U16 X,U16 Y,U16 Color);
 void YADA_C108 (U16 Addr,U8 Parameter_N);
 void YADA_C110 (U16 Addr,U8 Frame_Number);
 void YADA_90_RANDOM (U16 Addr,U8 *P,U8 Size);
 void YADA_90_ORDER (U32 Addr,U8 *P,U8 Size);
 void YADA_91 (U32 Addr,U16 Length);
 void YADA_9B00 ( void );
 void YADA_9BFF (U8 RTC_Mode,U8 Text_Mode,U16 Color,U16 X,U16 Y);
 void YADA_9B5A (void);
 void YADA_E7(U8 YY,U8 MM,U8 DD,U8 HH,U8 M,U8 SS);
 void YADA_E0 (U8 TFT_ID,U8 Bode_Set,U8 Para1);
 void YADA_5E(U8 V_ON,U8 V_OFF,U8 ON_TIME);
 void YADA_5F(U8 PWM_T);
 void YADA_1D (U16 ID,U16 XS,U16 YS,U16 XE,U16 YE);
 
/* wk --> add */
 void YADA_E4();

#endif