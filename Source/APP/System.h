// filename: typedef.h
/* wk @2012-11-29 */
/* define for 430 -> k60 */

#ifndef SYSTEM_H
#define SYSTEM_H

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned long U32;

//#define U8 unsigned char 
//#define U16 unsigned short
//#define U32 unsigned long

/* º¯ÊıÉêÃ÷ */
void delay_us(U16 uscnt);
void delay_ms(U16 period);   //delay routine (milliseconds)
//#define delay_time(U32) _time_delay(U32)
#endif