/*******************************************************************************
* File Name        : LCDDriver.c
* Author             : LB&TX
* Version            : V1.0.0
* Date                :
* Description        :触摸屏的驱动程序
                      本文件与工程的关联处仅在send_byte函数中
                      涉及到液晶屏与430的串口操作，没有用到的子程序可以注释掉。
*******************************************************************************/
/***************************************************
**
** Reverse by wk
** Date: 
** Description:  430  --> k60 
** 
**************************************************/

#include "includes.h"

MQX_FILE_PTR uart_lcd = NULL;

/* Uart initialization for send data*/
void UartLCD_init()
{
  uart_lcd  = fopen( "ttyb:", NULL );
  if( uart_lcd == NULL )
   {
      /* device could not be opened */
      _task_block();
   }
//   else 
//     printf("open ttya OK!\n");
}

void send_byte(U8 i)
{
    write(uart_lcd, &i, 1);
}
void send_word(U16 i)
{
    send_byte(i/256);
    send_byte(i%256);
}
void send_xy(U16 x,U16 y)//发送两个16位的数据
{
    send_word(x);
    send_word(y);
}
void send_area(U16 xs,U16 ys,U16 xe,U16 ye)
{
    send_word(xs);
    send_word(ys);
    send_word(xe);
    send_word(ye);
}
void send_U8str(U8 *p,U8 size)  //串口发送一串数据 (U8)
{
    U8 i=size;
    if(i==0)
    {
        while(*p!='\0')
        {
            send_byte(*p);
            p++;
        }
    }
    else
    {
        while(i)
        {
            send_byte(*p);           //此发送字节而非字
            p++;
            i--;
        }
    }
}
void send_U16str(U16 *q,U8 size)  //串口发送一串数据 (U16)size为0则发送整个字符串
{
    U8 i=size;
    if(i==0)
    {
        while(*q!='\0')
        {
            send_word(*q);
            q++;
        }
    }
    else
    {
        while(i)
        {
            send_word(*q);
            q++;
            i--;
        }
    }
}
void send_end()
{
//    send_xy(0xcc33,0xc33c);
     send_word(0xcc33);
     send_word(0xc33c); 
}

