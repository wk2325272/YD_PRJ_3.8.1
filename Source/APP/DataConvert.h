#ifndef DATAC_H
#define DATAC_H

#include "includes.h"



/*  º¯ÊýÉêÃ÷ */
 void UIValues2HR(float UIValues[21],U16 HR[]);
 void ChartoFloat(U8 CharIn[], float FloatOut[],U16 FNum,U16 times);
 void BCD2Decimal(U8 Data[],U8 Num);
// void FloatTo4DecimalsC108(float DataIn[],U16 DataBack[],U8 Num);
// void PhaseShift(float DataIn[],U16 DataBack[],U8 Num,U32 DATABACK[]); // wk @130420
 void PhaseShift(float DataIn[],U16 DataBack[],U8 Num,U8 DATBACK[]);

 void CharToInt(U8 CharIn[], U16 WordOut[],U8 LineNum,U8 HalfRange);
 void WR_WAVE_UI(U16 DATAIN[],U8 UI_U_I, U8 rewrite);

 void AtoFA(U8 DATAIN[],float DATAOUT[],U16 NUM);
 void FLTOINT_UI(float DATAIN[],int DATOUT[],U16 FLAGNUM);
 void EVEUISHIFT(U8 DATAIN[],float DATAOUT[]);
 void U16TOFL_UI(U8 DADAIN[],U16 NUM,float DATAOUT[]);
 void Sig_Fiq(U8 DADAIN[],U8 DATAOUT[],U16 TIMES, U8 NUM);
 char_ptr num2string(int_32 num,uchar len,uchar type);
#endif