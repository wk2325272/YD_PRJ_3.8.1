/*******************************************************************************
* File Name        : MenuView.c
* Author             : LB&TX
* Version            : V1.0.0
* Date                :
* Description        :LCD显示界面的数据显示页面
*******************************************************************************/
/***************************************************
**
** Reverse by wk
** Date: 
** Description:  430  --> k60 
** 
**************************************************/

#include "includes.h"

/* wk @130409 --> 变量定义 & 申明 */
//extern volatile U8 EVEnum,send_ok;  // wk @130403 --> uncomment
U8 EVEnum=0,EveRdNum=1;
const U16 COLOR[] = {0xffe0,0x07E0,0xf800,0x0000};
U8 HarmoInfo[][8]= {"A","B","C","幅 值","含有率"};
//U8 text[][3]= {"Ua","Ub","Uc","Ia","Ib","Ic"};//ColorF[9]= {0};
U8 SysParaOldIndex=0,SysEventOldIndex=0,EventOldIndex=0,EVEnum_old;
U8 SysFlashDataT[70]={0,0,0,0,0,1,1,0};   //系统设置的数据的临时参数
U8 SysFlashData[70]={0,0,0,0,0,1,1,0};   //wk @130326 -->写入Flash的系统设置参数
uchar SysDataSend[56]; // K60写给DSP的数据
U16 EventNum[9];  // wk @130405 --> 9次事件发生次数保存，每个事件占2字节，能记录65535次
U32 EventAddr[100]; // wk@130405 -->记录事件发生的时间：月、日、时、分、秒，每个占4字节
U8 USB_Flg=0;  // wk @130407 --> USB 是否插入标志
U8 time[7];
U8 SavePowerFlg; // WK @130401 --> 电能质量数据存储标志 1时存储
/* wk@130513 --> 事件和基本电能数据存储用变量*/
char file_nameES[12]="wk12345.csv",dir_nameES[5]="2013",monthDir_nameES[3]="12";       
uint_16 month_oldES=0;
U16 year_oldES=0;        
char file_namePS[10]="wk384.csv",dir_namePS[5]="1000";
uint_16 year_oldPS=0;
/* wk@130508 --> 用于波形显示，后期需要优化 */
U8 EvntWave[6144]={0}; // wk @130504 --> 定义局部变量时，程序跑飞，暂时定义成全局变量
U16 EVEUI[768];
 
extern U8 PowRxchar[],EvntRxchar[];
//volatile U8 npage=0; // wk @130403 --> uncomment
//volatile U16 nBlock=0,nBlock_old=1024; // wk @130403 --> uncomment

/*******************************************************************************
* 函  数  名      : GUI_VIEW_UI
* 描      述      : 电流电压波形及有效值显示，颜色为黄绿红，采用C104指令可自动擦除。
* 输      入      : 无
* 返      回      : 无
*******************************************************************************/
void GUI_VIEW_UI(void)
{
    U16 Coord_UI[]= {14,68,614,225,273,429};   //剪切及原点坐标
    U16 WAVE_UI[6*size_UI]= {0};  // wk -->  size_UI = 100
    CharToInt(&PowRxchar[WAVEUI_INDEX],WAVE_UI,6,75);   //将波形采样值扩充并赋给WAVE_UI数组
    linemark(48,0); //电压电流图标注上面的电压有效值及单位
    delay_ms(5);
    linemark(254,1);  //电压电流图标注下面的电流有效值及单位
    delay_ms(5);
    WR_WAVE_UI(WAVE_UI,1,0);                      //将所有采样数据写入液晶屏的缓冲区
    // wk -->                                  85          7              
    YADA_C104 (UA_addr,Coord_UI[0],Coord_UI[3],UI_LINE_NUM,UI_DX,COLOR[0],UA_addr + size_UI,COLOR[3]); 
    YADA_C104 (UB_addr,Coord_UI[0],Coord_UI[3],UI_LINE_NUM,UI_DX,COLOR[1],UB_addr + size_UI,COLOR[3]);
    YADA_C104 (UC_addr,Coord_UI[0],Coord_UI[3],UI_LINE_NUM,UI_DX,COLOR[2],UC_addr + size_UI,COLOR[3]);
    YADA_C104 (IA_addr,Coord_UI[0],Coord_UI[5],UI_LINE_NUM,UI_DX,COLOR[0],IA_addr + size_UI,COLOR[3]);
    YADA_C104 (IB_addr,Coord_UI[0],Coord_UI[5],UI_LINE_NUM,UI_DX,COLOR[1],IB_addr + size_UI,COLOR[3]);
    YADA_C104 (IC_addr,Coord_UI[0],Coord_UI[5],UI_LINE_NUM,UI_DX,COLOR[2],IC_addr + size_UI,COLOR[3]);
    delay_ms(5);
    WR_WAVE_UI(WAVE_UI,1,1);                       //将所有原采样数据写入触摸屏的另一地址
}
/*******************************************************************************
* 函  数  名      : GUI_VIEW_U
* 描      述      : 电压波形及有效值显示，颜色为黄绿红，采用C104指令可自动擦除。
* 输      入      : 无
* 返      回      : 无
*******************************************************************************/
void GUI_VIEW_U(void)
{
    U16 WAVE_U[3*size_UI]= {0};
    U16 Coord_U[]= {14,86,614,174,212,295,330,415};    //剪切及原点坐标a
    linemark(48,0);                                    //电压图标注上面的电压,及有效值
    delay_ms(5);
    CharToInt(&PowRxchar[WAVEUI_INDEX],WAVE_U,3,55);   //将波形采样值扩充并赋给WAVE_UI数组
    WR_WAVE_UI(WAVE_U,2,0);                     //将所有采样数据写入触摸屏
    YADA_C104 (UA_addr,Coord_U[0],185,U_I_LINE_NUM,U_I_DX,COLOR[0],UA_addr + size_UI,COLOR[3]);
    YADA_C104 (UB_addr,Coord_U[0],306,U_I_LINE_NUM,U_I_DX,COLOR[1],UB_addr + size_UI,COLOR[3]);
    YADA_C104 (UC_addr,Coord_U[0],428,U_I_LINE_NUM,U_I_DX,COLOR[2],UC_addr + size_UI,COLOR[3]);
    delay_ms(5);
    WR_WAVE_UI(WAVE_U,2,1);                      //将所有原采样数据写入触摸屏的另一地址
    // delay_ms(50);
}
/*******************************************************************************
* 函  数  名      : GUI_VIEW_I
* 描      述      : 电流波形及有效值显示，颜色为黄绿红，采用C104指令可自动擦除。
* 输      入      : 无
* 返      回      : 无
*******************************************************************************/
void GUI_VIEW_I(void)
{
    U16 WAVE_I[3*size_UI]= {0};
    U16 Coord_I[]= {14,86,614,174,212,295,330,415};      //剪切及原点坐标
    linemark(48,1);                                      //电流图标注上面的电流,及有效值
    delay_ms(5);
    CharToInt(&PowRxchar[WAVEUI_INDEX+240],WAVE_I,3,55);   //将波形采样值扩充并赋给WAVE_UI数组
    WR_WAVE_UI(WAVE_I,3,0);                   //将所有采样数据写入触摸屏
    YADA_C104 (IA_addr,Coord_I[0],185,U_I_LINE_NUM,U_I_DX,COLOR[0],IA_addr + size_UI,COLOR[3]);
    YADA_C104 (IB_addr,Coord_I[0],306,U_I_LINE_NUM,U_I_DX,COLOR[1],IB_addr + size_UI,COLOR[3]);
    YADA_C104 (IC_addr,Coord_I[0],428,U_I_LINE_NUM,U_I_DX,COLOR[2],IC_addr + size_UI,COLOR[3]);
    WR_WAVE_UI(WAVE_I,3,1);                      //将所有原采样数据写入触摸屏的另一地址
    //delay_ms(50);
}
/*******************************************************************************
* 函  数  名      : GUI_VIEW_VECT
* 描      述      : 电流电压向量及相角显示，颜色为黄绿红，采用56指令画向量，C108
                    指令显示相角值。
* 输      入      : 无
* 返      回      : 无
*
* 修       改     : WK
* 时       间     : 2013-03-13
* 描       述     : 基于2013-03-08界面
*******************************************************************************/
void GUI_VIEW_VECT(void)
{
    U8 text[][3]= {"Ua","Ub","Uc","Ia","Ib","Ic"}; 
    U16 VI_line[4];                                 //一条向量的数组
    U16 C108Dat[42] = {0};                          //剪切及原点坐标
    U16 VEC_ORIG_YCOORD=95;                        //相位的初始Y 坐标
    U16 Vec_Angle1[6]= {0};
    U8 temp1,Angle_DIS[24],temp2;
    float Vec_Anglefloat[6]= {0.0},temp;                   //向量的浮点型，调用方便
    YADA_71 (MenuViewVector,0,40,366,417,0,40);
    //电压电流各向量及标识
    ChartoFloat(&PowRxchar[VEC_INDEX],Vec_Anglefloat,6,10000);
    PhaseShift(Vec_Anglefloat,Vec_Angle1,6,Angle_DIS);
    for(U8 j=0; j<3; j++)
    {
        for(U8 i=0; i<2; i++)
        {
            temp=Vec_Angle1[i*3+j]*PI/180;
            YADA_40(COLOR[j],0x0000);
            VI_line[0]=Vec_X0;
            VI_line[1]=Vec_Y0;
            VI_line[2]=(U16)(Vec_X0+Vec_R0*cos(temp));
            VI_line[3]=(U16)(Vec_Y0-Vec_R0*sin(temp));
            YADA_98((U16)(Vec_X0+(Vec_R0 - 30*i - 10)*cos(temp)),(U16)(Vec_Y0-(Vec_R0 - 30*i - 10)*sin(temp)),0x22,0x81,0x01,COLOR[j],0x0000,text[j+i*3],0);//16是根据显示文本的点阵大小确定的，使文本在背景范围内显示
            YADA_56(VI_line,4);//前景色划直线
            delay_ms(2);
        }
    }
    //Sig_Fiq(&PowRxchar[VEC_INDEX],Angle_DIS,1000,2);
    //电压电流各相角的实时数值显示
    for (U8 k = 0; k < 2; k++)
    {
        for(U8 i=0; i<3; i++)
        {
            temp2=12*k+4*i;
            temp1=21*k + 7*i;
            C108Dat[temp1 + 0] = 0x3104;         //P  显示数据的模式
            C108Dat[temp1 + 1] = 455;            //显示相位的X坐标
            C108Dat[temp1 + 2] = VEC_ORIG_YCOORD + i*44 + k*158;     //Y坐标
            C108Dat[temp1 + 3] = C108FC_W;         //白色，下为黑色
            C108Dat[temp1 + 4] = 0x0000;
            C108Dat[temp1 + 5] = ((U16)(Angle_DIS[temp2])<<8)+((U16)Angle_DIS[temp2+1]);//两个字节转为一个字
            C108Dat[temp1 + 6] = ((U16)(Angle_DIS[temp2+2])<<8)+((U16)Angle_DIS[temp2+3]);
        }
    }
    YADA_C0(Vectoraddr,C108Dat,42);
    YADA_C108(Vectoraddr,6);
    delay_ms(5);  
}
/*******************************************************************************
* 函  数  名      : GUI_VIEW_ListMeasure
* 描      述      : C108数据显示页下的数值列表-电能计量数据显示，三相的U,I,P,S,Q,
                    PF,F。
* 输      入      : 无
* 返      回      : 无
*
* 修       改     : WK
* 时       间     : 2013-03-14
* 描       述     : 基于2013-03-08界面 : 修改了 C108Data 数组存放数据顺序以及 显示的算法
*******************************************************************************/
void GUI_VIEW_ListMeasure()   // wk --> 电能计量参数
{
#if 0 // wk @130408--> revrese dot old
//    U16 C108Data[147]= {0};//21个数据，存放顺序为U-P-Q-f-I-S-PF // wk -->
   /* WK --> */
      U16 C108Data[147]= {0};//21个数据，存放顺序为U-I-P-Q-S-PF-f
      
      for(int i=0;i<7;i++)
      {
        for(int abc=0;abc<3;abc++)// wk --> 切换ABC
        {
          C108Data[21*i+7*abc+0]=C108Mode_64;//显示数据的模式
          C108Data[21*i+7*abc+1]=164+abc*152;//左边列数据的X：79,右边X:434  148
          C108Data[21*i+7*abc+2]=135+i*45;//Y坐标43
          C108Data[21*i+7*abc+3]=C108FC_W;
          C108Data[21*i+7*abc+4]=C108BC_Bk;
          if(i<2)
          {
            C108Data[21*i+7*abc+5]=((U16)(PowRxchar[8*i+16*abc+UI_VIRTUAL_INDEX])<<8)+
                                   (U16)(PowRxchar[8*i+16*abc+UI_VIRTUAL_INDEX+1]);
            C108Data[21*i+7*abc+6]=((U16)(PowRxchar[8*i+16*abc+UI_VIRTUAL_INDEX+2])<<8)+
                                   (U16)(PowRxchar[8*i+16*abc+UI_VIRTUAL_INDEX+3]);
          }
          else
          {
             C108Data[21*i+7*abc+6]=((U16)(PowRxchar[20*abc+i*4+PQSf_INDEX])<<8)+
                                             (U16)(PowRxchar[20*abc+i*4+PQSf_INDEX+1]);
             C108Data[21*i+7*abc+7]=((U16)(PowRxchar[20*abc+i*4+PQSf_INDEX+2])<<8)+
                                             (U16)(PowRxchar[20*abc+i*4+PQSf_INDEX+3]);
          }
        }
      }
#endif
      /* WK --> END */
#if 1 // wk @130408--> revrese dot tx
    U16 C108Data[147]= {0};//21个数据，存放顺序为U-P-Q-f-I-S-PF
    U8 temp,temp1,DATA_DIS1[84]= {0};
    Sig_Fiq(PowRxchar,DATA_DIS1,100,3);
    Sig_Fiq(&PowRxchar[I_VIRTUAL_INDEX],&DATA_DIS1[I_VIRTUAL_INDEX],10,3);
    Sig_Fiq(&PowRxchar[PQS_INDEX],&DATA_DIS1[PQS_INDEX],100,9);
    Sig_Fiq(&PowRxchar[PF_INDEX],&DATA_DIS1[PF_INDEX],1,3);
    Sig_Fiq(&PowRxchar[F_INDEX],&DATA_DIS1[F_INDEX],10,3);
    for(U8 ui=0; ui<7; ui++) //切换UI
    {
        for(U8 lcv_u=0; lcv_u<3; lcv_u++)//切换ABC
        {
            temp=21*ui+7*lcv_u;
            temp1=12*ui+4*lcv_u;
            if((ui==1)||(ui==6))
            {
                C108Data[temp+0]=C108Mode_63;//显示数据的模式
                C108Data[temp+1]=179+lcv_u*153;//左边列数据的X：79,右边X:434
            }
            else if(ui==5)
            {
                C108Data[temp+0]=C108Mode_64;//显示数据的模式
                C108Data[temp+1]=167+lcv_u*153;//左边列数据的X：79,右边X:434
            }
            else
            {
                C108Data[temp+0]=C108Mode_62;//显示数据的模式
                C108Data[temp+1]=191+lcv_u*153;//左边列数据的X：79,右边X:434
            }
            //C108Data[temp+1]=167+lcv_u*153;//左边列数据的X：79,右边X:434
            C108Data[temp+2]=135+ui*43;//Y坐标43
            C108Data[temp+3]=C108FC_W;
            C108Data[temp+4]=C108BC_Bk;
            C108Data[temp+5]=((U16)(DATA_DIS1[temp1])<<8)+(U16)(DATA_DIS1[temp1+1]);
            C108Data[temp+6]=((U16)(DATA_DIS1[temp1+2])<<8)+(U16)(DATA_DIS1[temp1+3]);
        }
    }
#endif
    
    YADA_C0(DMMPowerInfoAdr,C108Data,84);
    YADA_C108(DMMPowerInfoAdr,12);
    delay_ms(5);
    YADA_C0(DMMPowerInfoAdr+168,&C108Data[84],63);
    YADA_C108(DMMPowerInfoAdr+168,9);
    delay_ms(5);
 
}
/*******************************************************************************
* 函  数  名      : GUI_VIEW_ListQuality
* 描      述      : C108数据显示页下的数值列表-电能质量数据显示，三相的PST,PlT,
                    eu，ei,U_THD,I_THD。
* 输      入      : 无
* 返      回      : 无
*
* 修       改     : WK
* 时       间     : 2013-03-14
* 描       述     : 基于2013-03-08界面 : 修改了 ListQC108 数组存放数据顺序以及 显示的算法
                    &&&&&& 显示的电能质量数据还没有赋给数据 &&&&&&&&
*******************************************************************************/
void GUI_VIEW_ListQuality() // wk --> 电能质量参数
{
#if 0 // wk @130408--> revrese dot old
    U8 temp1;
    U16 ListQC108[6*3*7];//24个数据 显示顺序：
    for(int num=0;num<6;num++) //数据类型切换 
      for(int abc=0;abc<3;abc++)//ABC切换
      {
        temp1=8*num+16*abc;
        ListQC108[num*21+abc*7]=C108Mode_64;//显示数据的模式
        ListQC108[num*21+abc*7+1]=164+abc*152;//显示左边列数据的X坐标75
        ListQC108[num*21+abc*7+2]=150+num*40;// Y轴坐标
        ListQC108[num*21+abc*7+3]=C108FC_W;
        ListQC108[num*21+abc*7+4]=C108BC_Bk;
        /* wk --> 显示的电能质量数据*/
         
        ListQC108[num*21+abc*7+5]=((U16)(PowRxchar[temp1+Pst_INDEX])<<8)+(U16)(PowRxchar[temp1+Pst_INDEX+1]);// -----------------待补充
        ListQC108[num*21+abc*7+6]=((U16)(PowRxchar[temp1+Pst_INDEX+2])<<8)+(U16)(PowRxchar[temp1+Pst_INDEX+3]);// -----------------待补充
      }
    
    YADA_C0(DMMPowerInfoAdr,ListQC108,9*7);
    YADA_C108(DMMPowerInfoAdr,9);
    delay_ms(5);
    YADA_C0(DMMPowerInfoAdr+168,&ListQC108[9*7],9*7);
    YADA_C108(DMMPowerInfoAdr+168,9);
    delay_ms(5);
#endif //wk -->
    
#if 1 // wk @130408--> revrese dot tx
        U16 ListQC108[126];//14个数据,按Pst-Plt-e-THDU-THDI的顺序存放
    U8 temp,temp1,DATA_DIS2[72]= {0};
    Sig_Fiq(&PowRxchar[U_ERR_INDEX],DATA_DIS2,10,6);
    Sig_Fiq(&PowRxchar[U_ERR_INDEX+24],&DATA_DIS2[24],100,6);
    Sig_Fiq(&PowRxchar[U_ERR_INDEX+48],&DATA_DIS2[48],10,6);
    for(U8 lcv_sl=0; lcv_sl<6; lcv_sl++)//先Pst后Plt
    {
        for(U8 lcv_slABC=0; lcv_slABC<3; lcv_slABC++)
        {
            temp=21*lcv_sl+7*lcv_slABC;
            temp1=12*lcv_sl+4*lcv_slABC;
            if((lcv_sl==2)||(lcv_sl==3))
            {
                ListQC108[temp+0]=C108Mode_62;//显示数据的模式
                ListQC108[temp+1]=191+152*lcv_slABC;//左边数据X:75
            }
            else
            {
                ListQC108[temp+0]=C108Mode_63;//显示数据的模式
                ListQC108[temp+1]=179+152*lcv_slABC;//左边数据X:75
            }
            // ListQC108[temp+1]=167+152*lcv_slABC;//左边数据X:75
            ListQC108[temp+2]=143+lcv_sl*41;//UY坐标43，IY坐标145
            ListQC108[temp+3]=C108FC_W;
            ListQC108[temp+4]=C108BC_Bk;
            ListQC108[temp+5]=((U16)(DATA_DIS2[temp1])<<8)+(U16)(DATA_DIS2[temp1+1]);
            ListQC108[temp+6]=((U16)(DATA_DIS2[temp1+2])<<8)+(U16)(DATA_DIS2[temp1+3]);
        }
    }
    YADA_C0(ListQualityInfoAdr,ListQC108,63);
    YADA_C108(ListQualityInfoAdr,9);
    delay_ms(5);
    YADA_C0(ListQualityInfoAdr+126,&ListQC108[63],63);
    YADA_C108(ListQualityInfoAdr+126,9);
    delay_ms(5);
#endif
    
}

