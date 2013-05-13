/*******************************************************************************
* File Name        : DSPSPI.c
* Author             : LB&TX
* Version            : V1.0.0
* Date                :
* Description        :SPI使用的是430的UartB1口。
                       SPI时钟由主机控制，
                       
*******************************************************************************/
#include "includes.h"

#define _SPIDMADATA_DBUG_  // 是否有 DBUG 输出信息

//U8 BufRxchar[ARRAY_SIZE]= {0};//转存数组，仅在此C文件中使用
U8 BufRxchar[6175]= {0};// wk -->转存数组，仅在此C文件中使用
U8 PowRxchar[Pow_SIZE]= {0}; //去除起始符结束符的有效数据，通信协议中的数据长度减去结束符
U8 EvntRxchar[Evnt_SIZE]= {0};
U8 SPIRxCnt=0;   //SPI接收标志，作用域在此文件
U16 TotalNum=0;//需接收数据的长度，作用域在此文件
//U8 DataType=0;//作用域在此文件
//U8 HeadFlg[4]={0}; // wk @20130325 -->  // wk @130403 --> uncomment
volatile U8 SPIPowerFlg;//默认值0
volatile U8 SPIEventFlg;//默认值0

U16 DataSize = ARRAY_SIZE ;  //wk -->传给 DMA 寄存器的一次服务数据长度
volatile static uchar count=0; // 用于数据头检测
U8 SPI_Send=0; // wk @130406 --> K60是否给DSP发送数据的标志
U8 HeadFlg[6]={0};
//volatile U16 Time_save=0;  //备份事件参数
//volatile U8 testflgg=0;   //MCU发送数据个数

MQX_FILE_PTR spifd_2; //spifd_2 --> spi2设备的句柄

 // wk @130403 --> uncomment
//GPIO_PIN_STRUCT pins_int[] = {    /* PTB10 定义结构体 */  // 定义 PTB10 上升沿中断
//            BSP_PTB10 | GPIO_PIN_IRQ_FALLING,
//            GPIO_LIST_END
//            };

/* wk --> 初始化与 DSP 通信使用的 SPI2 */
void spi2_dma_int(void)
{
  uint_32                param;
// @20130312 --> wk
//  spi_rw.BUFFER_LENGTH = 1; //初始化 spi_rw结构体数据长度
  /**************************** spi2 configure *************************/
#ifdef _SPIDBG_ 
    printf("\n-------------- Int SPI2 configure --------------\n\n");
#endif
//     /* Open the SPI driver */
   spifd_2 = fopen(TEST_CHANNEL_INT2,NULL);
     
   if (NULL == spifd_2)
   {
      printf ("Error opening SPI driver!\n");
      _time_delay (200L);
      _task_block ();
   }

   /* Set clock mode */
      param = SPI_CLK_POL_PHA_MODE1;
   if (SPI_OK == ioctl (spifd_2, IO_IOCTL_SPI_SET_MODE, &param))
   {
#ifdef _SPIDBG_
      printf ("SPI2 is OK!\n");
#endif
   } else {
#ifdef _SPIDBG_
      printf ("ERROR\n");
#endif
   }

   /* Set big endian */
   param = SPI_DEVICE_BIG_ENDIAN;    // wk -->这个设置是针对主机来说的，对于从机时设置都会出错
//   printf ("Setting endian to %s ... ", param == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
   if (SPI_OK == ioctl (spifd_2, IO_IOCTL_SPI_SET_ENDIAN, &param))
   {
#ifdef _SPIDBG_
      printf ("OK\n");
#endif   
   } else {
#ifdef _SPIDBG_     
      printf ("ERROR\n");
#endif      
   }
      /* Set transfer mode */
   param = SPI_DEVICE_SLAVE_MODE;
//   printf ("Setting SPI2 transfer mode to %s ... ", device_mode[param]);
   if (SPI_OK == ioctl (spifd_2, IO_IOCTL_SPI_SET_TRANSFER_MODE, &param))
   {
#ifdef _SPIDBG_      
      printf ("OK\n");
#endif     
   } else {
#ifdef _SPIDBG_
      printf ("ERROR\n");
#endif      
   }
   /* Clear statistics */
//   printf ("Clearing statistics ... ");
   if (SPI_OK == ioctl (spifd_2, IO_IOCTL_SPI_CLEAR_STATS, NULL))
   {
 #ifdef _SPIDBG_    
      printf ("OK\n");
 #endif 
   } else {
 #ifdef _SPIDBG_     
      printf ("ERROR\n");
  #endif     
   }
 #ifdef _SPIDBG_  
   printf("\n--------------       End       --------------\n\n");
 #endif  
   /******************************* spi2 conf end ****************************/
}

