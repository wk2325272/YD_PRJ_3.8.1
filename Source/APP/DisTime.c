#include "includes.h"
#include "DisTime.h"

volatile U8 TimeFlg=1;
U8 MinuteOld=0;

/*******************************************************************************
** Function Name	：TimeSet
** Input		：
** Return		：
** Author		：wk
** Version	：
** Date		：
** Dessription	：设置系统时钟
** Reverse	：
*******************************************************************************/
void TimeSet()
{
   DATE_STRUCT     time_rtc;
   TIME_STRUCT     time_mqx;
    
   /* initialize time */
    time_rtc.YEAR     = (U16)SysFlashData[13]+((U16)SysFlashData[14]<<8);
    time_rtc.MONTH    = (U16)SysFlashData[15]+((U16)SysFlashData[16]<<8);
    time_rtc.DAY      = (U16)SysFlashData[17];
    time_rtc.HOUR     = (U16)SysFlashData[19];
    time_rtc.MINUTE   = (U16)SysFlashData[21];
    time_rtc.SECOND   = (U16)SysFlashData[23];
    time_rtc.MILLISEC = 0;
     
    _time_from_date (&time_rtc, &time_mqx);

    _time_set( &time_mqx);
    if( _rtc_sync_with_mqx(FALSE) != MQX_OK )
    {
        printf("\nError synchronize time!\n");
        _task_block();
    }
    _time_get (&time_mqx);

    _time_to_date (&time_mqx, &time_rtc);
    
    print_mqx_time(&time_rtc, &time_mqx);
    print_current_time();
    
    TimeFlg=1; // 设置完成全部显示时间
    _rtc_init (RTC_INIT_FLAG_CLEAR);
    asm("NOP");
    _rtc_init ( RTC_INIT_FLAG_ENABLE); 
}

/*******************************************************************************
** Function Name	：TimeDis
** Input		：
** Return		：
** Author		： wk
** Version	：
** Date		：
** Dessription	：在液晶屏上显示时间
** Reverse	：
*******************************************************************************/
void TimeDis(U8 DisType)
{
  if (Dis_PicID != MenuTop)
  {
        TIME_STRUCT             time_sf;
        DATE_STRUCT             date_sf;
        _time_get(&time_sf);
        _time_to_date(&time_sf,&date_sf);
        
        if(DisType)
        { 
          U8 CurrentTime[7]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //秒，分，时，周，日，月，年
          U16 TimeX[6]= {214,178,142,106,70,10};
          U16 TimeC108[42];
          
          CurrentTime[6]=date_sf.YEAR>>8;
          CurrentTime[5]=(U8)date_sf.YEAR&0x00ff;
          CurrentTime[4]=date_sf.MONTH;
          CurrentTime[3]=date_sf.DAY;
          CurrentTime[2]=date_sf.HOUR;
          CurrentTime[1]=date_sf.MINUTE;
          CurrentTime[0]=date_sf.SECOND;

          for(U8 i=0; i<6; i++)
          {            
              TimeC108[7*i]=0x2084;//显示控制，2位整数，无效0显示，字体大小12*24
              TimeC108[7*i+1]=TimeX[i];
              TimeC108[7*i+2]=5;
              TimeC108[7*i+3]=0xffff;
              TimeC108[7*i+4]=0x0000;
              TimeC108[7*i+5]=0x0000;
              if(i==5)
              {
                TimeC108[7*i+6]=((U16)CurrentTime[i+1]<<8)+CurrentTime[i]; 
              }
              else
              {
                TimeC108[7*i+6]=CurrentTime[i];
              }
                
          }
          TimeC108[35]=0x4084;//年的显示控制
          YADA_C0(RX8025C108Addr,TimeC108,42);
          YADA_C108(RX8025C108Addr,6);
          
          YADA_98 (70,5,0x20,0x82,0x02,0xffff,0x0000,"-",2);
          YADA_98 (106,5,0x20,0x82,0x02,0xffff,0x0000,"-",2);
          YADA_98 (178,5,0x20,0x82,0x02,0xffff,0x0000,":",2);
          YADA_98 (214,5,0x20,0x82,0x02,0xffff,0x0000,":",2);  
        }
        else
        { 
          U8 CurrentTime;
          CurrentTime=date_sf.SECOND;
          U16 sC108[]= {0x2084,214,5,0xffff,0x0000,0x0000,CurrentTime};
          
          YADA_C0(RX8025C108Addr+35,sC108,7);
          YADA_C108(RX8025C108Addr+35,1);
          YADA_98 (214,5,0x20,0x82,0x02,0xffff,0x0000,":",1);
        }
  }
}
/*******************************************************************************
** Function Name	：DisTimeFlg
** Input		：
** Return		：
** Author		：WK
** Version	：
** Date		：
** Dessription	：时间显示的标志位设置，根据“分”是否变化，判断是否全部显示时间还是只显示秒
** Reverse	：
*******************************************************************************/
void DisTimeFlg()
{
    TIME_STRUCT             time_sf;
    DATE_STRUCT             date_sf;
    _time_get(&time_sf);
    _time_to_date(&time_sf,&date_sf);
    
    if(MinuteOld!=date_sf.MINUTE)
    {
       MinuteOld=date_sf.MINUTE;
       TimeFlg=1;
    }
    else
       TimeFlg=2;
}

/*FUNCTION**********************************************************************
 *
 * Function Name    : print_mqx_time
 * Returned Value   :
 * Comments         :
 *
 *END**************************************************************************/
static void print_mqx_time
(
        DATE_STRUCT_PTR     time_rtc,
        TIME_STRUCT_PTR     time_mqx
)
{
    printf ("MQX : %d s, %d ms (%02d.%02d.%4d %02d:%02d:%02d )\n", time_mqx->SECONDS, time_mqx->MILLISECONDS,time_rtc->DAY, time_rtc->MONTH, time_rtc->YEAR, time_rtc->HOUR, time_rtc->MINUTE, time_rtc->SECOND);
}

/*FUNCTION**********************************************************************
 *
 * Function Name    : print_current_time
 * Returned Value   :
 * Comments         :
 *
 *END**************************************************************************/
static void print_current_time(void)
{
    DATE_STRUCT     time_rtc;
    TIME_STRUCT     time_mqx;

    _rtc_get_time_mqxd (&time_rtc);
    _time_from_date (&time_rtc, &time_mqx);
    print_rtc_time(&time_rtc, &time_mqx);
}

/*FUNCTION**********************************************************************
 *
 * Function Name    : print_rtc_time
 * Returned Value   :
 * Comments         : Print rtc time to terminal
 *
 *END**************************************************************************/

static void print_rtc_time
(
        DATE_STRUCT_PTR     time_rtc,
        TIME_STRUCT_PTR     time_mqx
)
{
    printf ("RTC : %02d.%02d.%4d %02d:%02d:%02d (%02d s, %03d ms)\n", time_rtc->DAY, time_rtc->MONTH, time_rtc->YEAR, time_rtc->HOUR, time_rtc->MINUTE, time_rtc->SECOND, time_mqx->SECONDS, time_mqx->MILLISECONDS);

}