/*******************************************************************************
* 函  数  名      : GUI_VIEW_ListQuality2
* 描      述      : 三相不平衡所有显示函数
* 输      入      : 无
* 返      回      : 无
*
* 修       改     : WK
* 时       间     : 2013-03-14
* 描       述     : 基于2013-03-08界面 : 修改了 C108Data 数组存放数据顺序以及 显示的算法
*******************************************************************************/
void GUI_VIEW_ListQuality2(U8 U_DISK)
{
#if 0 // wk @130408--> revrese dot old
  U16 BlockC108[5*2*7]={0};
  for(int num=0;num<5;num++)
    for(int ui=0;ui<2;ui++)
    {
      BlockC108[num*14+ui*7]=0x3404;
      BlockC108[num*14+ui*7+1]= 270+ui*210;
      BlockC108[num*14+ui*7+2]= 160+num*45;
      
      BlockC108[num*14+ui*7+3]=C108FC_W;
      BlockC108[num*14+ui*7+4]=C108BC_Bk;
      /* wk --> 显示的电能质量数据 */
      BlockC108[num*14+ui*7+5]=0;  // ----------------待补充
      BlockC108[num*14+ui*7+6]=0;  // ----------------待补充
    }
  YADA_C0(ListUnblanceAdr, BlockC108,10*7);
  YADA_C108(ListUnblanceAdr,10);
#endif // wk --> end

#if 1 // wk @130408--> revrese dot tx
  U8 j,k,index=0,DATA_DIS3[40]= {0};
    U16 BlockC108[70]= {0},temp=0;
    Sig_Fiq(&PowRxchar[UNBLA_INDEX],DATA_DIS3,100,10);
    for(j=0; j<5; j++)
        for(k=0; k<2; k++)
        {
            temp=4*k+8*j;
            index=7*k+14*j;
            BlockC108[index]=0x3204;//显示数据的模式
            BlockC108[index+3]=C108FC_W;
            BlockC108[index+4]=0x0000;
            BlockC108[index+1]=241+k*212;//X +80*j
            BlockC108[index+2]=148+j*47;//Y坐标
            BlockC108[index+5]=((U16)(DATA_DIS3[temp])<<8)+((U16)DATA_DIS3[temp+1]);
            BlockC108[index+6]=((U16)(DATA_DIS3[temp+2])<<8)+((U16)DATA_DIS3[temp+3]);
        }
    YADA_C0(ListUnblanceAdr, BlockC108,70);
    YADA_C108(ListUnblanceAdr,10);
#endif
}

/*******************************************************************************
* 函  数  名      : GUI_VIEW_ListQuality
* 描      述      : 谐波柱状图显示函数。可通过移位键选择三相或者谐波总次数，
                    上下键来选择各单一次数，并显示其电流电压值
* 输      入      : 无
* 返      回      : 无
*
* 修       改     : WK
* 时       间     : 2013-03-14
* 描       述     : 基于2013-03-08界面 
*******************************************************************************/
void GUI_VIEW_HarmoGraph() 
{
#if 1
    float Graphfloat[52]= {0};
    U16 UHarmoBarXY[104]= {0};//26次谐波，共26*4个点
    U16 IHarmoBarXY[104]= {0}; 
    U8 HarmoInfoZone[][10]= {"第01-26次","第25-50次"};//WK -->
    U16 temp,temp1;
    U8 HARM_DIS[8]= {0};
//    U16 square[16]= {14,42,16,65,91,42,93,65,414,79,416,102,414,264,416,287}; //光标位置
    
    U16 HarmoUI[28]= {0x2004,310,78,0xffff,0x0000,0x0000,HarmoGraphUorder,  //0x03ae 表主体背景色
                      0x2004,310,265,0xffff,0x0000,0x0000,HarmoGraphIorder,
                      0x3304,0x01FC,0x004E,0xffff,0x0000,0x0000,0x0000,
                      0x3304,0x0200,0x0108,0xffff,0x0000
                     };//根据放大倍数控制格式
    /* wk --> 影响返回主界面，原因待进一步分析中。。。--> 已解决 */
    // wk @130408--> revrese dot old
//    HarmoUI[19]=((U16)(PowRxchar[600*HarmoGraphPhase+4*HarmoGraphUorder-604+Harmo_INDEX])<<8)
//                +(U16)(PowRxchar[600*HarmoGraphPhase+4*HarmoGraphUorder-603+Harmo_INDEX]);
//    HarmoUI[20]=((U16)(PowRxchar[600*HarmoGraphPhase+4*HarmoGraphUorder-602+Harmo_INDEX])<<8)
//                +(U16)(PowRxchar[600*HarmoGraphPhase+4*HarmoGraphUorder-601+Harmo_INDEX]);
//    HarmoUI[26]=((U16)(PowRxchar[600*HarmoGraphPhase+4*HarmoGraphIorder-404+Harmo_INDEX])<<8)
//                +(U16)(PowRxchar[600*HarmoGraphPhase+4*HarmoGraphIorder-403+Harmo_INDEX]);
//    HarmoUI[27]=((U16)(PowRxchar[600*HarmoGraphPhase+4*HarmoGraphIorder-402+Harmo_INDEX])<<8)
//                +(U16)(PowRxchar[600*HarmoGraphPhase+4*HarmoGraphIorder-401+Harmo_INDEX]);
//    ChartoFloat(&PowRxchar[600*(HarmoGraphPhase-1)+(HarmoGraphRange-1)*96+Harmo_INDEX], Graphfloat,26,1000);//转换电压数据，！放大倍数
//    ChartoFloat(&PowRxchar[600*(HarmoGraphPhase-1)+(HarmoGraphRange-1)*96+Harmo_INDEX+200], &Graphfloat[26],26,1000);//转换电流数据
    // wk @130408--> revrese dot tx
    temp=600*HarmoGraphPhase+4*HarmoGraphUorder+Harmo_INDEX;
    Sig_Fiq(&PowRxchar[temp-604],HARM_DIS,100,1);
    Sig_Fiq(&PowRxchar[temp-404],&HARM_DIS[4],10,1);
    HarmoUI[19]=((U16)(HARM_DIS[0])<<8)
                +(U16)(HARM_DIS[1]);
    HarmoUI[20]=((U16)(HARM_DIS[2])<<8)
                +(U16)(HARM_DIS[3]);
    HarmoUI[26]=((U16)(HARM_DIS[4])<<8)
                +(U16)(HARM_DIS[5]);
    HarmoUI[27]=((U16)(HARM_DIS[6])<<8)
                +(U16)(HARM_DIS[7]);
    temp1=600*(HarmoGraphPhase-1)+(HarmoGraphRange-1)*96+Harmo_INDEX;
    ChartoFloat(&PowRxchar[temp1], Graphfloat,26,10000);//转换电压数据，！放大倍数
    ChartoFloat(&PowRxchar[temp1+200], &Graphfloat[26],26,10000);//转换电流数据
    // wk @130408--> revrese dot end
    
    /* wk --> end */
    //电压范围0~300,分三级显示0~3.0,3.0~300
    //循环构建柱状图的数组，每一个数据转换成矩形的（Xe，Ye）（Xs，Ys）
    for (U16 lcv_uho=0; lcv_uho < 26; lcv_uho++)
    {
        UHarmoBarXY[4*lcv_uho]=91+20*lcv_uho;//Xe
        if (Graphfloat[lcv_uho] <= 3 )//幅值小于2.4V
        {
            UHarmoBarXY[4*lcv_uho+1]=(U16)(248.0-Graphfloat[lcv_uho] *14.667);
        }
        else if(Graphfloat[lcv_uho]  > 3 && Graphfloat[lcv_uho] < 30)
        {
            UHarmoBarXY[4*lcv_uho+1]=(U16)(204.0-(Graphfloat[lcv_uho] -3.0)*1.63);
        }
        else if (Graphfloat[lcv_uho] >= 30)
        {
            UHarmoBarXY[4*lcv_uho+1]=(U16)(160.0-(Graphfloat[lcv_uho] -30)*0.163);
        }
        UHarmoBarXY[4*lcv_uho+2]=104+20*lcv_uho;//Xs
        UHarmoBarXY[4*lcv_uho+3]=248;//Ys
    }
    //电流范围0~20A，分3级，0~0.2A，0.2~2A，2~20A
    for ( U16 lcv_iho=0; lcv_iho <26; lcv_iho++)
    {
        IHarmoBarXY[4*lcv_iho]=91+20*lcv_iho;//Xe
        if (Graphfloat[lcv_iho+26]  <= 0.2 )//幅值小于0.2A
        {
            IHarmoBarXY[4*lcv_iho+1]=(U16)(434-Graphfloat[lcv_iho+26] *220);
        }
        else if (Graphfloat[lcv_iho+26] > 0.2 && Graphfloat[lcv_iho+26] < 2)
        {
            IHarmoBarXY[4*lcv_iho+1]=(U16)(390-(Graphfloat[lcv_iho+26] -0.2)*24.444);
        }
        else if (Graphfloat[lcv_iho+26] >= 2)
        {
            IHarmoBarXY[4*lcv_iho+1]=(U16)(346-(Graphfloat[lcv_iho+26] -2)*2.444);
        }
        IHarmoBarXY[4*lcv_iho+2]=104+20*lcv_iho;
        IHarmoBarXY[4*lcv_iho+3]=434;
    }
    
    YADA_71(MenuViewHarmoGraph1,18,39,63,67,18,39);  // wk --> ?
    YADA_71(MenuViewHarmoGraph1,95,39,191,69,95,39);  // wk --> 清楚上一次显示的相位值

    YADA_98(165,40,0x22,0x81,0x02,0xffff,0x0000,HarmoInfoZone[HarmoGraphRange-1],0); //显示谐波区间
    YADA_98(95,40,0x22,0x81,0x02,0xffff,0x0000,HarmoInfo[HarmoGraphPhase-1],0);//显示相信息A、B、C. // HarmoGraphPhase默认值是 1
    
    YADA_C0(HarmoGraphInfoAdr,HarmoUI,28);
    YADA_C108(HarmoGraphInfoAdr,4);
    delay_ms(5);
    YADA_40(0xf800,0x0000);
    YADA_71(MenuViewHarmoGraph1,85,114,616,251,85,114);//刷新电压柱状图显示区域
    YADA_5B(UHarmoBarXY,104);//画电压柱状图
    delay_ms(5);
    YADA_71(MenuViewHarmoGraph1,85,300,616,480,85,300);//刷新电流柱状图显示区域
    YADA_5B(IHarmoBarXY,104);//画电流柱状图
    delay_ms(5);
#endif    // #if 1
}