void YADA_00 (void)
{
    send_word(0xaa00);
    send_end();
}
void YADA_40 (U16 FrontC,U16 BackC)
{
    send_word(0xaa40);
    send_xy(FrontC,BackC);
    send_end();
}
void YADA_41 (U8 D_X,U8 D_Y)
{
    send_word(0xaa41);
    send_byte(D_X);
    send_byte(D_Y);
    send_end();
}
void YADA_42 (U16 X,U16 Y)
{
    send_word(0xaa42);
    send_xy(X,Y);
    send_end();
}
void YADA_43 (U16 X,U16 Y)
{
    send_word(0xaa43);
    send_xy(X,Y);
    send_end();
}
void YADA_44 (U8 EN,U16 X,U16 Y,U8 Width,U8 Height)
{
    send_word(0xaa44);
    send_byte(EN);
    send_xy(X,Y);
    send_byte(Width);
    send_byte(Height);
    send_end();
}
void prints(U8 cmd,U16 x,U16 y,U8 *s,U8 size)//显示文本，字符串以0x00结束，cmd可为53/54/55/6E/6F
{
    send_byte(0xAA);
    send_byte(cmd);
    send_xy(x,y);
    send_U8str(s,size);
    send_end();
}
void YADA_53 (U16 X,U16 Y,U8 *P,U8 Size)
{
    prints(0x53,X,Y,P,Size);
}
void YADA_54 (U16 X,U16 Y,U8 *P,U8 Size)
{
    prints(0x54,X,Y,P,Size);
}
void YADA_55 (U16 X,U16 Y,U8 *P,U8 Size)
{
    prints(0x55,X,Y,P,Size);
}
void YADA_6E (U16 X,U16 Y,U8 *P,U8 Size)
{
    prints(0x6E,X,Y,P,Size);
}
void YADA_6F (U16 X,U16 Y,U8 *P,U8 Size)
{
    prints(0x6F,X,Y,P,Size);
}
void YADA_98 (U16 X,U16 Y,U8 Lib_ID,U8 C_Mode,U8 C_Dots,U16 FrontC,U16 BackC,U8 *P,U8 Size)
{
    send_word(0xAA98);
    send_xy(X,Y);
    send_byte(Lib_ID);
    send_byte(C_Mode);
    send_byte(C_Dots);
    send_xy(FrontC,BackC);
    send_U8str(P,Size);
    send_end();
}
void YADA_45(U16 *P,U8 Flag)
{
    send_word(0xAA45);
    if(Flag==0)//关闭文本框限制
    {
        send_byte(0x00);
    }
    else
    {
        send_U16str(P,0);
    }
    send_end();
}
void YADA_50 (U16 *P,U8 Size)//背景色显示点，相当于将点擦除
{
    send_word(0xAA50);
    send_U16str(P,Size);
    send_end();
}
void YADA_51 (U16 *P,U8 Size)//前景色显示点
{
    send_word(0xAA51);
    send_U16str(P,Size);
    send_end();
}
void YADA_74 (U16 X,U16 YS,U16 YE,U16 BC,U16 *P,U8 Size)
{
    send_word(0xAA74);
    send_area(X,YS,YE,BC);
    send_U16str(P,Size);
    send_end();
}
void YADA_56 (U16 *P,U8 Size)
{
    send_word(0xaa56);
    send_U16str(P,Size);
    send_end();
}
void YADA_5D (U16 *P,U8 Size)
{
    send_word(0xaa5D);
    send_U16str(P,Size);
    send_end();
}
void YADA_75 (U16 X,U16 Y,U8 H_Max,U8 *P,U8 Size)
{
    send_word(0xaa75);
    send_xy(X,Y);
    send_byte(H_Max);
    send_U8str(P,Size);
    send_end();
}
void YADA_76 (U16 X,U8 X_Dis,U16 *P,U8 Size)
{
    send_word(0xaa76);
    send_word(X);
    send_byte(X_Dis);
    send_U16str(P,Size);
    send_end();
}
void YADA_78 (U16 X,U16 Y,U8 *P,U8 Size)
{
    send_word(0xaa78);
    send_xy(X,Y);
    send_U8str(P,Size);
    send_end();
}
void YADA_57 (ARC *P,U8 Size)
{
    send_word(0xaa57);
    if(Size==0)
    {
        U16 i=sizeof(P);
        U16 j;
        for(j=0; j<i; j++)
        {
            send_byte(P->style);
            send_xy(P->X_0,P->Y_0);
            send_byte(P->R_0);
            P++;
        }
    }
    else
    {
        U16 k;
        for(k=0; k<Size; k++)
        {
            send_byte(P->style);
            send_xy(P->X_0,P->Y_0);
            send_byte(P->R_0);
            P++;
        }
    }
    send_end();
}
void YADA_5704 (U16 X,U16 Y,U16 R,U16 AS,U16 AE)
{
    send_byte(0xaa);
    send_xy(0x5704,X);
    send_area(Y,R,AS,AE);
    send_end();
}
/*59 -前景色显示1像素线宽的1个或多个矩形外框*/
void YADA_59 (U16 *P,U8 Size)
{
    send_word(0xaa59);
    send_U16str(P,Size);
    send_end();
}
/*69 -背景色显示1像素线宽的1个或多个矩形外框*/
void YADA_69 (U16 *P,U8 Size)
{
    send_word(0xaa69);
    send_U16str(P,Size);
    send_end();
}
void YADA_64 (U16 X,U16 Y,U16 Color)
{
    send_word(0xaa64);
    send_xy(X,Y);
    send_word(Color);
    send_end();
}
void YADA_52 (void)
{
    send_word(0xaa52);
    send_end();
}
/*5A -背景色填充1个或多个矩形区域*/
void YADA_5A (U16 *P,U8 Size)
{
    send_word(0xaa5A);
    send_U16str(P,Size);
    send_end();
}
/*5B -前景色填充1个或多个矩形区域*/
void YADA_5B (U16 *P,U8 Size)
{
    send_word(0xaa5B);
    send_U16str(P,Size);
    send_end();
}
/*5C -反色填充1个或多个矩形区域*/
void YADA_5C (U16 *P,U8 Size)
{
    send_word(0xaa5C);
    send_U16str(P,Size);
    send_end();
}
void shift (U8 cmd,U16 *p,U8 c_Dots,U8 size)
{
    send_byte(0xAA);
    send_byte(cmd);
    send_U16str(p,size);
    send_byte(c_Dots);
    send_end();
}
void YADA_60 (U16 *P,U8 C_Dots,U8 Size)
{
    shift(0x60,P,C_Dots,Size);
}
void YADA_61 (U16 *P,U8 C_Dots,U8 Size)
{
    shift(0x61,P,C_Dots,Size);
}
void YADA_62 (U16 *P,U8 C_Dots,U8 Size)
{
    shift(0x62,P,C_Dots,Size);
}
void YADA_63 (U16 *P,U8 C_Dots,U8 Size)
{
    shift(0x63,P,C_Dots,Size);
}

