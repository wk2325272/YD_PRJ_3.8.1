#ifndef DSPSPI_H
#define DSPSPI_H

#include "includes.h"

/* wk @130412 --> �����ú궨�� */
#define T_SPI 0

#define ARRAY_SIZE 1 // wk --> for test
/* wk @130412 : Pow_SIZE --> 2052 + 480 */
/* ������������ + �������� */
#define Pow_SIZE  2532 

/* wk @130412 : OFFSET --> 14 + 84 + 2*/
/* Ԥ�� + MCU2DSP + ��־ */
#define OffSET 16 // WK --> ���ڴ�BufRxchar��ȡ�����������ݵ�PowRxchar�е�ƫ��


#define Evnt_SIZE 6146
#define TEST_CHANNEL_INT2 "ispi2:"

/* wk -->  �������� */
extern volatile U8 SPIPowerFlg;//Ĭ��ֵ0
extern volatile U8 SPIEventFlg;//Ĭ��ֵ0


/* wk -->  �������� */
//void int_callback(void);
void spi2_dma_int(void);
inline void ptb_10_int(void);
void DMA_RecData_OK();
void SPIDMA_Task( uint_32 param);


/* wk --> 20130125 */
//#define BSP_SPI2_INTERRUPT_VECTOR INT_SPI2
//void new_spi2_isr(pointer user_spi2_ptr);
//typedef struct isr_spi2_struct
//{
//  pointer ISR_DATA;
//  U8   data;
//} ISR_SPI2_STRUCT, _PTR_  ISR_SPI2_STRUCT_PTR;

#endif