/*******************************************************************************
* 函  数  名      : GUI_VIEW_HarmoList
* 描      述      : 谐波列表显示函数。可通过移位键选择三相或者谐波总次数
* 输      入      : 无
* 返      回      : 无
*
* 修       改     : WK
* 时       间     : 2013-03-14
* 描       述     : 基于2013-03-08界面 
*******************************************************************************/
void GUI_VIEW_HarmoList()
{
#if 0 // wk @130408--> revrese dot old
    U16 HRU16[100]= {0};
    U16 ListC108[182]= {0},IndexInit;
    float Listfloat[50]= {0};
        
    YADA_98(58,42,0x22,0x81,0x02,0xffff,0x0000,HarmoInfo[HarmoListPhase-1],0);//显示相信息A、B、C.
    delay_ms(5);
    if(HarmoListUorI==1)
    {
        YADA_98(205,42,0x22,0x81,0x02,0xffff,0x0000,"U",0);
    }
    else
    {
        YADA_98(205,42,0x22,0x81,0x02,0xffff,0x0000,"I",0);
    }
        
    delay_ms(5);
    if(HarmoListAmporRatio==1)//显示幅值时不需要转换
    {
        YADA_98(330,41,0x22,0x81,0x02,0xffff,0x0000,HarmoInfo[3],0); // wk @130409 -->显示幅值
        IndexInit=(HarmoListPhase-1)*600+(HarmoListUorI-1)*200+(HarmoListRange-1)*96+Harmo_INDEX;
        for(U8 LorR=0; LorR<2; LorR++) //先左边列后右边列
        {
            for(U8 i=0; i<13; i++)
            {
                ListC108[7*i+91*LorR]=0x3304;//显示数据的模式
                ListC108[7*i+91*LorR+1]=100+338*LorR;//显示左边列谐波数据的X为82,右边列谐波数据X为438
                ListC108[7*i+91*LorR+2]=77+28*i;//Y坐标
                ListC108[7*i+91*LorR+3]=0xffff;
                ListC108[7*i+91*LorR+4]=0x0000;
                ListC108[7*i+91*LorR+5]=((U16)(PowRxchar[IndexInit+8*i+LorR*4])<<8)+((U16)PowRxchar[IndexInit+8*i+LorR*4+1]);
                ListC108[7*i+91*LorR+6]=((U16)(PowRxchar[IndexInit+8*i+LorR*4+2])<<8)+((U16)PowRxchar[IndexInit+8*i+LorR*4+3]);;
                //delay_ms(1);
            }
        }
    }
    else//显示含有率
    {
        YADA_98(330,41,0x22,0x81,0x02,0xffff,0x0000,HarmoInfo[4],0); // wk @130409 -->显示含有效
        ChartoFloat(&PowRxchar[(HarmoListPhase-1)*600+(HarmoListUorI-1)*200+Harmo_INDEX], Listfloat,50,1000);
        UIValues2HR(Listfloat,HRU16);//转换50次的值，若每次转换26次，则需记录第一次的值用于算25-50次的含有率
        YADA_40(0xffe0,0x0000);/*黄色前景，黑色背景*/
        for(U8 LorR=0; LorR<2; LorR++)//先左边列后右边列
        {
            for(U8 i=0; i<13; i++)
            {
                ListC108[7*i+91*LorR]=0x3304;//显示数据的模式，注意从显示4位小数切换到2位小数时，必须刷新页面
                ListC108[7*i+91*LorR+1]=100+338*LorR;//显示左边列谐波数据的X为82,右边列谐波数据X为438
                ListC108[7*i+91*LorR+2]=77+28*i;//Y坐标
                ListC108[7*i+91*LorR+3]=0xffff;
                ListC108[7*i+91*LorR+4]=0x0000;
                ListC108[7*i+91*LorR+5]=*(HRU16+i*4+LorR*2+(HarmoListRange-1)*48);
                ListC108[7*i+91*LorR+6]=*(HRU16+i*4+LorR*2+(HarmoListRange-1)*48+1);
                //delay_ms(1);
            }
        }
    }
#endif
    
    U16 HRU16[100]= {0},temp,temp1;
    U16 ListC108[182]= {0},IndexInit;
    U8 LIST_DIS[104]= {0};
    float Listfloat[50]= {0};
    U8 HarmoInfoZone[][10]= {"第1-26次","第25-50次","幅 值","含有率","U","I"};
    
    YADA_98(58,42,0x22,0x81,0x02,0xffe0,0x0000,HarmoInfo[HarmoListPhase-1],0);//显示相信息A、B、C.
    delay_us(5);
    YADA_98(205,42,0x22,0x81,0x02,0xffe0,0x0000,HarmoInfoZone[3+HarmoListUorI],1);
    delay_us(5);
    YADA_98(496,41,0x22,0x81,0x02,0xffe0,0x0000,HarmoInfoZone[HarmoListRange-1],0);
    delay_us(5);
    YADA_98(330,41,0x22,0x81,0x02,0xffe0,0x0000,HarmoInfoZone[1+HarmoListAmporRatio],6);
    if(HarmoListAmporRatio==1)//显示幅值时不需要转换
    {
        IndexInit=(HarmoListPhase-1)*600+(HarmoListUorI-1)*200+(HarmoListRange-1)*96+Harmo_INDEX;
        if(HarmoListUorI==1)
        {
            Sig_Fiq(&PowRxchar[IndexInit],LIST_DIS,100,26);
        }
        else
        {
            Sig_Fiq(&PowRxchar[IndexInit],LIST_DIS,10,26);
        }
        for(U8 LorR=0; LorR<2; LorR++) //先左边列后右边列
        {
            for(U8 i=0; i<13; i++)
            {
                temp=7*i+91*LorR;
                temp1=8*i+LorR*4;
                if(HarmoListUorI==1)
                {
                    ListC108[temp]=0x3204;//显示数据的模式
                }
                else
                {
                    ListC108[temp]=0x3304;//显示数据的模式
                }
                ListC108[temp+1]=100+338*LorR;//显示左边列谐波数据的X为82,右边列谐波数据X为438
                ListC108[temp+2]=77+28*i;//Y坐标
                ListC108[temp+3]=C108FC_W;
                ListC108[temp+4]=0x0000;
                ListC108[temp+5]=((U16)(LIST_DIS[temp1])<<8)+((U16)LIST_DIS[temp1+1]);
                ListC108[temp+6]=((U16)(LIST_DIS[temp1+2])<<8)+((U16)LIST_DIS[temp1+3]);;
            }
        }
    }
    else//显示含有率
    {
        ChartoFloat(&PowRxchar[(HarmoListPhase-1)*600+(HarmoListUorI-1)*200+Harmo_INDEX], Listfloat,50,100);
        UIValues2HR(Listfloat,HRU16);//转换50次的值，若每次转换26次，则需记录第一次的值用于算25-50次的含有率
        YADA_40(0xffe0,0x0000);/*黄色前景，黑色背景*/
        for(U8 LorR=0; LorR<2; LorR++)//先左边列后右边列
        {
            for(U8 i=0; i<13; i++)
            {
                temp=7*i+91*LorR;
                ListC108[temp]=0x4204;//显示数据的模式，注意从显示4位小数切换到2位小数时，必须刷新页面
                ListC108[temp+1]=100+338*LorR;//显示左边列谐波数据的X为82,右边列谐波数据X为438
                ListC108[temp+2]=77+28*i;//Y坐标
                ListC108[temp+3]=C108FC_W;
                ListC108[temp+4]=0x0000;
                ListC108[temp+5]=*(HRU16+i*4+LorR*2+(HarmoListRange-1)*48);
                ListC108[temp+6]=*(HRU16+i*4+LorR*2+(HarmoListRange-1)*48+1);
            }
        }
    }
    
    delay_ms(5);
    YADA_C0(HarmoListInfoAdr,ListC108,91);
    YADA_C108(HarmoListInfoAdr,13);
    delay_ms(5);
    YADA_C0(HarmoListInfoAdr+182,&ListC108[91],91);
    YADA_C108(HarmoListInfoAdr+182,13);
    delay_ms(5);
}