/* wk --> 初始化 PTB10 用于数据中断接收 */  // wk @130403 --> uncomment
//inline void ptb_10_int(void)
//{
// /* wk --> set gpio of ptb10 in int mode for spi data read */
//    ptbfd_10 = fopen("gpio:read", (char_ptr) &pins_int );
//    ioctl(ptbfd_10,GPIO_IOCTL_SET_IRQ_FUNCTION, (pointer)int_callback);
///* end */
//}

/*
** 函数名：
** 作者：
** 说明：在没有使用事件时 SPI 接收数据的外部函数
** 测试记录：wk@130408-->数据发送：5个数据头（4个数据头+1个启动信号）+正常数据，注意，这次测试结果表明，数据没有丢失
*/
void DMA_RecData_OK  
(
  void
)
{
#if 0 // wk @130412 --> 旧协议  
    if(count<4)
    {
      HeadFlg[count]=BufRxchar[0];
      count++;
      if(count==4)
        if(HeadFlg[0]==0x33 && HeadFlg[1]==0x33 && HeadFlg[2]==0x33 && (HeadFlg[3]== 0x44 ||HeadFlg[3]== 0x55))
        {
          if(HeadFlg[3]==0x44)
            DataSize= 2556; // wk @130420 -->发送数据 4+1+14+2+2532+12+4=2560
          else
            DataSize= Evnt_SIZE + 4; // wk @20130325 -->
          
            count=5; //  wk --> 数据头已经找到 
//          printf("\n1");
        }
        else
        {
          HeadFlg[0]=HeadFlg[1];
          HeadFlg[1]=HeadFlg[2];
          HeadFlg[2]=HeadFlg[3];
          count=3;
        }
     
    }
    else if(count==5)
    {
         count=6; // 用一个数据改变 DMA 接收数据的长度，此数据将会被舍弃        
//         SPI_Send=1; // wk @130406 --> K60是否给DSP发送数据的标志
    }
    else
    {
      /* wk @130408 --> data trans */ 
       if(HeadFlg[3]==0x44) // wk @20130325 -->
         for(int i=0;i<Pow_SIZE;i++)
         {
           PowRxchar[i] = BufRxchar[i+OffSET];
           SPIPowerFlg=1;
         }
       else
           printf("Event\n");  // 事件数据还没有处理
       
//        printf("%x\t%x\n",BufRxchar[0],BufRxchar[1]); // test 
        count=0; 
        DataSize=1;
        SPI_Send=0;
        fclose(spifd_2);
        asm("NOP");  
        spi2_dma_int(); // 刷新 DMA 寄存器
        asm("NOP");   
    }
#endif 
         /* wk @130412 -->基于新协议 */
#if 1
    if(count<6)
    {
      HeadFlg[count]=BufRxchar[0];
      count++;
      if(count==4)
      {
        if(HeadFlg[0]!=0x33 || HeadFlg[1]!=0x33 || HeadFlg[2]!=0x33 || (HeadFlg[3]!=0x44 && HeadFlg[3]!=0x55))
        {
          HeadFlg[0]=HeadFlg[1];
          HeadFlg[1]=HeadFlg[2];
          HeadFlg[2]=HeadFlg[3];
          count=3;
        }
      }
      else if(count==6)
      {
        // wk @130420 --> 发送数据 4+2+1+14+2+2532+12+4 = 2571,DMA的数据长度=2571-7=2564，但是数据接收时有两个丢失，故减2
        
        DataSize = (((U16)HeadFlg[4])<<8) + HeadFlg[5]-2;   
        count=7;
      }
     
    }
    else if(count==7)
    {
         count=8; // 用一个数据改变 DMA 接收数据的长度，此数据将会被舍弃
         /* wk @130420--> 暂时不考虑发送 */ 
         if(SysSet.EventSendFlg==1)
         SPI_Send=1; // wk @130406 --> K60是否给DSP发送数据的标志
    }
    else
    {
      /* wk @130408 --> data trans */ 
       if(HeadFlg[3]==0x44) // wk @20130325 -->
       {
         for(int i=0;i<Pow_SIZE;i++)
           PowRxchar[i] = BufRxchar[i+OffSET];
          
         SPIPowerFlg=1;
       }
       else
       {
         for(int i=0;i<Evnt_SIZE;i++)
           EvntRxchar[i]= BufRxchar[i+OffSET-2]; //wk @130412 -->将标志也存到数据中
         
         SPIEventFlg=1;
         
         /* 事件类型测试 */
//         uchar type;
//         type=EvntRxchar[0]&0x3f;
//         printf("%d\n",type);
         asm("NOP");

       }
       
       if(SysSet.EventSendFlg) //wk @130412 -->判断DSP是否成功接收数据
       {
         if(EvntRxchar[0]&0xC0==0xC0)
         { 
           SysSet.EventSendFlg=2;    //wk @130412 --> 发送成功       
           SPI_Send=0;  
         }
       }
       
#if T_SPI  
        printf("%x\t%x\n",BufRxchar[0],BufRxchar[1]); 
#endif  // END --> T_SPI 
        count=0; 
        DataSize=1;
        fclose(spifd_2);
        asm("NOP");  
        spi2_dma_int(); // 刷新 DMA 寄存器
        asm("NOP");   
    }
#endif
    
}

