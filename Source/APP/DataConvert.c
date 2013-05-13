/*******************************************************************************
* File Name        : DataConvert.c
* Author             : LB&TX
* Version            : V1.0.0
* Date                :
* Description        :数据转换函数
*******************************************************************************/
#define  DataCnvt_GLOBALS

#include "includes.h"
/*******************************************************************************
* 函  数  名      : UIValues2HR
* 描      述      :压电流谐波有效值转换为100000倍谐波含有率(无符号的实数转换为4个
                  字节供C108显示用)，显示时控制小数位为3即可，需要将接受到的四
                  个字节电压电流谐波值先转换为float型
* 输      入      : UIValues[]
* 返      回      : HR[]
*******************************************************************************/
void UIValues2HR(float UIValues[],U16 HR[])
{
    float temp=UIValues[0];
    U32 t=0;
    for(U8 i=0; i<50; i++)
    {
        t=(U32)((UIValues[i]/temp)*100000);
        *(HR+2*i)=(t&0xffff0000)>>16;
        *(HR+2*i+1)=t&0x0000ffff;
    }
}
/*************************************************
  Function:       ChartoFloat
  Description:    将SPI接收到的连续四个字节的数据转换成一个浮点数(除以10000还原)
  Input:          CharIn[],输入的字节数组
                  FloatOut[],转换以后的数据存放的起始地址
                  FNum,转换以后的浮点数个数
  Return:         无
  Others:         0321暂时将所有接收到的数据转换成浮点型
                  ???可否进行优化???
*************************************************/
void ChartoFloat(U8 CharIn[],float FloatOut[],U16 FNum,U16 times)
{
//    long  temp; 
    float temp;  // wk --> modifeid 
    for(U16 i=0; i<FNum; i++)
    {
      /* wk --> 此处影响：进入谐波列表项后，程序卡死在这里，触摸屏死掉   --> 解决： 标志位未初始化问题导致*/
        temp=((long)(CharIn[4*i])<<24)+((long)(CharIn[4*i+1])<<16)+((long)(CharIn[4*i+2])<<8)+(long)(CharIn[4*i+3]);
        *(FloatOut+i)=((float)temp)/times;
    }
}
/*******************************************************************************
* 函  数  名      : FloatTo4DecimalsC108
* 描      述      :将浮点数转换为供C108显示用的四个字节，浮点数乘以了10000，因此
                   C108显示时需要控制是4位小数，只用于显示正数
* 输      入      : DataIn[]，U8 Num
* 返      回      : DataBack[]
*******************************************************************************/
/*
void FloatTo4DecimalsC108(float DataIn[],U16 DataBack[],U8 Num)
{
    U32 t=0;
    for(U8 i=0; i<Num; i++)
    {
        t=(U32)((DataIn[i])*10000);
        *(DataBack+2*i)=(t&0xffff0000)>>16;
        *(DataBack+2*i+1)=t&0x0000ffff;
    }
}
*/
/*******************************************************************************
* 函  数  名      : CharToInt
* 描      述      : 将波形采样值扩充并赋给WAVE_UI数组字节变为字，
                    80个字节变为100个字,WordOut个数为100*LineNum
* 输      入      : CharIn[]，LineNum
* 返      回      : WordOut[]
*******************************************************************************/
void CharToInt(U8 CharIn[], U16 WordOut[],U8 LineNum,U8 HalfRange)
{
    U8 i,j,temp;
    U16 abstemp;
    for (j = 0; j < LineNum; j++)
        for (i = 0; i < Cycledot; i++)  // wk -->  Cycledot = 80 //实时波形的点数
        {
            temp=CharIn[i + j*Cycledot];
            abstemp = abs(temp-127) * HalfRange*2/255;
            if((temp-127)<0)
            {
                WordOut[i + j*size_UI] =HalfRange-abstemp; //
            }
            else
            {
                WordOut[i + j*size_UI] =HalfRange+abstemp; //
            }
            if(i<20)
            {
                WordOut[i + j*size_UI+Cycledot]=WordOut[i + j*size_UI];
            }
        }
}
void BCD2Decimal(U8 Data[],U8 Num)///将BCD码转换为10进制数
{
    for(U8 i=0; i<Num; i++)
    {
        Data[i] = (Data[i] >> 4) *10+(Data[i] &0x0f);
    }
}
/*******************************************************************************
* 函  数  名      : PhaseShift
* 描      述      : 电压电流相位转换为与UA的差值，且变为角度。
* 输      入      : DataIn[]，Num
* 返      回      : DataBack[]
*******************************************************************************/
#if 0 // @130420
void PhaseShift(float DataIn[],U16 DataBack[],U8 Num,U32 DATABACK[])
{
    U8 i;
    for(i=0; i<Num; i++)
    {
        if(((DataIn[i]-DataIn[0])<0)==1)
        {
            //DataBack[i]=(int)((DataIn[i]-DataIn[0]+360)*PI/180);
            DataBack[i]=(int)((DataIn[i]-DataIn[0]+360));
            DATABACK[i]=(U32)((DataIn[i]-DataIn[0]+360)*10000);
        }
        else
        {
            //DataBack[i]=(int)((DataIn[i]-DataIn[0])*PI/180);
            DataBack[i]=(int)((DataIn[i]-DataIn[0]));
            DATABACK[i]=(U32)((DataIn[i]-DataIn[0])*10000);
        }
    }
}
#endif
void PhaseShift(float DataIn[],U16 DataBack[],U8 Num,U8 DATBACK[])
{
    U8 i,temp;//
    U32 DATA_TEMP[6]= {0};
    for(i=0; i<Num; i++)
    {
        if((DataIn[i]-DataIn[0])<0)
        {
            //DataBack[i]=(int)((DataIn[i]-DataIn[0]+360)*PI/180);
            DataBack[i]=(int)((DataIn[i]-DataIn[0]+360));
            DATA_TEMP[i]=(U32)((DataIn[i]-DataIn[0]+360)*10);
            temp=4*i;
            DATBACK[temp]=(U8)(DATA_TEMP[i]>>24);
            DATBACK[temp+1]=(U8)(DATA_TEMP[i]>>16);
            DATBACK[temp+2]=(U8)(DATA_TEMP[i]>>8);
            DATBACK[temp+3]=(U8)(DATA_TEMP[i]);
            // DATA_TEMP[i]=(U16)((DataIn[i]-DataIn[0]+360)*10);
            /// temp=2*i;
            // DATBACK[temp+1]=(U8)DATA_TEMP[i];
            //DATBACK[temp]=((U8)DATA_TEMP[i]>>8);
        }
        else
        {
            //DataBack[i]=(int)((DataIn[i]-DataIn[0])*PI/180);
            DataBack[i]=(int)((DataIn[i]-DataIn[0]));
            DATA_TEMP[i]=(U32)((DataIn[i]-DataIn[0])*10);
            temp=4*i;
            DATBACK[temp]=(U8)(DATA_TEMP[i]>>24);
            DATBACK[temp+1]=(U8)(DATA_TEMP[i]>>16);
            DATBACK[temp+2]=(U8)(DATA_TEMP[i]>>8);
            DATBACK[temp+3]=(U8)(DATA_TEMP[i]);
            //DATA_TEMP[i]=(U16)((DataIn[i]-DataIn[0])*10);
            // temp=2*i;
            // DATBACK[temp+1]=(U8)DATA_TEMP[i];
            // DATBACK[temp]=((U8)DATA_TEMP[i]>>8);
        }
    }
}
/*******************************************************************************
* 函  数  名      : WR_WAVE_UI
* 描      述      : 将WAVE_UI写入触摸屏，一次最多248个字节，所以分次写入
                    用C104画波形，需将数据再写入触摸屏。
* 输      入      : DataIn[]，UI_U_I，rewrite
* 返      回      : 无
*******************************************************************************/
void WR_WAVE_UI(U16 DATAIN[],U8 UI_U_I, U8 rewrite)
{
    switch(UI_U_I)
    {
    case 1:
        YADA_C0 (UA_addr + rewrite*size_UI,DATAIN,size_UI);  // #define size_UI 100               //波形采样数据各相总数
        YADA_C0 (UB_addr + rewrite*size_UI,&DATAIN[size_UI],size_UI);
        YADA_C0 (UC_addr + rewrite*size_UI,&DATAIN[size_UI*2],size_UI);
        YADA_C0 (IA_addr + rewrite*size_UI,&DATAIN[size_UI*3],size_UI);
        YADA_C0 (IB_addr + rewrite*size_UI,&DATAIN[size_UI*4],size_UI);
        YADA_C0 (IC_addr + rewrite*size_UI,&DATAIN[size_UI*5],size_UI);
        break;
    case 2:
        YADA_C0 (UA_addr + rewrite*size_UI,DATAIN,size_UI);
        YADA_C0 (UB_addr + rewrite*size_UI,&DATAIN[size_UI],size_UI);
        YADA_C0 (UC_addr + rewrite*size_UI,&DATAIN[size_UI*2],size_UI);
        break;
    case 3:
        YADA_C0 (IA_addr + rewrite*size_UI,DATAIN,size_UI);
        YADA_C0 (IB_addr + rewrite*size_UI,&DATAIN[size_UI],size_UI);
        YADA_C0 (IC_addr + rewrite*size_UI,&DATAIN[size_UI*2],size_UI);
        break;
    default:
        break;
    }
}
/*******************************************************************************
* 函  数  名      : AtoFA
* 描      述      : 将U盘读到的字符串转化为浮点数数组,包含正负
* 输      入      : DATAIN[],待转化的字符数组
* 返      回      : DATAOUT[],转化得到的浮点数数组
*******************************************************************************/
void AtoFA(U8 DATAIN[],float DATAOUT[],U16 NUM)
{
    U16 j=0;
    char TEST[9],k=0;
    for(U16 i=0; i<NUM*48; i++)//每行预设为48个字符
    {
        TEST[k]=DATAIN[i];
        k++;
        if((DATAIN[i]==',')||DATAIN[i]==0x0d)
        {
            DATAOUT[j]=atof(TEST);
            k=0;
            //printf("%f",DATAOUT[j]);
            j++;
            if(j>=NUM*6)
            {
                break;
            }
        }
    }
}
/*******************************************************************************
* 函  数  名      : FLTOINT_UI
* 描      述      : 将U盘读到的电压电流浮点数数组转化为C104显示波形的整型数据
* 输      入      : DATAIN[],电压电流浮点数组
* 返      回      : DATOUT[],输出用于事件波形C104显示的数组
*******************************************************************************/
void FLTOINT_UI(float DATAIN[],int DATOUT[],U16 FLAGNUM)
{
    for(U8 i=0; i<FLAGNUM; i++)
    {
        DATOUT[i]=(int)(DATAIN[i*6]/4)+87;   //电压+-311变为屏幕显示的数据，87为中线值。
        DATOUT[i+FLAGNUM]=(int)(DATAIN[i*6+1]/4)+87;
        DATOUT[i+2*FLAGNUM]=(int)(DATAIN[i*6+2]/4)+87;
        /*DATOUT[i+3*FLAGNUM]=(int)(DATAIN[i*6+3]*14)+87;//电流+-6变为屏幕显示的数据，87为中线值。
        DATOUT[i+4*FLAGNUM]=(int)(DATAIN[i*6+4]*14)+87;
        DATOUT[i+5*FLAGNUM]=(int)(DATAIN[i*6+5]*14)+87;*/
    }
}
/*******************************************************************************
* 函  数  名      :
* 描      述      : 将U盘读到的电流浮点数数组转化为整型，并且改变正负
* 输      入      : DATAIN[],待转化的浮点数数组
* 返      回      : DATOUT1[],DATOUT2[],DATOUT3[]三相电压数组输出
******************************************************************************/
void EVEUISHIFT(U8 DATAIN[],float DATAOUT[])
{
    U16 temp;
    for(U16 i=0; i<EVEUILEN*12; i++)
    {
        temp=((U16)(DATAIN[2*i])<<8)+((U16)(DATAIN[2*i+1]));
        DATAOUT[i]=((float)temp)/100;
    }
}
/*******************************************************************************
* 函  数  名      : U16TOFL_UI
* 描      述      : 将AD的电压电流采样值变为+-311v和+-8.48A以内
* 输      入      : U8 DADAIN[],U8 NUM,UI_FLG,0为电压，1为电流
* 返      回      : 返float DATAOUT[]
*******************************************************************************/
void U16TOFL_UI(U8 DADAIN[],U16 NUM,float DATAOUT[])//NUM为待转换数据个数
{
    for(U16 i=0; i<NUM; i++)
    {
        DATAOUT[i]=((float)((int)(DADAIN[2*i])<<8)+(int)(DADAIN[2*i+1]))/10;//>>7~=/128.0.00026 0.0095
    }
    /*
  for(U16 i=NUM; i<NUM*2; i++)
  {
     DATAOUT[i]=(((int)(DADAIN[2*i])<<8)+(int)(DADAIN[2*i+1]))*0.00026;
  }*/
}
/*******************************************************************************
* 函  数  名      : Sig_Fiq
* 描      述      : 将电能数据按要求的有效数字进行转换
* 输      入      : U8 DADAIN[],U8 TIMES，10为不缩小10倍即3位有效数字,
                    100为缩小100倍即2位有效数字,1000为不缩小1000倍即1位有效数字
* 返      回      : 返float DATAOUT[]
*******************************************************************************/
//void Sig_Fiq(U8 DADAIN[],U8 DATAOUT[],U16 TIMES)
//{
//  long temp;
//  temp=(((long)(DADAIN[0])<<24)+((long)(DADAIN[1])<<16)+((long)(DADAIN[2])<<8)+((long)(DADAIN[3])))/TIMES;
//  DATAOUT[3]=temp;
//  DATAOUT[2]=temp>>8;
//  DATAOUT[1]=temp>>16;
//  DATAOUT[0]=temp>>24;
//}
void Sig_Fiq(U8 DADAIN[],U8 DATAOUT[],U16 TIMES, U8 NUM)
{
    long temp;
    U8 i,j;
    for(i=0; i<NUM; i++)
    {
        j=4*i;
        temp=(((long)(DADAIN[j])<<24)+((long)(DADAIN[1+j])<<16)+((long)(DADAIN[2+j])<<8)+((long)(DADAIN[3+j])))/TIMES;
        DATAOUT[3+j]=temp;
        DATAOUT[2+j]=temp>>8;
        DATAOUT[1+j]=temp>>16;
        DATAOUT[j]=temp>>24;
    }
}

/*******************************************************************************
** Function Name	：num2string
** Input		： type =0 文件夹，=1 .CSV文件
** Return		：转好好的字符串指针
** Author		： wk
** Version	：
** Date		：
** Dessription	： 将 32 位整数转换成字符串
** Reverse	：
*******************************************************************************/
char_ptr num2string(int_32 num,uchar len,uchar type) // wk --> len <= 13-4-1=8
{
  char_ptr name;
  name = _mem_alloc_zero( len+5 );
  uchar sep_data[9];uint_32 temp;
  for(int i=0;i<len;i++)
  {
    temp=(uint_32)pow(10,i);
    sep_data[i]=(num/temp)%10;
  }
  
  for(int i=0;i<len;i++)
  {
    *(name+i)=(uchar)(0x30+sep_data[len-1-i]);
  }
  if(type==0)
  {
    *(name+len)='\0';
  }
  else
  {
    *(name+len)=0x2e; // .
    *(name+len+1)=0x43; // C
    *(name+len+2)=0x53; // S
    *(name+len+3)=0x56; // V
    *(name+len+4)='\0';
  }
  return name;
}