/*******************************************************************************
* 函  数  名      : linemark
* 描      述      : 压电流标注函数。显示通过U_I判断电压\电流有效值
* 输      入      : Y_COORD为Y坐标；U_I=0，则表示标注电压，U_I=1，则表示标注电流，
* 返      回      : 无
*******************************************************************************/
void linemark(U16 Y_COORD, U16 UorI)
{
#if 0  // wk @130405-->修改显示位数之前
    U16 C108Dat[21]= {0};
    U8 UORI[][2]= {"V","A"};
    for(U8 k=0; k<3; k++)
    {
        C108Dat[7*k + 0] = 0x3403;//P  显示数据的模式
        C108Dat[7*k + 1] = 118+200*k;//显示左边列谐波数据的X坐标118 318,518
        C108Dat[7*k + 2] = Y_COORD;//Y坐标
        C108Dat[7*k + 3] = COLOR[k];
        C108Dat[7*k + 4] = 0x0000;
        /* wk --> 显示的出来的数据 = C108[5]* 65536 + C108[6] */
        C108Dat[7*k + 5] = ((U16)(PowRxchar[UI_VIRTUAL_INDEX + 16*k + UorI*8]) << 8) + (U16)(PowRxchar[UI_VIRTUAL_INDEX + 1 + 16*k + UorI*8]);//两个字节转为一个字
        C108Dat[7*k + 6] = ((U16)(PowRxchar[UI_VIRTUAL_INDEX + 2 + 16*k + UorI*8]) << 8) + (U16)(PowRxchar[UI_VIRTUAL_INDEX + 3 + 16*k + UorI*8]);
        YADA_98(198+200*k, Y_COORD, 0x22, 0x81, 0x01, COLOR[k], 0x0000, UORI[UorI], 0);  //标注电压或电流的单位
    }
    delay_ms(5);
    YADA_C0(WAVEUIaddr + UorI*42, C108Dat, 21);  
    YADA_C108(WAVEUIaddr + UorI*42, 3);   //写入有效值，每次3个
#endif
    
    U16 C108Dat[21]= {0};
    U8 VI_DIS[12]={0};
    U8 UORI[][2]= {"V","A"},temp,temp1,k;
    for(k=0; k<3; k++)
    {
      if(UorI==0)
         // wk @130408--> revrese dot tx
//        Sig_Fiq(&PowRxchar[UI_VIRTUAL_INDEX+8+16*k],&VI_DIS[4*k],10);   //数据显示为要求的有效数字
      {
//      PowRxchar[0]=0x00;PowRxchar[1]=0x21;PowRxchar[2]=0x01;PowRxchar[3]=0x93;
      Sig_Fiq(PowRxchar,VI_DIS,100,3);
      }
      else
         // wk @130408--> revrese dot tx
//        Sig_Fiq(&PowRxchar[UI_VIRTUAL_INDEX+16*k],&VI_DIS[4*k],100); 
       Sig_Fiq(&PowRxchar[I_VIRTUAL_INDEX],VI_DIS,10,3);
    }
    for(k=0; k<3; k++)
    {
      temp=7*k;
      temp1=k*4;
      if(UorI)
      {
        C108Dat[temp + 0] = 0x3303;//P
      }
      else
      {
         C108Dat[temp + 0] = 0x3203;//P  
      }
        C108Dat[temp + 1] = 118+200*k;//显示左边列谐波数据的X坐标118 318,518
        C108Dat[temp + 2] = Y_COORD;//Y坐标
        C108Dat[temp + 3] = COLOR[k];
        C108Dat[temp + 4] = 0x0000;
        C108Dat[temp + 5] = ((U16)(VI_DIS[temp1]) << 8) + (U16)(VI_DIS[1 + temp1]);
        C108Dat[temp + 6] = ((U16)(VI_DIS[temp1+2]) << 8) + (U16)(VI_DIS[3 + temp1]);
        YADA_98(198+200*k, Y_COORD, 0x22, 0x81, 0x01, COLOR[k], 0x0000, UORI[UorI], 0);  //标注电压或电流的单位
    }
    
    delay_ms(5);
    YADA_C0(WAVEUIaddr + UorI*42, C108Dat, 21);
    YADA_C108(WAVEUIaddr + UorI*42, 3);   //写入有效值，每次3个
}
/*******************************************************************************
* 函  数  名      : GUI_SYS_PARASET
* 描      述      : 系统参数设定。可通过移位键选择各项设置。数值输入的范围为0~99999，大于5位时清零
*                   此页的时间设置没有处理输入是否在正确范围
* 输      入      : 无
* 返      回      : 无
* 
* 修       改     : WK
* 时       间     : 2013-03-13
* 描       述     : 基于2013-03-08界面
*******************************************************************************/
void GUI_SYS_PARASET(void)
{
    U8 OFF_ON[][4]= {"关闭","开启"},i,temp=0,temp1=0;
    U16 ParaSetC108[63]= {0},PARA_y=0,string2U16=0;
    static U8 flg_sys[9]={0} ;// wk --设置参数超出了限制标志
    
     SHELL_CONTEXT_PTR    shell_ptr;
     shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
     _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
    //U32 String2U32=0;
    U16 SysParaXY[18]= { 450, 50, 450, 80 , 450 ,112,
                         377,146, 470,146 , 540,146,   // 年是4位显示的，因此X坐标小于时间（2位）的X坐标
                         400,178, 470,178,  540,178
                       };
    U16 SysParaSetSq[36]= {445,48,448,73,  445,80,448,105, 445,110,448,135, 
                           370,145,373,170, 460,145,463,170,  530,145,533,170, 
                           370,176,373,201, 460,176,463,201,  530,176,533,201
                           };
    /* wk --> 光标 */
    if(SysSet.SwFlg) //初次进入时读取flash
    {
        YADA_40(0x0000,0xfc00);//前景色为黑色，背景色为橙色
        YADA_5A(SysParaSetSq,4);  // WK --> 以背景色填充（148,265）、（148,296）的矩形
#if 0 // wk --> 全部读取Flash中参数      
        shell_ptr->ARGC = 2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="f:\\"; 
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC = 2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="sysset";
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC=1;
        shell_ptr->ARGV[0]="pwd";
        Shell_pwd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC=5;
        shell_ptr->ARGV[0]="read";
        shell_ptr->ARGV[1]="sysset.txt";
        shell_ptr->ARGV[2]="84";
        shell_ptr->ARGV[3]="begin";
        shell_ptr->ARGV[4]="0";
        Shell_read_wk(shell_ptr->ARGC, shell_ptr->ARGV,SysFlashData);   
#endif // wk --> 全部读取Flash中参数
        
#if 1 // wk --> 只读取Flash中系统参数      
        shell_ptr->ARGC = 2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="f:\\"; 
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC = 2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="sysset";
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC=1;
        shell_ptr->ARGV[0]="pwd";
        Shell_pwd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC=5;
        shell_ptr->ARGV[0]="read";
        shell_ptr->ARGV[1]="sysset.txt";
        shell_ptr->ARGV[2]="25";
        shell_ptr->ARGV[3]="begin";
        shell_ptr->ARGV[4]="0";
        Shell_read_wk(shell_ptr->ARGC, shell_ptr->ARGV,SysFlashDataT);   
#endif //  wk --> 只读取Flash中系统参数
        
        //DISTIME(0);//显示当前的时间
       // memcpy(SysFlashData,ParaSetC108,48);//从内部information memory中读取存储的参数
        SysSet.SwFlg=0;
        
        // wk --> 写入有效值  第一次全部写入
        for(i=0;i<9;i++)
        {
          temp=2*i;
          temp1=7*i;
          if(i<3)
          {
            ParaSetC108[temp1 + 0] = 0x5004;            //显示相位的X坐标
          }
          else if(i==3)
          {
            ParaSetC108[temp1 + 0] = 0x4004;        //显示相位的X坐标  
          }
          else
          {
            ParaSetC108[temp1 + 0] = 0x2004;        //显示相位的X坐标  
          }
          ParaSetC108[temp1 + 1] = SysParaXY[temp];            //显示相位的X坐标
          ParaSetC108[temp1 + 2] = SysParaXY[temp + 1];     //Y坐标
          ParaSetC108[temp1 + 3] = 0xffff;         //白色，下为黑色
          ParaSetC108[temp1 + 4] = 0x0000;
          ParaSetC108[temp1 + 5] = 0;         //白色，下为黑色
          ParaSetC108[temp1 + 6] = (U16) (SysFlashDataT[temp+8]<<8)+(U16)(SysFlashDataT[temp+7]);
        }
               
        YADA_C0(ParaSetAddr, ParaSetC108, 63);  // WK --> 写暂存缓冲区
        delay_us(10);
        YADA_C108(ParaSetAddr, 9);   //写入有效值，每次10个
        delay_us(10);   
        
       for(i=0;i<7;i++)  // WK --> 向屏幕 7 个控制按钮写 “开 ”、“关 ”状态
       {
          PARA_y=63+i*37;
          /* WK --> 清除上一次的状态 */
          YADA_71(MenuParaSET,148, PARA_y,240,PARA_y+24, 148, PARA_y);  //WK --> 剪切图标显示
          YADA_98(152, PARA_y, 0x22, 0x81, 0x02, 0xffff, 0x0000, OFF_ON[SysFlashDataT[i]], 4);// WK -->字库选择
          delay_us(10);
       }
    }
    
    if(SysSet.FuncFlg) //wk --> 上移 ，下移
    {
        //YADA_5B(&SysParaSetSq[SysParaOldIndex*4],4);  //擦除前一处
        YADA_5B(&SysParaSetSq[SysParaOldIndex*4],4);   // 擦除前一处
        SysParaOldIndex=SysSet.ParaIndex;
        SysSet.FuncFlg=0;
        SysSet.DataCnt=0; //切换时键盘输入清零
        YADA_5A(&SysParaSetSq[SysSet.ParaIndex*4],4);      //背景色填充相所在的矩形区域/**/
        //YADA_5A(&SysParaSetSq[SysSet.ParaIndex*4],4);    //背景色填充相所在的矩形区域/**/
    }
    
    if(SysSet.DataFlg)//清除时SysSet.DataFlg为1，但DatNumFlg为0
    {
        if(SysSet.DataCnt==0)
        {
            string2U16=0;
        }
        else
        for(i=0; i<SysSet.DataCnt; i++) //利用输入数据个数
        {
                string2U16=string2U16*10+SysSet.Data[i]-'0';    //求整数部分
        }
        
             temp=(SysSet.ParaIndex)*2; //WK --> 
             SysFlashDataT[temp+7]=(U8)(string2U16);
             SysFlashDataT[temp+8]=(U8)(string2U16>>8);
             /*　WK --> 重新显示修改过的数据 */
            U16 OneC108[7]= {SysSet.ParaIndex<3?0x5004:(SysSet.ParaIndex==3?0x4004:0x2004),
                              SysParaXY[temp],SysParaXY[temp+1],0xffff,0x0000}; //0x0000 --> 黑
            OneC108[5] =0;
            OneC108[6] =(U16) (SysFlashDataT[temp+8]<<8)+(U16)(SysFlashDataT[temp+7]);
            
            switch(SysSet.ParaIndex) // WK --> 阈值判断
            {
            case 0:
               if(OneC108[6]>200)
              { 
                OneC108[3]=0xf800;
                flg_sys[0]=1; // 统计时间超限标志
              }
               break;
            case 1:
               if(OneC108[6]>200)
              { 
                OneC108[3]=0xf800;
                flg_sys[1]=1; // 存数时间超限标志
              }
               break;
            case 2:
              if(OneC108[6]>200)
              { 
                OneC108[3]=0xf800;
                flg_sys[2]=1; //上传时间超限标志
              }
               break;
            case 3:  // year
              if(OneC108[6]>2099)  
              {
                OneC108[3]=0xf800;
                flg_sys[3]=1; // 年上超限标志
              }
              else if(OneC108[6]<1970)
              {
                OneC108[3]=0xf800;
                flg_sys[3]=2; // 年下超限标志
              }
              else
                flg_sys[3]=3; // 年没有超限
                
               break;
            case 4:
              if(OneC108[6]>12)  
              {
                OneC108[3]=0xf800;
                flg_sys[4]=1;// 月上超限标志
              }
              else if(OneC108[6]==0)
              {
                OneC108[3]=0xf800;
                flg_sys[4]=2;// 月下超限标志
              }
               break;
            case 5:
              if(OneC108[6]>30) 
              {
                OneC108[3]=0xf800;
                flg_sys[5] =1; // 日上超限标志
              }
              else if(OneC108[6]==0)
              {
                OneC108[3]=0xf800;
                flg_sys[5]=2;// 日下超限标志
              }
               break;
            case 6:
               if(OneC108[6]>24)
               { OneC108[3]=0xf800;
                 flg_sys[6]=1; //小时超限标志
               }
               break;
            case 7:
              if(OneC108[6]>60)
              { OneC108[3]=0xf800;
                flg_sys[7]=1; // 分超限标志
              }
              break;
            case 8:
              if(OneC108[6]>60)
              { OneC108[3]=0xf800;
                flg_sys[8]=1; // 秒超限标志
              }
              break;
            default:
              break;
            }
            YADA_C0(ParaSetAddr+SysSet.ParaIndex*7, OneC108, 7);//修改发生改变的数据项
            YADA_C108(ParaSetAddr+SysSet.ParaIndex*7, 1);
             
            SysSet.DataFlg=0;
      
    }
    if(SysSet.SwitchSet[7]==1)
    {
      for(i=0;i<7;i++)  // WK --> 向屏幕 7 个控制按钮写 “开 ”、“关 ”状态
      {
            PARA_y=63+i*37;
            SysFlashDataT[i]=SysSet.SwitchSet[i];
            /* WK --> 清除上一次的状态 */
            YADA_71(MenuParaSET,148, PARA_y,240,PARA_y+24, 148, PARA_y);  //WK --> 剪切图标显示
            YADA_98(152, PARA_y, 0x22, 0x81, 0x02, 0xffff, 0x0000, OFF_ON[SysFlashDataT[i]], 4);// WK -->字库选择
            delay_us(10);
      }
      SysSet.SwitchSet[7]=0;  // wk --> 清楚标志
    }
    
    /* WK --> 保存键 */
    if(SysSet.ParaSaveFlg)
    {    
      /* pwd*/
//     shell_ptr->ARGC=1;
//     shell_ptr->ARGV[0]="pwd";
//     Shell_pwd(shell_ptr->ARGC, shell_ptr->ARGV);
      for(int i=0;i<9;i++) // wk @130326 --> 对超出阈值的进行处理
      {
        if((flg_sys[i]==1) || (flg_sys[i]==2))
          switch(i)
          {
          case 0:
          case 1:
          case 2:
            SysFlashDataT[2*i+7]=200;
            SysFlashDataT[2*i+8]=0;
            flg_sys[i]=0;
           break;
          case 3: // 年
            if(flg_sys[i]==1)
            {
              SysFlashDataT[2*i+7]=0x33;  
              SysFlashDataT[2*i+8]=0x08;
            }
            else  if(flg_sys[i]==3)
            {
              ;
            }
            else if(flg_sys[i]==2)
            {
              SysFlashDataT[2*i+7]=0xb2;  
              SysFlashDataT[2*i+8]=0x07;
            }
            flg_sys[i]=0;
           break;
          case 4: //月
            if(flg_sys[i]==1)
            {
              SysFlashDataT[2*i+7]=12;
            }
            else
            {
              SysFlashDataT[2*i+7]=1;  
            } 
            SysFlashDataT[2*i+8]=0;
            flg_sys[i]=0;
            break;
          case 5: // 日
            if(flg_sys[i]==1)
              SysFlashDataT[2*i+7]=60;
            else
              SysFlashDataT[2*i+7]=1;
            SysFlashDataT[2*i+8]=0;
            flg_sys[i]=0;
            break;
          case 6: // 时
            SysFlashDataT[2*i+7]=24;
            SysFlashDataT[2*i+8]=0;
            flg_sys[i]=0;
            break;
          case 7: // 分
            SysFlashDataT[2*i+7]=60;
            SysFlashDataT[2*i+8]=0;
            flg_sys[i]=0;
            break;
          case 8: // 秒
            SysFlashDataT[2*i+7]=60;
            SysFlashDataT[2*i+8]=0;
            flg_sys[i]=0;
            break;
          }
      }
      
#if 0 //WK -->保存时  SysFlashData 全部保存 
    for(int i=0;i<84;i++) 
    {
      SysFlashSave[i]=SysFlashDataT[i];
    }
    shell_ptr->ARGC=2;
    shell_ptr->ARGV[0]="cd";
    shell_ptr->ARGV[1]="f:\\"; 
    Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
    
    shell_ptr->ARGC = 2;
    shell_ptr->ARGV[0]="cd";
    shell_ptr->ARGV[1]="sysset";
    Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
    
    shell_ptr->ARGC=4;
    shell_ptr->ARGV[0]="w";
    shell_ptr->ARGV[1]="sysset.txt";
    shell_ptr->ARGV[2]="begin";
    shell_ptr->ARGV[3]="0";
    Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,84,SysFlashDataT);
    
    shell_ptr->ARGC=2;
    shell_ptr->ARGV[0]="update"; // wk --> update
    shell_ptr->ARGV[1]="flush";
//    Shell_update(shell_ptr->ARGC, shell_ptr->ARGV,84,SysFlashSave);
    Shell_update(shell_ptr->ARGC, shell_ptr->ARGV);
#endif  // WK -->保存时  SysFlashData 全部保存 END
   
#if 1 // wk --> 只保存系统设置参数界面的参数   1-25
    for(int i=0;i<25;i++) 
    {
      SysFlashData[i]=SysFlashDataT[i];
    }
    shell_ptr->ARGC=2;
    shell_ptr->ARGV[0]="cd";
    shell_ptr->ARGV[1]="f:\\"; 
    Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
    
    shell_ptr->ARGC = 2;
    shell_ptr->ARGV[0]="cd";
    shell_ptr->ARGV[1]="sysset";
    Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
    
    shell_ptr->ARGC=4;
    shell_ptr->ARGV[0]="w";
    shell_ptr->ARGV[1]="sysset.txt";
    shell_ptr->ARGV[2]="begin";
    shell_ptr->ARGV[3]="0";
    Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,25,SysFlashData); 
    
    shell_ptr->ARGC=2;
    shell_ptr->ARGV[0]="update";// wk --> update
    shell_ptr->ARGV[1]="flush";