/*显示一副全图像 */
void YADA_70(U16 ID) 
{
    send_word(0xaa70);
    send_word(ID);
    send_end();
}
void YADA_7B (U16 ID)
{
    send_word(0xaa7B);
    send_word(ID);
    send_end();
}

/* 从指定图片剪切图标粘贴到当前显示页 */
void YADA_71 (U16 ID,U16 XS,U16 YS,U16 XE,U16 YE,U16 X,U16 Y)
{
    send_word(0xaa71);
    send_word(ID);
    send_area(XS,YS,XE,YE);
    send_xy(X,Y);
    send_end();
}
void YADA_9C (U16 ID,U16 XS,U16 YS,U16 XE,U16 YE,U16 X,U16 Y)
{
    send_word(0xaa9C);
    send_word(ID);
    send_area(XS,YS,XE,YE);
    send_xy(X,Y);
    send_end();
}
void YADA_9D (U16 ID,U16 XS,U16 YS,U16 XE,U16 YE,U16 X,U16 Y)
{
    send_word(0xaa9D);
    send_word(ID);
    send_area(XS,YS,XE,YE);
    send_xy(X,Y);
    send_end();
}
void YADA_E2 (U16 ID)
{
    send_word(0xaaE2);
    send_word(ID);
    send_end();
}
void YADA_99 (U16 *P,U8 Size)
{
    send_word(0xaa99);
    send_U16str(P,Size);
    send_end();
}
void YADA_D0 (void)
{
    send_word(0xaad0);
    send_end();
}
void YADA_C0 (U16 Addr,U16 *P,U8 Size)
{
    send_xy(0xAAC0,Addr);
    send_U16str(P,Size);
    send_end();
}
void YADA_C2 (U16 Addr,U16 Data_Length)
{
    send_word(0xAAC2);
    send_xy(Addr,Data_Length);
    send_end();
}
void YADA_C101 (U16 Addr,U16 Pixel_Number)
{
    send_byte(0xAA);
    send_word(0xC101);
    send_xy(Addr,Pixel_Number);
    send_end();
}
void YADA_C102 (U16 Addr,U16 Line_Number)
{
    send_byte(0xAA);
    send_word(0xC102);
    send_xy(Addr,Line_Number);
    send_end();
}
void YADA_C103 (U16 Addr,U16 X,U16 Y,U16 Line_Number,U8 D_X,U8 Dis_X,U8 K_y,U16 Color)
{
    send_byte(0xAA);
    send_word(0xC103);
    send_xy(Addr,X);
    send_xy(Y,Line_Number);
    send_byte(D_X);
    send_byte(Dis_X);
    send_byte(K_y);
    send_word(Color);
    send_end();
}

 /* 使用暂存缓冲区的数据点高速无闪烁连线（示波器） */
