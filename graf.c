#include <math.h>
#include "fonts.h"
#include "menu.hpp"
#include "main.hpp"
#include "spi.h"
#include "lcd.h"
#include "lcd_middle.h"
#include "timers.h"
#include "defines.h"
#include "touch.h"
#include "modbus.h"
#include "LPC_Rtc.h"
#include "calc.h"
/*
struct ARCH_VIEW{
    int addr;//адреса у флеш крок 256
    union{
        unsigned int start_channal;//початковий канал
        unsigned int channal;//канал для графіків
    };
    int cursor;//позиція курсору на графіку

    REC rec[15][16];//на графік виділено область розміром 260 х 198 24*11 = 264
};
typedef struct ARCH_VIEW ARCH_VIEW;
*/

/*===============================================================================================*/
void flash_arch_disp_g()
{

    double dg;
    char y0,y1;
    char k,l;
    int P;

/*
    for(i=0;i<240;i++){
        y1 = abs(99*(1.0 - sin(2.0*3.14/240*i)))+1;
        if(i)lcd_buf_DrawLine(i-1,y0,i,y1,true);
        y0 = y1;
    }
*/

    for(k=0;k<15;k++){
        for(l=0;l<16;l++)
        {
            switch(one.channal){
              case 0:P = one.rec[k][l].Ggas;break;
              case 1:P = (int)one.rec[k][l].Gair *10;break;
              case 2:P = one.rec[k][l].Tgasiv;break;
              case 3:P = one.rec[k][l].Tsum;break;
              case 4:P = one.rec[k][l].Tfider;break;
              case 5:P = one.rec[k][l].curr;break;
              case 6:P = one.rec[k][l].Vbr;break;
            }
            if(P <= N.full[one.channal]){
                dg = ((float)(N.full[one.channal] - N.nill[one.channal]) - (float)(N.full[one.channal] - P)) / (float)(N.full[one.channal] - N.nill[one.channal]);
                if(dg < 0) dg = 0;
                if(dg > 1) dg = 1;
                y1 = (int)(198.0 * (1-dg));
            }else{
                if(P <= N.nill[one.channal])y1 = 198;
                else{
                    if(P == 32767)y1 = 198;//обрив
                    else y1 = 0;
                }
            }
            if(y1 > 198)y1 = 198;
            //if(y1 < 0)y1 = 0;
            if(one.rec[k][l].time == 0)break;//якщо більше нема записів і йде чисте місце то вийти
            if(k || l)lcd_buf_DrawLine(k*16+l, y0, k*16+l+2, y1,true);//якщо не перша точка
            y0 = y1;
        }
    }

    if(k == 15 && l== 16)
        lcd_buf_DrawLine(240, y0, 241, y1,true);//якщо остання точка

    //нульова лінія
    dg = ((float)(N.full[one.channal] - N.nill[one.channal]) - N.full[one.channal])/(float)(N.full[one.channal] - N.nill[one.channal]);
    if(dg<0)dg=0;
    if(dg>1)dg=1;
    y1 = (int)(198.0 * (1-dg));


    int i;
    for(i=0;i<241;i=i+5){
        lcd_buf_line(i,y1,i+2,y1,2);
    }


    sprintf(LCD_str,"0");
    LCD_put_str((int)( 12*(1-dg) ),31,LCD_str);

    //bar
    lcd_buf_line(0,0,241,0,true);
    lcd_buf_line(241,0,241,199,true);
    lcd_buf_line(0,199,241,199,true);
    lcd_buf_line(0,0,0,199,true);

    if(time_10mS > 15){
        //cursor
        lcd_buf_line(one.cursor+1,1,one.cursor+1,198,true);
    }

    char j;
    //права шкала
    for(j=0;j<=N.scal[one.channal].big*N.scal[one.channal].small;j++){
        if(j%N.scal[one.channal].small==0)lcd_buf_DrawLine(241,j*198/(N.scal[one.channal].big*N.scal[one.channal].small),247,j*198/(N.scal[one.channal].big*N.scal[one.channal].small),true);
        else lcd_buf_DrawLine(241,j*198/(N.scal[one.channal].big*N.scal[one.channal].small),245,j*198/(N.scal[one.channal].big*N.scal[one.channal].small),true);//права шкала
    }

    //лідписуєм графік
    k = one.cursor / 16;
    l = one.cursor % 16;

    LPC_Rtc_DateTime_t Dt;

    time_UNpack(one.rec[k][l].time, &Dt);

    switch(one.channal){
      case 0:P = one.rec[k][l].Ggas;break;
      case 1:P = (int)one.rec[k][l].Gair * 10;break;
      case 2:P = one.rec[k][l].Tgasiv;break;
      case 3:P = one.rec[k][l].Tsum;break;
      case 4:P = one.rec[k][l].Tfider;break;
      case 5:P = one.rec[k][l].curr;break;
      case 6:P = one.rec[k][l].Vbr;break;
    }

    //дата час і параметр
    if(Dt.day>31 || Dt.month>12 || Dt.year>4096 || Dt.hour>23 || Dt.minute>59|| Dt.second>59){
        sprintf(LCD_str,"ПОМИЛКА!");LCD_put_str(2,31,LCD_str, fixedsys);
    }else{
        sprintf(&LCD_str[0],"%02d.%02d",(int)Dt.day, (int)Dt.month);LCD_put_str(2,31,LCD_str, fixedsys);
        sprintf(&LCD_str[0],"     %04d",(int)Dt.year);LCD_put_str(3,31,LCD_str, fixedsys);
        sprintf(&LCD_str[0],"%02d:%02d:%02d",(int)Dt.hour, (int)Dt.minute, (int)Dt.second);LCD_put_str(4,32,LCD_str, fixedsys);
    }
    if(P != 32767){
        switch(one.channal){
          case 0:P = one.rec[k][l].Ggas;
          sprintf(&LCD_str[0],"Газ"); LCD_put_str(6,31,LCD_str);
          sprintf(&LCD_str[0],"%4.1fмЗ",(float)P/10);LCD_put_str(7,31,LCD_str);
          break;
          case 1:P = one.rec[k][l].Gair;
          sprintf(&LCD_str[0],"Повітря"); LCD_put_str(6,31,LCD_str);
          sprintf(&LCD_str[0],"%3dмЗ",P);LCD_put_str(7,31,LCD_str);
          break;
          case 2:P = one.rec[k][l].Tgasiv;
          sprintf(&LCD_str[0],"Дим");LCD_put_str(6,31,LCD_str);
          sprintf(&LCD_str[0],"%7.1f\260C", (float)P/10);LCD_put_str(7,31,LCD_str);
          break;
          case 3:P = one.rec[k][l].Tsum;
          sprintf(&LCD_str[0],"Суміш");LCD_put_str(6,31,LCD_str);
          sprintf(&LCD_str[0],"%7.1f\260C", (float)P/10);LCD_put_str(7,31,LCD_str);
          break;
          case 4:P = one.rec[k][l].Tfider;
          sprintf(&LCD_str[0],"Фідер");LCD_put_str(6,31,LCD_str);
          sprintf(&LCD_str[0],"%7.1f\260C",(float)P/10);LCD_put_str(7,31,LCD_str);
          break;
          case 5:P = one.rec[k][l].curr;
          sprintf(&LCD_str[0],"Струм");LCD_put_str(6,31,LCD_str);
          sprintf(&LCD_str[0],"%7.2f А", (float)P/100);LCD_put_str(7,31,LCD_str);
          break;
          case 6:P = one.rec[k][l].Vbr;
          sprintf(&LCD_str[0],"Вібратор");LCD_put_str(6,31,LCD_str);
          sprintf(&LCD_str[0],"%7.2fC", (float)P/100);LCD_put_str(7,31,LCD_str);
          break;
        }

    }
    else{
        sprintf(&LCD_str[0],"######");
        LCD_put_str(7,31,LCD_str, fixedsys);
    }

    //діапазо значень на графіку
    sprintf(LCD_str,"%d",N.full[one.channal]/10);
    LCD_put_str(0,31,LCD_str);

    sprintf(LCD_str,"%d",N.nill[one.channal]/10);
    LCD_put_str(12,31,LCD_str);
    
    if(encB==0){
      sprintf(LCD_str,"параметр");
      LCD_put_str(8,31,LCD_str);
    }else{
      sprintf(LCD_str,"<-час-> ");
      if(!force)LCD_put_str(8,31,LCD_str);
      else {
        sprintf(LCD_str,"<-ЧАСx8->");
        LCD_put_str(8,31,LCD_str,true);
        
      }
    }

/*
    sprintf(&LCD_str[0],"k=%2d l=%2d ",k,l);
    LCD_put_str(27,0,LCD_str, arial);
*/
    /*
    //виймаємо попередження
    att_min = one.record[k].rec[l].att[0] | (((int)one.record[k].rec[l].att[1]&0xf)<<8);
    att_max = (one.record[k].rec[l].att[2]<<4) | (((int)one.record[k].rec[l].att[1]>>4)&0xf);
    //виймаємо аварії
    alr_min = one.record[k].rec[l].alr[0] | (((int)one.record[k].rec[l].alr[1]&0xf)<<8);
    alr_max = (one.record[k].rec[l].alr[2]<<4) | (((int)one.record[k].rec[l].alr[1]>>4)&0xf);

    sprintf(&LCD_str[0],"Канал %2d",one.channal+1);
    if(att_min & (1<<one.channal))sprintf(&LCD_str[8]," ПРЕДУПРЕЖД! T<Tmin");
    if(att_max & (1<<one.channal))sprintf(&LCD_str[8]," ПРЕДУПРЕЖД! T>Tmax");
    if(alr_min & (1<<one.channal))sprintf(&LCD_str[8]," АВАРИЯ! T<Tmin");
    if(alr_max & (1<<one.channal))sprintf(&LCD_str[8]," АВАРИЯ! T>Tmax");

    LCD_put_str(14, 0,LCD_str, fixedsys);
 */
}
/*===============================================================================================*/
/*------------------------------------------------------------------------------------------------/
/   функція на підміну для графіків         GRAF GRAF
/------------------------------------------------------------------------------------------------*/
void flash_arch_last()
{
    do{
        spi1_rd_status_flash(one.addr);
        while(spi1_sts_bit.end_rx == 0);
    }while(spi1_status_rg&0x01);//while write in progress(WIP)

    rd_flash(one.addr,&one.rec, 256*14);
    while(spi1_sts_bit.end_rx == 0);

    //копіюємо в останній запис той щапис що формується
    rd_rmtrn(a_mng_rmtrn_addr + 8, &one.rec[14], 256); //+8 щоб перескочити адресу і кількість
    while(spi1_sts_bit.end_rx == 0);
}
//------------------------------------------------------------------------------------------------/

void flash_arch_read()
{
    if(N.archive_addr < arch_flash_addr){
        do{
            spi1_rd_status_flash(one.addr);
            while(spi1_sts_bit.end_rx == 0);
        }while(spi1_status_rg&0x01);//while write in progress(WIP)

        rd_flash(one.addr,&one.rec, 256*15);
        while(spi1_sts_bit.end_rx == 0);
    }
    else flash_arch_last();

}
//------------------------------------------------------------------------------------------------/
void flash_arch_read_g()
{
    do{
        spi1_rd_status_flash(one.addr);
        while(spi1_sts_bit.end_rx == 0);
    }while(spi1_status_rg&0x01);//while write in progress(WIP)

    rd_flash(one.addr,&one.rec, 256*15);
    while(spi1_sts_bit.end_rx == 0);
}
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/