//    Shell_update(shell_ptr->ARGC, shell_ptr->ARGV,25,SysFlashSave);
    Shell_update(shell_ptr->ARGC, shell_ptr->ARGV);
#endif // wk --> 只保存系统设置参数界面的参数   1-25
    
    /*WK --> 保存成功标志 */
    for(i=0;i<9;i++)
        {
          temp=2*i;
          temp1=7*i;
          if(i<3)
          {
            ParaSetC108[temp1 + 0] = 0x5004;            //显示相位的X坐标
          }
          else if(i==3)
          {
            ParaSetC108[temp1 + 0] = 0x4004;        //显示相位的X坐标  
          }
          else
          {
            ParaSetC108[temp1 + 0] = 0x2004;        //显示相位的X坐标  
          }
          ParaSetC108[temp1 + 1] = SysParaXY[temp];            //显示相位的X坐标
          ParaSetC108[temp1 + 2] = SysParaXY[temp + 1];     //Y坐标
          ParaSetC108[temp1 + 3] = 0xffc1;         //黄色，下为黑色
          ParaSetC108[temp1 + 4] = 0x0000;
          ParaSetC108[temp1 + 5] = 0;         //白色，下为黑色
          ParaSetC108[temp1 + 6] = (U16) (SysFlashDataT[temp+8]<<8)+(U16)(SysFlashDataT[temp+7]);
        }
        YADA_C0(ParaSetAddr, ParaSetC108, 63);  // WK --> 写暂存缓冲区
        delay_us(10);
        YADA_C108(ParaSetAddr, 9);   //写入有效值，每次10个
        
        SysSet.ParaSaveFlg=0;  // WK --> 清楚标志
        
        TimeSet(); // wk@130510 --> 重新设置时间
        delay_us(10); 
        file_namePS[0]='w'; // wk @130513 --> 每次设置时间后，基本电能质量数据都要从新建立新的文件
        
        _mem_free(shell_ptr); 
    }
    else
       _mem_free(shell_ptr); 
}
/*******************************************************************************
* 函  数  名      : GUI_SYS_EVENTSET
* 描      述      : 事件限值设定。可通过移位键选择各项设置
* 输      入      : 无
* 返      回      : 无
*
* 修       改     : WK
* 时       间     : 2013-03-13
* 描       述     : 基于2013-03-08界面
*******************************************************************************/

void GUI_SYS_EVENTSET(void)
{
    float String2F=0.0;
    U32 Float2L=0;
    U8 k,temp=0;
    U16 ParaSetC108[98]= {0};
    static U8 flg_event[11]={0}; // wk @130326 --> 事件设置参数超限标志
    U16 NumWave;
    U16 DotWave;
    
    SHELL_CONTEXT_PTR    shell_ptr;
    shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
    _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
    
    U16 SysEventXY[22]= {
                          155,97, 155,133, 155,169, 155,205, 155,241, 155,278, 155,315,  /*WK --> 第1列数据坐标 */
                          481,97, 481,133, 481,169, 481,205 /* WK -->第2列数据坐标 */
                        }; 
    U16 SysEventSetSq[44]= {
                              /* WK -->第1列光标坐标 */
                              150,96,153,122, 150,132,153,158, 150,168,153,194, 150,204,153,230, 150,240,153,266, 150,276,153,304, 150,314,153,340,
                              /* WK -->第2列光标坐标 */
                              476,96,479,122, 476,132,479,158, 476,168,479,194, 476,204,479,230
                           }; //相、UI选择\区间所在的黑色矩形框
    if(SysSet.SwFlg)  // wk @130326 --> 第一次进入时，会读取Flash中保存数据
    {
        /* WK --> 光标 */
        YADA_40(0x0000,0xfc00);//前景色为黑色，背景色为橙色 set only once
        YADA_5A(SysEventSetSq,4);
        //memcpy(&SysFlashData[EVESET_INDEX],SysEventAddr,48);
#if 0 // wk --> 全部读取Flash中参数      
        shell_ptr->ARGC = 2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="f:\\"; 
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC = 2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="sysset";
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC=1;
        shell_ptr->ARGV[0]="pwd";
        Shell_pwd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC=5;
        shell_ptr->ARGV[0]="read";
        shell_ptr->ARGV[1]="sysevent.txt";
        shell_ptr->ARGV[2]="84";
        shell_ptr->ARGV[3]="begin";
        shell_ptr->ARGV[4]="0";
        Shell_read_wk(shell_ptr->ARGC, shell_ptr->ARGV,SysFlashData);   
#endif // wk --> 全部读取Flash中参数     

#if 1 // wk @130326 --> 只保存事件设置参数
        shell_ptr->ARGC = 2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="f:\\"; 
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC = 2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="sysset";
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC=1;
        shell_ptr->ARGV[0]="pwd";
        Shell_pwd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC=5;
        shell_ptr->ARGV[0]="read";
        shell_ptr->ARGV[1]="sysevent.txt";
        shell_ptr->ARGV[2]="45";
        shell_ptr->ARGV[3]="begin";
        shell_ptr->ARGV[4]="0";   // WK @130326  --> 事件设置参数偏移26保存
        Shell_read_wk(shell_ptr->ARGC, shell_ptr->ARGV,&(SysFlashDataT[25]));  
#endif      
        if(SysFlashDataT[EVESEND_FLAG]==1)
          SysSet.EventSendFlg=1; // 防止上一次掉电时数据没有发送完成，下次上电接着发送
        
        SysSet.SwFlg=0;
        
        for(k=0; k<11; k++)//在第一次时全部显示，以后每次数据更改时只修改相应的项
        {
            temp=7*k;               // old --> 0x5204
            ParaSetC108[temp + 0] = 0x3204;         //P  显示数据的模式
            ParaSetC108[temp + 1] = SysEventXY[2*k];            //显示相位的X坐标
            ParaSetC108[temp + 2] = SysEventXY[2*k + 1];     //Y坐标
            ParaSetC108[temp + 3] = 0xffff;         //白色，下为黑色
            ParaSetC108[temp + 4] = 0x0000;
            ParaSetC108[temp + 5] =(U16) (SysFlashDataT[3+4*k+EVESET_INDEX]<<8)+(U16)(SysFlashDataT[2+4*k+EVESET_INDEX]);
            ParaSetC108[temp + 6] =(U16) (SysFlashDataT[1+4*k+EVESET_INDEX]<<8)+(U16)(SysFlashDataT[4*k+EVESET_INDEX]);
        }
        YADA_C0(EventSetAddr, ParaSetC108, 11*7);
        YADA_C108(EventSetAddr, 11);   //写入有效值，每次10个
    }
    
    if(SysSet.FuncFlg)//右移、左移、T，修改光标
    {
        YADA_5B(&SysEventSetSq[SysEventOldIndex*4],4);//擦除前一处
        YADA_5A(&SysEventSetSq[SysSet.EvntIndex*4],4);    //背景色填充相所在的矩形区域
        SysEventOldIndex=SysSet.EvntIndex;
        SysSet.FuncFlg=0;
        SysSet.DataCnt=0;//切换时键盘输入清零 so you don't need to clear the SysSet.Data Array
    }
    
    if(SysSet.DataFlg)//修改一项数据
    {
      /* wk @130514 --> 优化2 */
//      if(SysSet.DataFlg)
//      {
//        SysFlashDataT[EVESEND_FLAG]=0;
//      }
      /* wk @130514 --> end */
      if(SysSet.DataCnt==0)//clear key
        {
            String2F=0;
        }
        else
        {
            U8 i;
            float k;
            //把字符转化为浮点数
            for( i=0; i<SysSet.DataCnt&&SysSet.Data[i]!='.'; i++) //利用输入数据个数
            {
                String2F=String2F*10+SysSet.Data[i]-'0';    //求整数部分
            }
            if(SysSet.Data[i]=='.')
                for(i++,k=0.1; i<SysSet.DataCnt&&SysSet.Data[i]!='.'; i++,k=k/10)
                {
                    String2F=String2F+(SysSet.Data[i]-'0')*k;
                }
        }
          
        Float2L=(long)(String2F*100);//enlarge 100 times
        temp=SysSet.EvntIndex*4;
        SysFlashDataT[temp+EVESET_INDEX]=(U8)(Float2L);  //四字节数
        SysFlashDataT[temp+1+EVESET_INDEX]=(U8)(Float2L>>8);
        SysFlashDataT[temp+2+EVESET_INDEX]=(U8)(Float2L>>16);
        SysFlashDataT[temp+3+EVESET_INDEX]=(U8)(Float2L>>24);
        
        U16 OneC108[7]= {0x3204,SysEventXY[2*SysSet.EvntIndex],SysEventXY[2*SysSet.EvntIndex + 1],0xffff,0x0000};
        /* wk @130514--> 优化2*/
//        if(SysFlashDataT[EVESEND_FLAG])
//        {
//          OneC108[3]=0xffe0;
//        }
        /* wk @130514--> end*/
        OneC108[5] =(U16) (SysFlashDataT[3+temp+EVESET_INDEX]<<8)+(U16)(SysFlashDataT[2+temp+EVESET_INDEX]);
        OneC108[6] =(U16) (SysFlashDataT[1+temp+EVESET_INDEX]<<8)+(U16)(SysFlashDataT[temp+EVESET_INDEX]);
        
        /* WK --> 阈值判断 */
        switch(SysSet.EvntIndex)
        {
        case 0:
          if(((OneC108[6]+((U32)OneC108[5]<<16))!=400)|| ((OneC108[6]+((U32)OneC108[5]<<16))!=600)||
                    ((OneC108[6]+((U32)OneC108[5]<<16))!=800)|| ((OneC108[6]+((U32)OneC108[5]<<16))!=1000)||
                    ((OneC108[6]+((U32)OneC108[5]<<16))!=1200))
           { 
            OneC108[3]=0xf800;
            flg_event[0]=1;
           }
            break;
        case 1:
          if(((OneC108[6]+(OneC108[5]<<16))!=6400)|| ((OneC108[6]+(OneC108[5]<<16))!=12800)|| 
             ((OneC108[6]+(OneC108[5]<<16))!=25600))
          {
            OneC108[3]=0xf800;
            flg_event[1]=1;
          }
            break;
        case 2:
           if((OneC108[6]+(OneC108[5]<<16))>UDeviation)
           {
            OneC108[3]=0xf800;
            flg_event[2]=1;
           }
            break;
        case 3:
           if((OneC108[6]+(OneC108[5]<<16))>FDeviation)
           {
            OneC108[3]=0xf800;
            flg_event[3]=1;
           }
            break;
        case 4:
          if((OneC108[6]+(OneC108[5]<<16))>USurge)
          {
            OneC108[3]=0xf800;
            flg_event[4]=1;          
          }
          break;
        case 5:
          if((OneC108[6]+(OneC108[5]<<16))>NUnblance)
          {
            OneC108[3]=0xf800;
            flg_event[5]=1;          
          }
          break;
        case 6:
          if((OneC108[6]+(OneC108[5]<<16))>LngFlick)
          {
            OneC108[3]=0xf800;
            flg_event[6]=1;          
          }
          break;
        case 7:
          if((OneC108[6]+(OneC108[5]<<16))>TotalHarmonic)
          {
            OneC108[3]=0xf800;
            flg_event[7]=1;          
          }
          break;
        case 8:  //WK @130326 -->间谐波暂时没有做
          break;
        case 9:
          if((OneC108[6]+(OneC108[5]<<16))>UHarmonic)
          {
            OneC108[3]=0xf800;
            flg_event[9]=1;          
          }
          break;
        case 10:
          if((OneC108[6]+(OneC108[5]<<16))>IHarmonic)
          {
            OneC108[3]=0xf800;
            flg_event[10]=1;          
          }
          break;      
        default:
          break;
        }
        YADA_C0(EventSetAddr+SysSet.EvntIndex*7, OneC108, 7);//修改发生改变的数据项
        YADA_C108(EventSetAddr+SysSet.EvntIndex*7, 1);
        SysSet.DataFlg=0;
    }
    
    if(SysSet.EventSaveFlg)//将参数值发给DSP
    {
     for(int i=0;i<11;i++)
      if(flg_event[i]==1)
        switch(i)
        {
          case 0:
            if( (SysFlashDataT[4*i+EVESET_INDEX+0]+((U32)SysFlashDataT[4*i+EVESET_INDEX+1]<<8)+((U32)SysFlashDataT[4*i+EVESET_INDEX+2]<<16))<=400)
            {
                NumWave=400;
            }
            else if((SysFlashDataT[4*i+EVESET_INDEX]+((U32)SysFlashDataT[4*i+EVESET_INDEX+1]<<8)+((U32)SysFlashDataT[4*i+EVESET_INDEX+2]<<16))<=600)
            {
                NumWave=600;
            }
            else if((SysFlashDataT[4*i+EVESET_INDEX]+((U32)SysFlashDataT[4*i+EVESET_INDEX+1]<<8)+((U32)SysFlashDataT[4*i+EVESET_INDEX+2]<<16))<=800)
            {
                NumWave=800;
            }
            else if((SysFlashDataT[4*i+EVESET_INDEX]+((U32)SysFlashDataT[4*i+EVESET_INDEX+1]<<8)+((U32)SysFlashDataT[4*i+EVESET_INDEX+2]<<16))<=1000)
            {
                NumWave=1000;
            }
            else
            {
                NumWave=1200;
            }
           SysFlashDataT[4*i+25]=(U8)(NumWave)%256;
           SysFlashDataT[4*i+26]=(U8)((NumWave)>>8)%256;
           SysFlashDataT[4*i+27]=(U8)((NumWave)>>16)%256;
           SysFlashDataT[4*i+28]=(U8)((NumWave)>>24)%256;
           flg_event[i]=0;
           break;
         case 1:
           if( (SysFlashDataT[4*i+25]+(SysFlashDataT[4*i+26]<<8)+(SysFlashDataT[4*i+27]<<16))<=6400)
             DotWave=6400;
           else if((SysFlashDataT[4*i+25]+(SysFlashDataT[4*i+26]<<8)+(SysFlashDataT[4*i+27]<<16))<=12800)
             DotWave=12800;
           else
             DotWave=25600;
           SysFlashDataT[4*i+25]=(U8)(DotWave)%256;
           SysFlashDataT[4*i+26]=(U8)(DotWave>>8)%256;
           SysFlashDataT[4*i+27]=(U8)(DotWave>>16)%256;
           SysFlashDataT[4*i+28]=(U8)(DotWave>>24)%256;
           flg_event[i]=0;
           break;
        case 2:
           SysFlashDataT[4*i+25]=(U8)(UDeviation)%256;
           SysFlashDataT[4*i+26]=(U8)(UDeviation>>8)%256;
           SysFlashDataT[4*i+27]=(U8)(UDeviation>>16)%256;
           SysFlashDataT[4*i+28]=(U8)(UDeviation>>24)%256;
           flg_event[i]=0;
           break;
        case 3:
           SysFlashDataT[4*i+25]=(U8)(FDeviation)%256;
           SysFlashDataT[4*i+26]=(U8)(FDeviation>>8)%256;
           SysFlashDataT[4*i+27]=(U8)(FDeviation>>16)%256;
           SysFlashDataT[4*i+28]=(U8)(FDeviation>>24)%256;
           flg_event[i]=0;
           break;
        case 4:
           SysFlashDataT[4*i+25]=(U8)(USurge)%256;
           SysFlashDataT[4*i+26]=(U8)(USurge>>8)%256;
           SysFlashDataT[4*i+27]=(U8)(USurge>>16)%256;
           SysFlashDataT[4*i+28]=(U8)(USurge>>24)%256;
           flg_event[i]=0;
           break;
        case 5:
           SysFlashDataT[4*i+25]=(U8)(NUnblance)%256;
           SysFlashDataT[4*i+26]=(U8)(NUnblance>>8)%256;
           SysFlashDataT[4*i+27]=(U8)(NUnblance>>16)%256;
           SysFlashDataT[4*i+28]=(U8)(NUnblance>>24)%256;
           flg_event[i]=0;
           break;
        case 6:
           SysFlashDataT[4*i+25]=(U8)(LngFlick)%256;
           SysFlashDataT[4*i+26]=(U8)(LngFlick>>8)%256;
           SysFlashDataT[4*i+27]=(U8)(LngFlick>>16)%256;
           SysFlashDataT[4*i+28]=(U8)(LngFlick>>24)%256;
           flg_event[i]=0;
           break;
        case 7:
           SysFlashDataT[4*i+25]=(U8)(TotalHarmonic)%256;
           SysFlashDataT[4*i+26]=(U8)(TotalHarmonic>>8)%256;
           SysFlashDataT[4*i+27]=(U8)(TotalHarmonic>>16)%256;
           SysFlashDataT[4*i+28]=(U8)(TotalHarmonic>>24)%256;
           flg_event[i]=0;
        case 8: //间谐波没有做
          break;
        case 9:
           SysFlashDataT[4*i+25]=(U8)(UHarmonic)%256;
           SysFlashDataT[4*i+26]=(U8)(UHarmonic>>8)%256;
           SysFlashDataT[4*i+27]=(U8)(UHarmonic>>16)%256;
           SysFlashDataT[4*i+28]=(U8)(UHarmonic>>24)%256;
           flg_event[i]=0;
           break;
        case 10:
           SysFlashDataT[4*i+25]=(U8)(IHarmonic)%256;
           SysFlashDataT[4*i+26]=(U8)(IHarmonic>>8)%256;
           SysFlashDataT[4*i+27]=(U8)(IHarmonic>>16)%256;
           SysFlashDataT[4*i+28]=(U8)(IHarmonic>>24)%256;
           flg_event[i]=0;
           break;
          default:
            break;
        }
#if 0 //WK -->保存时  SysFlashData 全部保存     
    for(int i=0;i<84;i++) 
    {
      SysFlashSave[i]=SysFlashDataT[i];
    }
    shell_ptr->ARGC=2;
    shell_ptr->ARGV[0]="cd";
    shell_ptr->ARGV[1]="f:\\"; 
    Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
    
    shell_ptr->ARGC = 2;
    shell_ptr->ARGV[0]="cd";
    shell_ptr->ARGV[1]="sysset";
    Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
    
    shell_ptr->ARGC=4;
    shell_ptr->ARGV[0]="write";
    shell_ptr->ARGV[1]="sysevent.txt";
    shell_ptr->ARGV[2]="begin";
    shell_ptr->ARGV[3]="0";
    Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,84,SysFlashSave);
    
    // wk --> update
    shell_ptr->ARGC=2;
    shell_ptr->ARGV[0]="update";
    shell_ptr->ARGV[1]="flush";
//    Shell_update(shell_ptr->ARGC, shell_ptr->ARGV,84,SysFlashSave);  
    Shell_update(shell_ptr->ARGC, shell_ptr->ARGV); 
#endif  // WK -->保存时  SysFlashData 全部保存 END
 
#if 1  // wk @130326 --> 只保存事件界面数据 44个事件阈值数据 + 1个K602DSP发送状态标志
    for(int i=0;i<44;i++) 
    {
      SysFlashData[i+25]=SysFlashDataT[i+25];  
    }
    SysFlashData[EVESEND_FLAG]=SysSet.EventSendFlg;
      
    shell_ptr->ARGC=2;
    shell_ptr->ARGV[0]="cd";
    shell_ptr->ARGV[1]="f:\\"; 
    Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
    
    shell_ptr->ARGC = 2;
    shell_ptr->ARGV[0]="cd";
    shell_ptr->ARGV[1]="sysset";
    Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
    
    shell_ptr->ARGC=4;
    shell_ptr->ARGV[0]="w";
    shell_ptr->ARGV[1]="sysevent.txt";
    shell_ptr->ARGV[2]="begin";
    shell_ptr->ARGV[3]="0";  // WK @130326 --> 偏移 26  注意：偏移25时，初始上电时，波形个数为2.55，因此该到了偏移26
    Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,45,&(SysFlashData[25]));
    
    shell_ptr->ARGC=2;
    shell_ptr->ARGV[0]="update";// wk --> update
    shell_ptr->ARGV[1]="flush";
    Shell_update(shell_ptr->ARGC, shell_ptr->ARGV);