void YADA_C104 (U16 Addr1,U16 X,U16 Y,U16 Line_Number,U8 Dis_X,U16 Color1,U16 Addr0,U16 Color0)
{
    send_byte(0xAA);
    send_word(0xC104);
    send_xy(Addr1,X);
    send_xy(Y,Line_Number);
    send_byte(0x01);//自动固定写01
    send_byte(Dis_X);
    send_word(Color1);
    send_xy(Addr0,Color0);
    send_end();
}
void YADA_C105 (U16 Addr,U16 X,U16 Y,U16 Line_Number,U8 D_X,U8 Dis_X,U8 M_y,U8 D_y,U16 Color)
{
    send_byte(0xAA);
    send_word(0xC105);
    send_xy(Addr,X);
    send_xy(Y,Line_Number);
    send_byte(D_X);
    send_byte(Dis_X);
    send_byte(M_y);
    send_byte(D_y);
    send_word(Color);
    send_end();
}
void YADA_C106 (U16 Addr,U16 X,U16 Y,U16 Line_Number,U8 D_X,U8 Dis_X,U8 M_y,U8 D_y,U16 Color,U16 Ymin,U16 Ymax)
{
    send_byte(0xAA);
    send_word(0xC106);
    send_xy(Addr,X);
    send_xy(Y,Line_Number);
    send_byte(D_X);
    send_byte(Dis_X);
    send_byte(M_y);
    send_byte(D_y);
    send_word(Color);
    send_xy(Ymin,Ymax);
    send_end();
}
void YADA_C10700 (U16 Addr,U16 X_Len,U16 Y_Len)
{
    send_xy(0xAAC1,0X0700);
    send_xy(Addr,X_Len);
    send_word(Y_Len);
    send_end();
}
void YADA_C10701 (U16 Addr,U16 X_Len,U16 Y_Len,U16 X,U16 Y,U16 Color,U8 MODE,U16 *P,U8 Size)
{
    send_xy(0xAAC1,0X0701);
    send_area(Addr,X_Len,Y_Len,X);
    send_xy(Y,Color);
    send_byte(MODE);
    send_U16str(P,Size);
    send_end();
}
void YADA_C10702 (U16 Addr,U16 X_Len,U16 Y_Len,U16 X,U16 Y,U16 Color)
{
    send_xy(0xAAC1,0X0702);
    send_area(Addr,X_Len,Y_Len,X);
    send_xy(Y,Color);
    send_end();
}
void YADA_C108 (U16 Addr,U8 Parameter_N)
{
    send_byte(0xAA);
    send_word(0xC108);
    send_word(Addr);
    send_byte(Parameter_N);
    send_end();
}
void YADA_C110 (U16 Addr,U8 Frame_Number)
{
    send_byte(0xAA);
    send_word(0xC110);
    send_word(Addr);
    send_byte(Frame_Number);
    send_end();
}
void YADA_90_RANDOM (U16 Addr,U8 *P,U8 Size)
{
    send_area(0xAA90,0x55aa,0x5aa5,0x01de);
    send_word(Addr);
    send_U8str(P,Size);
    send_end();
}
void YADA_90_ORDER (U32 Addr,U8 *P,U8 Size)
{
    send_xy(0xaa90,0x55aa);
    send_word(0x5aa5);
    send_word(Addr/65536);
    send_word(Addr%65536);
    send_U8str(P,Size);
    send_end();
}
void YADA_91 (U32 Addr,U16 Length)
{
    send_area(0xaa91,Addr/65536,Addr%65536,Length);
    send_end();
}
void YADA_9B00 (void)
{
    send_word(0xaa9B);
    send_byte(0x00);
    send_end();
}
void YADA_9BFF (U8 RTC_Mode,U8 Text_Mode,U16 Color,U16 X,U16 Y)
{
    send_word(0xaa9B);
    send_byte(0xFF);
    send_byte(RTC_Mode);
    send_byte(Text_Mode);
    send_word(Color);
    send_xy(X,Y);
    send_end();
}
void YADA_9B5A (void)
{
    send_word(0xaa9B);
    send_byte(0x5a);
    send_end();
}
void YADA_E7(U8 YY,U8 MM,U8 DD,U8 HH,U8 M,U8 SS)
{
    send_xy(0xaae7,0x55aa);
    send_word(0x5aa5);
    send_byte(YY);
    send_byte(MM);
    send_byte(DD);
    send_byte(HH);
    send_byte(M);
    send_byte(SS);
    send_end();
}
void YADA_5E(U8 V_ON,U8 V_OFF,U8 ON_TIME)
{
    U8 dat[4]= {0x55,0xaa,0x5a,0xa5};
    send_word(0xaa5E);
    if(V_ON!=0xFF)
    {
        send_U8str(dat,4);
        send_byte(V_ON);
        send_byte(V_OFF);
        send_byte(ON_TIME);
    }
    send_end();
}
void YADA_5F(U8 PWM_T)
{
    send_word(0xaa5F);
    if(PWM_T!=0xFF)
    {
        send_byte(PWM_T);
    }
    send_end();
}
void YADA_E0 (U8 TFT_ID,U8 Bode_Set,U8 Para1)
{
    send_xy(0xaae0,0x55aa);
    send_word(0x5aa5);
    send_byte(TFT_ID);
    send_byte(Bode_Set);
    send_byte(Para1);
    send_end();
}

/* add --> wk */
 void YADA_E4()  // 触摸屏校准
 {
   send_word(0xaaE4);
   send_word(0x55aa);
   send_word(0x5aa5);
   send_end();
 }