/*
** 函数名：
** 作者：
** 说明：在使用事件时 SPI 接收数据的外部任务函数
*/
void SPIDMA_Task( uint_32 param)
{
  pointer  pspidma_event;
#ifdef _SPIDMADATA_DBUG_
   printf("\n----------SPIDMA_Task----------\n");
   printf("\n----------             ----------\n");
   printf("\n----------             ----------\n");
   printf("\n----------     END     ----------\n");
#endif 
  
#ifdef _SPIDMADATA_DBUG_
    if(_event_create("spidma_event") != MQX_OK){  // 创建事件
      printf("\n Make spidma event failed");
      _task_block();
    }
    else
      printf("\n Make spidma event OK");
  
  if(_event_open("spidma_event", &pspidma_event) != MQX_OK){
    printf("\nOpen spidma event failed ");
    _task_block();
  }  
  else
    printf("\n Open spidma event OK");
#endif
  
#ifndef _SPIDMADATA_DBUG_
  _event_create("spidma_event");
  _event_open("spidma_event", &pspidma_event);
#endif
  
  while(TRUE)
  {
    _event_wait_all(pspidma_event,0x02,0); //等待事件发生
    _event_clear(pspidma_event,0x02);  // 清除事件标志
  
    if(BufRxchar[0]==0x66 && BufRxchar[1]==0x33 && BufRxchar[2]==0x33 && 
       BufRxchar[3]==0x33 && BufRxchar[4]==0x44 && BufRxchar[5]==0x0a&& 
       BufRxchar[6]==0x4c)
   { 
      for(int i=0 ;i<2635 ;i++)
        PowRxchar[i] = BufRxchar[i+7];
      SPIPowerFlg=1;
      printf("1\n");  // @20130312 --> wk: Test the data is OK ?
  }
  
//  printf("2\n");   // @20130312 --> wk: Test if enter the func ?
  }
    
}