#endif
           
       for(uchar ij=4;ij<52;ij++) // wk@130509--> 将发送给DSP的数据赋值
//         SysDataSend[ij]= SysFlashData[ij+21];
         SysDataSend[ij]=ij;
       
       /*WK --> 显示修改的参数设置 */
        for(k=0; k<11; k++)
            {
                temp=7*k;
                ParaSetC108[temp + 0] = 0x3204;         //P  显示数据的模式
                ParaSetC108[temp + 1] = SysEventXY[2*k];            //显示相位的X坐标
                ParaSetC108[temp + 2] = SysEventXY[2*k + 1];     //Y坐标
                ParaSetC108[temp + 3] = 0xffff;         //白色，下为黑色
                ParaSetC108[temp + 4] = 0x0000;
                ParaSetC108[temp + 5] =(U16) (SysFlashDataT[3+4*k+EVESET_INDEX]<<8)+(U16)(SysFlashDataT[2+4*k+EVESET_INDEX]);
                ParaSetC108[temp + 6] =(U16) (SysFlashDataT[1+4*k+EVESET_INDEX]<<8)+(U16)(SysFlashDataT[4*k+EVESET_INDEX]);
            }
         YADA_C0(EventSetAddr, ParaSetC108, 11*7);
         YADA_C108(EventSetAddr,11);   //写入有效值，每次10个
       /* wk --> 保存成功并发送成功标志 END */
         
       SysSet.EventSaveFlg=0; //清楚保存标志
       SysSet.EventSendFlg=1; //开启 K60 2 DSP 标志
       _mem_free(shell_ptr); 
    }
    else
      _mem_free(shell_ptr); 
    
    if(SysSet.EventSendFlg==1)
    {
        YADA_71(MenuSysEvent,521,425,630,439,521,425);
        YADA_98(521, 425, 0x22, 0x81, 0x02, 0xffe0, 0x0000, "SENDING...", 11);
    }
    else if(SysSet.EventSendFlg==2)
    {
        YADA_71(MenuSysEvent,521,425,630,439,521,425);
        YADA_98(521, 425, 0x22, 0x81, 0x02, 0xffe0, 0x0000, "SUCCESS...", 11); // 可以考虑换成擦除上面的文字
        
         /*WK --> 保存成功并发送成功标志，使字体变黄显示 */
        for(k=0; k<11; k++)
            {
                temp=7*k;
                ParaSetC108[temp + 0] = 0x3204;         //P  显示数据的模式
                ParaSetC108[temp + 1] = SysEventXY[2*k];            //显示相位的X坐标
                ParaSetC108[temp + 2] = SysEventXY[2*k + 1];     //Y坐标
                ParaSetC108[temp + 3] = 0xffc1;         //白色，下为黑色
                ParaSetC108[temp + 4] = 0x0000;
                ParaSetC108[temp + 5] =(U16) (SysFlashDataT[3+4*k+EVESET_INDEX]<<8)+(U16)(SysFlashDataT[2+4*k+EVESET_INDEX]);
                ParaSetC108[temp + 6] =(U16) (SysFlashDataT[1+4*k+EVESET_INDEX]<<8)+(U16)(SysFlashDataT[4*k+EVESET_INDEX]);
            }
         YADA_C0(EventSetAddr, ParaSetC108, 11*7);
         YADA_C108(EventSetAddr,11);   //写入有效值，每次10个
       /* wk --> 保存成功并发送成功标志 END */
    }
    else
    {
       YADA_71(MenuSysEvent,521,425,630,439,521,425);
       YADA_98(521, 425, 0x22, 0x81, 0x02, 0xffe0, 0x0000, "WAITING...", 11);
    }
    
}

/*******************************************************************************
* 函  数  名      : GUI_INIT_SET
* 描      述      : 恢复出厂设置，确认后系统各设置参数恢复最初状态。
* 输      入      : 无
* 返      回      : 无
*******************************************************************************/
void GUI_INIT_SET(void)
{
    U8 PBUF[]= {"恢复出厂设置成功！"};
    
    SHELL_CONTEXT_PTR    shell_ptr;
    shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
    _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
    
    if(InitAck)
    {
        flg_int();

        shell_ptr->ARGC=2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="f:\\"; 
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC = 2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="sysset";
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        shell_ptr->ARGC=4;
        shell_ptr->ARGV[0]="w";
        shell_ptr->ARGV[1]="sysset.txt";
        shell_ptr->ARGV[2]="begin";
        shell_ptr->ARGV[3]="0";
        Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,25,SysFlashDataT);
        
        shell_ptr->ARGV[1]="sysevent.txt";
        Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,45,&SysFlashDataT[25]);
        
        shell_ptr->ARGC=2;
        shell_ptr->ARGV[0]="update"; // wk --> update
        shell_ptr->ARGV[1]="flush";
        Shell_update(shell_ptr->ARGC, shell_ptr->ARGV);
        
        TimeSet();
          
        YADA_98(200, 211, 0x22, 0x81, 0x02, 0xffe0, 0x0000, PBUF, 0);
        InitAck=0;
    }

    _mem_free(shell_ptr); 
}

