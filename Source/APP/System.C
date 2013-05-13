/*******************************************************************************
* File Name        : System.c
* Author             : wk
* Version            : V1.0.0
* Date                :
* Description        : —” ±≥Ã–Ú
*******************************************************************************/

#include "includes.h"

// Defined for Delay_ms_Common()
#define MCU_BUS_CLOCK 96000000L
#define MCU_BUS_CYCLES_100US (MCU_BUS_CLOCK / 10000)

static void Cpu_Delay100US(U16 us100)
{
  unsigned int i;
  while((us100--)){
    for(i=0; i < (MCU_BUS_CYCLES_100US/10); i++)
    {// 10 cycles delay
      asm("NOP");
      asm("NOP");
      asm("NOP");
    }
  }
}

void delay_ms(U16 period)   //delay routine (milliseconds)
{
  
  while (period != 0)
  {
    Cpu_Delay100US (10);          
    period--;    
  }  
}


void delay_us(U16 uscnt)
{
  while(uscnt--)
  {
    for(U16 i=0;i<24;i++)  //  1*10ns+ uscnt* (22*(4+x*nop) * 10ns)= 1us 
    {
      asm("NOP");
    }
  }
}