/*******************************************************************************
* 函  数  名      : GUI_EventMonitor
* 描      述      : 事件监测显示，NPage[101~114]为事件的类别叠加;
* 输      入      : 无
* 返      回      : 无
* 
* 修改：修改完成！@130504
*******************************************************************************/
void GUI_EventMonitor(U8 U_DISK)
{
        U16 EVENTMONXY[18]= {181,127,181,174,181,219,181,265,181,310,505,127,505,174,
                         505,219,505,265
                        };
    U16 MONITC108[63]={0};
    U8 temp=0;
    for(U8 k=0; k<9; k++)
    {
        temp=7*k;
        MONITC108[temp + 0] = 0x6004;         //P  显示数据的模式
        MONITC108[temp + 1] =EVENTMONXY[2*k];            //显示相位的X坐标
        MONITC108[temp + 2] = EVENTMONXY[2*k + 1];     //Y坐标
        MONITC108[temp + 3] = 0xffff;         //白色，下为黑色
        MONITC108[temp + 4] = 0x0000;
        MONITC108[temp + 5] =0;
        MONITC108[temp + 6] =EventNum[k];
    }
    YADA_C0(EventMonLAddr, MONITC108, 63);
    YADA_C108(EventMonLAddr, 9);   //写入事件，每次10个
}
/*******************************************************************************
* 函  数  名      : GUI_EventList
* 描      述      : 事件列表显示，从nandflash中读取。
* 输      入      : 无
* 返      回      : 无
*
* 修改： 列表显示完成 @130504 
*******************************************************************************/
void GUI_EventList(void)
{
  U16 EVELSTXY[]= {30,40,35,63,30,70,35,93,30,98,35,121,30,125,35,148,30,152,35,175,30,179,35,202,30,206,
                     35,229,30,233,35,256,30,262,35,285,30,288,35,311,30,316,35,339,30,342,35,365,30,370,35,393,30,396,35,419
                    };
    U8 wNum; //wk@130504 -->  每页列表写的次数
    char EVECONTENT[14][34]= {0};
    char EvntType[][6]= {"U_ERR","F_ERR","U_WAV","U_UNB","L_FLK","U_THD","I_HAM","E_HAM","O_HAM"};
    char EvntPhase[2][6]= {"START","STOP "};
    uchar EvntHead[10];
    
    char temp_dir[5]="2013",file_name[12]="wk12345.csv";
    U16 temp_year=0;U8 temp_month=0,month;U32 file_num;
    
    YADA_40(0x0000,0xfc00);//前景色为黑色，背景色为橙色 set only once
//    if(EVEfunflg==1)  //功能键发生标志置一
//    {
//        YADA_5B(&EVELSTXY[EventOldIndex*4],4);//擦除前一处
//        YADA_5A(&EVELSTXY[EVEline*4],4);    //背景色填充相所在的矩形区域
//        EventOldIndex=EVEline;
//        EveRdNum=EVEline+14*EVEpage+1;     //2013-4-9-10-18新增EveRdNum变量，记录故障录波波形的地址，加1.
//        EVEfunflg=0;
//    }
//    else
//    {
//      YADA_5A(&EVELSTXY[EventOldIndex*4],4); //2013-4-9-15-18,未按上移下移按键的标注。
//    } 
    if(USB_Flg==1&& SysFlashData[6]==1)
    {
      SHELL_CONTEXT_PTR    shell_ptr;
      shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
      _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
      
      if((EvntPgUpFlg==1) || ((EVEpage+1)<(EVEnum/14)) )
        {
           wNum=14;
           EvntPgUpFlg=0;
        }
        else 
           wNum=EVEnum%15;
      
      shell_ptr->ARGC = 2;
      shell_ptr->ARGV[0]="cd";
      shell_ptr->ARGV[1]="u:\\event"; 
      Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
 
      for(uchar i=0;i<wNum;i++)
      {
          EvntPgUpFlg=0;
          if(temp_year!=year_oldES)
          {
            temp_year=year_oldES;
            sprintf(temp_dir,"%d",year_oldES);
            
            shell_ptr->ARGC = 2;
            shell_ptr->ARGV[0]="cd";
            shell_ptr->ARGV[1]=temp_dir; 
            Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
          }
          month = (EventAddr[i+14*EVEpage]>>22)&0x0f;
          if(temp_month!=month)
          {
            temp_month=month;
            sprintf(temp_dir,"%d",month);
            shell_ptr->ARGC = 2;
            shell_ptr->ARGV[0]="cd";
            shell_ptr->ARGV[1]=temp_dir; 
            Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
          }
          file_num=EventAddr[i+14*EVEpage]&0x3fffff;  // 
          /* wk @30425 --> end */
          sprintf(file_name,"%d.csv",file_num);
          
          shell_ptr->ARGC=5;
          shell_ptr->ARGV[0]="read";
          shell_ptr->ARGV[1]=file_name;
          shell_ptr->ARGV[2]="10";
          shell_ptr->ARGV[3]="begin";
          shell_ptr->ARGV[4]="0";
          Shell_read_wk(shell_ptr->ARGC, shell_ptr->ARGV,EvntHead);
          
          sprintf( EVECONTENT[i], "%d %d-%d-%d %d:%d:%d %.6s %.6s",EvntHead[0],(U16)EvntHead[1]+((U16)EvntHead[2]<<8),
                   EvntHead[3],EvntHead[4],EvntHead[5],EvntHead[6],EvntHead[7],EvntType[EvntHead[8]&0x0f],EvntPhase[EvntHead[9]]); 
          asm("NOP");
          YADA_98(40, EVELSTXY[i*4+1], 0x22, 0x81, 0x02, C108FC_W, 0x0000, (U8 *)EVECONTENT[i], 34);  
      }  
       
    if(EVEfunflg==1)  //功能键发生标志置一
    {
        YADA_5B(&EVELSTXY[EventOldIndex*4],4);//擦除前一处
        YADA_5A(&EVELSTXY[EVEline*4],4);    //背景色填充相所在的矩形区域
        EventOldIndex=EVEline;
        EveRdNum=EVEline+14*EVEpage+1;     //2013-4-9-10-18新增EveRdNum变量，记录故障录波波形的地址，加1.
        EVEfunflg=0;
    }
    else
    {
      YADA_5A(&EVELSTXY[EventOldIndex*4],4); //2013-4-9-15-18,未按上移下移按键的标注。
    }
    
      _mem_free(shell_ptr);
    }
    else if(USB_Flg==0)
    {
      printf("\nATTENTION:USB is DETACHED!\n");
    }
    else if(SysFlashData[6]==0)
    {
      printf("\nATTENTION:USB Switch is CLOSED!\n");
    }
}
/*******************************************************************************
* 函  数  名      : GUI_EventWave
* 描      述      : 事件波形显示
* 输      入      : 无
* 返      回      : 无
* 问      题      : ? 在函数内部定义局部变量数据 EvntWave时，程序会走飞，研究栈与堆可以解决此问题。? @130413
*******************************************************************************/ 

void GUI_EventWave(U8 U_DISK)
{
    U16 Coord_UI[]= {14,68,200,404,273,429};   //剪切及原点坐标;
    U16 XY[]= {36,54,40,50,44,54,40,50,40,420,40,224,598,224,594,220,598,224,594,228};
//    U16 EVEUI[192];
//    U8 EvntWave[1536]; // wk @130504 --> 定义局部变量时，程序跑飞，暂时定义成全局变量
    char file_name[18]="1234567.csv",temp_dir[5]="2013";
    uint_32 file_num;
    U8 month;
    
    if(USB_Flg==1&& SysFlashData[6]==1)
    {
      if((EVEnum>0)&&(EveRdNum<=(EVEnum)))  //2013-4-9-12-30故障发生才显示。
      {
        SHELL_CONTEXT_PTR    shell_ptr;
        shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
        _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
        
        shell_ptr->ARGC = 2;
        shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]="u:\\event"; 
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);   
        
        sprintf(temp_dir,"%d",year_oldES);
    //      shell_ptr->ARGC = 2;
    //      shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]=temp_dir; 
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
              
        month = (EventAddr[EveRdNum-1]>>22)&0x0f;
        sprintf(temp_dir,"%d",month);
    //      shell_ptr->ARGC = 2;
    //      shell_ptr->ARGV[0]="cd";
        shell_ptr->ARGV[1]=temp_dir; 
        Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
        
        file_num=EventAddr[EveRdNum-1]; // wk @130413-->获取事件的文件名的月、日、时、分、秒值
        sprintf(file_name,"%d.CSV",file_num&0x3fffff);///100000000
        
        shell_ptr->ARGC=5;
        shell_ptr->ARGV[0]="read";
        shell_ptr->ARGV[1]=file_name;
        shell_ptr->ARGV[2]="6144";
        shell_ptr->ARGV[3]="begin";
        shell_ptr->ARGV[4]="10";
        Shell_read_wk(shell_ptr->ARGC, shell_ptr->ARGV,EvntWave);  
       
        for(U16 j=0; j<LINENUM; j++)
        {
            //JT-test 2013-4-6，放大倍数改变。
            EVEUI[j]=(((((int_32)EvntWave[j*24])<<24)+(((int_32)EvntWave[j*24+1])<<16))>>22)+90;
            EVEUI[j+LINENUM]=(((((int_32)EvntWave[j*24+2])<<24)+(((int_32)EvntWave[j*24+3])<<16))>>22)+90;
            EVEUI[j+LINENUM*2]=(((((int_32)EvntWave[j*24+4])<<24)+(((int_32)EvntWave[j*24+5])<<16))>>22)+90;
        }
        /* wk @130508--> 显示点数 LINENUM */ 
        YADA_C0 (0x0000,EVEUI,LINENUM>>1);
        YADA_C0 (0x0000+(LINENUM>>1),&EVEUI[LINENUM>>1],LINENUM>>1);
        YADA_C0 (0x0000,EVEUI,255); // wk@130508-->显示点数 LINENUM-1
        /* wk @130508--> 显示点数 LINENUM */ 
        YADA_C0 (0x0100,&EVEUI[LINENUM],LINENUM>>1);// UB_addr
        YADA_C0 (0x0100+(LINENUM>>1),&EVEUI[LINENUM+(LINENUM>>1)],LINENUM>>1);// UB_addr
//        YADA_C0 (0x0100,&EVEUI[LINENUM],255); // wk@130508-->显示点数 LINENUM-1 UB_addr
       /* wk @130508--> 显示点数 LINENUM */  
        YADA_C0 (0x0200,&EVEUI[LINENUM<<1],LINENUM>>1);
        YADA_C0 (0x0200+(LINENUM>>1),&EVEUI[(LINENUM<<1)+(LINENUM>>1)],LINENUM>>1);
//        YADA_C0 (0x0200,&EVEUI[LINENUM<<1],255);// wk@130508-->显示点数 LINENUM-1 UC_addr
      
        //JT-test xy坐标
        YADA_40(0xffff,0x0000);
        YADA_56(XY,6);
        YADA_56(&XY[6],4);
        YADA_56(&XY[10],4);
        YADA_56(&XY[14],6);
        YADA_98(44,56, 0x22, 0x81, 0x02, C108FC_W, 0x0000,"U", 1);
        
        YADA_C103 (0x0000,40,Coord_UI[3],255,1,2,32,COLOR[0]);
        YADA_C103 (0x0100,40,Coord_UI[3],255,1,2,32,COLOR[1]);
        YADA_C103 (0x0200,40,Coord_UI[3],255,1,2,32,COLOR[2]);  
        
        YADA_98(150, 40, 0x22, 0x81, 0x02, C108FC_W, 0x0000,(U8 *)file_name, 12);//2013-4-9-10-18测试事件文件名 ????
        delay_ms(5);
        
       _mem_free(shell_ptr);
     }
    }
    else if(USB_Flg==0)
    {
      printf("\nATTENTION:USB is DETACHED!\n");
    }
    else if(SysFlashData[6]==0)
    {
      printf("\nATTENTION:USB Switch is CLOSED!\n");
    }
}
/*******************************************************************************
* 函  数  名      : GUI_STATUS
* 描      述      : 工作状态的显示，如U盘存储，IP地址，版本号等。
* 输      入      : U8 U_DISK，U盘的标志。
* 返      回      : 无
*******************************************************************************/
void GUI_STATUS(U8 U_DISK)
{
  // wk @130409 --> 内容待完善
    U16 StatusC108[21]= {0},U_DISC[3]= {0};
    U8 temp=0; 
    char file_name[15]="48.123.72.200";
    
    uint_32 ipaddr= 200 + ((uint_32)72<<8) + ((uint_32)123<<16) + ((uint_32)49<<24);
    sprintf(file_name,"%d.%d.%d.%d",(ipaddr>>24)&0xff,(ipaddr>>16)&0xff,(ipaddr>>8)&0xff,(ipaddr)&0xff);
    YADA_98(150, 168, 0x22, 0x81, 0x02, C108FC_W, 0x0000, file_name, 15);  
    
    if(USB_Flg==1)
    {
      SHELL_CONTEXT_PTR    shell_ptr;
      shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
      _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
      
      shell_ptr->ARGC=2;
      shell_ptr->ARGV[0]="df_d";
      shell_ptr->ARGV[1]="u:";
      Shell_df_driver(shell_ptr->ARGC, shell_ptr->ARGV,&U_DISC[0],&U_DISC[1]);
      U_DISC[2]=(U16)(U_DISC[1]/120);
      
      _mem_free(shell_ptr);
    }
    
    for(U8 i=0; i<3; i++)
    {
        temp=7*i;
        StatusC108[temp]=0x6004;//显示数据的模式
        StatusC108[temp+1]=480;//显示左边列谐波数据的X为82,右边列谐波数据X为438
        StatusC108[temp+2]=94+37*i;//Y坐标
        StatusC108[temp+3]=C108FC_W;
        StatusC108[temp+4]=0x0000;
        StatusC108[temp+5]=0;
        StatusC108[temp+6]=U_DISC[i];
    }
    YADA_C0(StatusAddr,StatusC108,21);
    YADA_C108(StatusAddr,3);   
}
/*******************************************************************************
* 函  数  名      : EventSave
* 描      述      : 事件存储
* 输      入      :
* 返      回      : 无
*******************************************************************************/
void EventSave(U8 U_DISK)
{
    if(USB_Flg==1&& SysFlashData[6]==1) // ==1 时插入
    {
          SHELL_CONTEXT_PTR    shell_ptr;
          shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
          _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
          
//          static char file_name[12]="wk12345.csv",evntdir_name[5]="2013",monthDir_name[3]="12";
//          
//          static uint_16 month_old=0;
          
          TIME_STRUCT             time_sf;
          DATE_STRUCT             date_sf;     
          _time_get(&time_sf);
          _time_to_date(&time_sf,&date_sf);
          time[0]=date_sf.YEAR&0x00ff;
          time[1]=date_sf.YEAR>>8;
          time[2]=date_sf.MONTH;
          time[3]=date_sf.DAY;
          time[4]=date_sf.HOUR;
          time[5]=date_sf.MINUTE;
          time[6]=date_sf.SECOND;
    
          shell_ptr->ARGC = 2;
          shell_ptr->ARGV[0]="cd";
          shell_ptr->ARGV[1]="u:\\event"; 
          Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
        
          if(year_oldES!=date_sf.YEAR) // wk --> creata a dir named of year
          {
//            evntdir_name=num2string(date_sf.YEAR,4,0);
            sprintf(dir_nameES,"%d",date_sf.YEAR);
            year_oldES=date_sf.YEAR;
            
            shell_ptr->ARGC = 2;
            shell_ptr->ARGV[0]="mkdir";
            shell_ptr->ARGV[1]=dir_nameES; 
            Shell_mkdir(shell_ptr->ARGC, shell_ptr->ARGV);
          }
          
          shell_ptr->ARGC = 2;
          shell_ptr->ARGV[0]="cd";
          shell_ptr->ARGV[1]=dir_nameES; 
          Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
          if(month_oldES!=date_sf.MONTH)
          {
//            monthDir_name=num2string(date_sf.MONTH,2,0);
            sprintf(monthDir_nameES,"%d",date_sf.MONTH);
            month_oldES=date_sf.MONTH;
            
            shell_ptr->ARGC = 2;
            shell_ptr->ARGV[0]="mkdir";
            shell_ptr->ARGV[1]=monthDir_nameES; 
            Shell_mkdir(shell_ptr->ARGC, shell_ptr->ARGV);
          }
          shell_ptr->ARGC = 2;
          shell_ptr->ARGV[0]="cd";
          shell_ptr->ARGV[1]=monthDir_nameES; 
          Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
                    
          sprintf(file_nameES,"%d.csv",date_sf.SECOND+(date_sf.MINUTE<<6)+(date_sf.HOUR<<12)+(date_sf.DAY<<17));
          
          if(EVEnum==100)
          {
            EVEnum=1;
            
            for(uchar tmpNum=0;tmpNum<100;tmpNum++)
              EventAddr[tmpNum]=0;
            for(uchar tmpNum=0;tmpNum<9;tmpNum++)
              EventNum[tmpNum]=0;
          }
          else
            EVEnum++; // wk @130412-->事件总数
//          EventAddr[EVEnum-1]=date_sf.MONTH*100000000+date_sf.DAY*1000000+date_sf.HOUR*10000+
//                            date_sf.MINUTE*100+date_sf.SECOND; // wk @130412-->获得当前事件的地址：、日、时、分、秒
          EventAddr[EVEnum-1]= date_sf.SECOND+((U32)date_sf.MINUTE<<6)+((U32)date_sf.HOUR<<12)+((U32)date_sf.DAY<<17)
                               +((U32)date_sf.MONTH<<22)+((U32)(EvntRxchar[0]&0x0f)<<26)+((U32)(EvntRxchar[1]&0x01)<<30);  //wk @130425 -->添加事件开始/结束标志
          /* wk @130504--> 待补充事件次数限制判断*/
          EventNum[(EvntRxchar[0]&0x3f)]++; // wk @130412-->事件类型叠加
          
          /* wk @130412--> 总数 + 时间 + 类型 + 开始/结束 + 数据 */
         
          shell_ptr->ARGC=4;
          shell_ptr->ARGV[0]="write";
          shell_ptr->ARGV[1]=file_nameES;
          shell_ptr->ARGV[2]="current";
          shell_ptr->ARGV[3]="0";
          Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,1,&EVEnum);
          Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,7,time);
          /* wk @130412 --> test */
//          uchar test[]={0,1,2,3,4,5,6,7,8,9,10};
//          Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,100,test);
          /* wk @130412 --> save event data */
          Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,Evnt_SIZE,EvntRxchar); // wk @130412-->EvntRxchar包含标志位和事件数据
    
         _mem_free(shell_ptr);  // wk @130403 --> important
    }
  else if(USB_Flg==0)
  {
    printf("\nATTENTION:USB is DETACHED!\n");
  }
  else if(SysFlashData[6]==0)
  {
    printf("\nATTENTION:USB Switch is CLOSED!\n");
  }
}
/*******************************************************************************
* 函  数  名      : PowerSave
* 描      述      : 电能数据存储
* 输      入      : 无
* 返      回      : 无
*******************************************************************************/
void PowerSave(void)
{
  if(USB_Flg==1&& SysFlashData[6]==1)
  {
      SHELL_CONTEXT_PTR    shell_ptr;
      shell_ptr = _mem_alloc_zero( sizeof( SHELL_CONTEXT ));
      _mem_set_type(shell_ptr, MEM_TYPE_SHELL_CONTEXT);
      uint_32 file_size;
//      static char file_name[10]="wk384.csv",dir_name[5]="1000";
//      static uint_16 year_old=0;
      TIME_STRUCT             time_sf;
      DATE_STRUCT             date_sf;
      
      _time_get(&time_sf);
      _time_to_date(&time_sf,&date_sf);
      time[0]=date_sf.YEAR&0x00ff;
      time[1]=date_sf.YEAR>>8;
      time[2]=date_sf.MONTH;
      time[3]=date_sf.DAY;
      time[4]=date_sf.HOUR;
      time[5]=date_sf.MINUTE;
      time[6]=date_sf.SECOND;
      
//      printf("Y=%d\tM=%d\tD=%d\tH=%d\tM=%d\tS=%d\n",date_sf.YEAR,date_sf.MONTH,date_sf.DAY,date_sf.HOUR,date_sf.MINUTE,date_sf.SECOND);
      
      shell_ptr->ARGC = 2;
      shell_ptr->ARGV[0]="cd";
      shell_ptr->ARGV[1]="u:\\power"; 
      Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
      
      if(year_oldPS!=date_sf.YEAR) // wk --> creata a dir named of year
      {
//        dir_name=num2string((uint_32)date_sf.YEAR,4,0);
        sprintf(dir_namePS,"%d",date_sf.YEAR);
        year_oldPS=date_sf.YEAR;
        
//        shell_ptr->ARGC = 2;
//        shell_ptr->ARGV[0]="mkdir";
        shell_ptr->ARGV[1]=dir_namePS; 
        Shell_mkdir(shell_ptr->ARGC, shell_ptr->ARGV);
      }
      /* wk @130407 --> 注意： 这里可以添加年份文件夹查找的，确定文件夹已经建立在打开 */
//      shell_ptr->ARGC = 2;  //WK --> 进入 dir_name 下面
//      shell_ptr->ARGV[0]="cd";
      shell_ptr->ARGV[1]=dir_namePS; 
      Shell_cd(shell_ptr->ARGC, shell_ptr->ARGV);
      
      if(*file_namePS=='w') // wk --> 第一次进来时，用月、日、时获取文件名
      {
//       file_name=num2string(date_sf.MINUTE+(date_sf.DAY<<6)+(date_sf.MONTH<<11),6,1);
        sprintf(file_namePS,"%d.CSV",date_sf.HOUR+(date_sf.DAY<<5)+(date_sf.MONTH<<10));
      }
      else
      {
//        shell_ptr->ARGC = 2;
//        shell_ptr->ARGV[0]="df_s";
        shell_ptr->ARGV[1]=file_namePS;   //wk --> 注意：查找的文件名暂时必须要是大写
        Shell_search_file_r1(shell_ptr->ARGC, shell_ptr->ARGV,&file_size);
        
        if(file_size>=134216618)  // wk --> 128M = 128*1024*1024 bytes = 134217728 bytes,但是我们取 2539的52862倍 134216618
        {
//          file_name=num2string(date_sf.MINUTE+(date_sf.DAY<<6)+(date_sf.MONTH<<11),6,1);
          sprintf(file_namePS,"%d.CSV",date_sf.HOUR+(date_sf.DAY<<5)+(date_sf.MONTH<<10));
        }
      }
      
      shell_ptr->ARGC=4;
      shell_ptr->ARGV[0]="write";
      shell_ptr->ARGV[1]=file_namePS;
      shell_ptr->ARGV[2]="current";
      shell_ptr->ARGV[3]="0";
      Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,7,time);
           
//      shell_ptr->ARGC=4;
//      shell_ptr->ARGV[0]="write";
//      shell_ptr->ARGV[1]=file_name;
//      shell_ptr->ARGV[2]="current";
//      shell_ptr->ARGV[3]="0";
        Shell_write_binary(shell_ptr->ARGC, shell_ptr->ARGV,Pow_SIZE,PowRxchar);

     _mem_free(shell_ptr);  // wk @130403 --> important
  }
  else if(USB_Flg==0)
  {
    printf("\nATTENTION:USB is DETACHED!\n");
  }
  else if(SysFlashData[6]==0)
  {
    printf("\nATTENTION:USB Switch is CLOSED!\n");
  }
     
}

void flg_int(void)   // wk --> 一些标志的初始化 
{
//    Dis_PicID=0;
//    HarmoGraphShift=0;//
    HarmoGraphPhase=1;// 默认值1
    HarmoGraphRange=1;// 用于选择显示谐波范围，默认值为1,对应1~26次，2对应25~50
    HarmoGraphUorder=1;// 用于控制谐波电压具体次数的数值显示
    HarmoGraphIorder=1;// 用于控制谐波电流具体次数的数值显示
    HarmoListShift=0;// 谐波列表显示中的功能右移键
    HarmoListPhase=1;
    HarmoListUorI=1;
    HarmoListRange=1;
    HarmoListAmporRatio=1;
    VIEWHoldFlg=0; //保 持键默认为0，键按下时值变为1，再次按下时值变为0；
    ViewKeyFlg=0;
    SysSetKeyFlg=0;
    EventKeyFlg=0;
    MenuSwFlg=0;
    EVEpage=0;
    EVEline=0;
    EVEfunflg=0;
    
    SavePowerFlg =0;
    /* wk@130509-->初始化K602DSP数组的数据头和数据尾*/
    SysDataSend[0]=0x33;SysDataSend[1]=0x33;SysDataSend[2]=0x33;SysDataSend[3]=0x44;
    SysDataSend[52]=0xcc;SysDataSend[53]=0x33;SysDataSend[54]=0xc3;SysDataSend[55]=0x3c;
    
    for(uchar i=0;i<84;i++)
     SysFlashData[i]=SysFlashDataT[i]=0;
    
    SysFlashData[5]=SysFlashDataT[5]=1;
    SysFlashData[6]=SysFlashDataT[6]=1; // 存储时间间隔
    
    SysFlashData[9]=SysFlashDataT[9]=3;
    SysFlashData[10]=SysFlashDataT[10]=0; // 存储时间间隔
    
    SysFlashData[13]=SysFlashDataT[13]=0xb2;
    SysFlashData[14]=SysFlashDataT[14]=0x07; // year
    SysFlashData[15]=SysFlashDataT[15]=1;   // month
    SysFlashData[17]=SysFlashDataT[17]=1;   // date
    SysFlashData[19]=SysFlashDataT[19]=1;   //hour
    SysFlashData[21]=SysFlashDataT[21]=1;   // minite 
    
    /* 事件阈值初始化 */
    SysFlashData[25]=SysFlashDataT[25]=(U8)(400&0x00ff);   
    SysFlashData[26]=SysFlashDataT[26]=400>>8;   // 周波数 
    SysFlashData[29]=SysFlashDataT[29]=(U8)(25600&0x00ff);   
    SysFlashData[30]=SysFlashDataT[30]=25600>>8;   //波点数
    SysFlashData[33]=SysFlashDataT[33]=(U8)(UDeviation&0x00ff);   
    SysFlashData[34]=SysFlashDataT[34]=UDeviation>>8;   //电压偏差
    SysFlashData[37]=SysFlashDataT[37]=FDeviation;   //频率偏差
    SysFlashData[41]=SysFlashDataT[41]=USurge;   //电压波动
    SysFlashData[45]=SysFlashDataT[45]=NUnblance;   //负序不平衡
    SysFlashData[49]=SysFlashDataT[49]=LngFlick;   //长时闪变
    SysFlashData[53]=SysFlashDataT[53]=(U8)(TotalHarmonic&0x00ff);
    SysFlashData[54]=SysFlashDataT[54]=TotalHarmonic>>8;   //电压总畸变率
    SysFlashData[61]=SysFlashDataT[61]=(U8)(400&0x00ff);  
    SysFlashData[62]=SysFlashDataT[62]=UHarmonic>>8;   //偶谐波
    SysFlashData[65]=SysFlashDataT[65]=IHarmonic;   //奇谐波
    
    /* wk@130514--> 事件记录信息初始化  */
    for(uchar i=0;i<9;i++)
    {
      EventNum[i]=0;
    }
    for(uchar i=0;i<100;i++)
    {
      EventAddr[i]=0;
    }
   
}