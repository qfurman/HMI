#include <stdio.h>
#include "iolpc2138.h"
#include "type.h"
#include "menu.hpp"
#include "ramtron.h"

#include "spi.h"
#include "timers.h"

const int polynom_1 = 0x12090178;
const int polynom_2 = 0x871a9a21;

int minute;

/*
**===========================================================================
**
**===========================================================================
*/

int crypt(int inp, int polynom)
{
    char i;

    for(i=0;i<32;i++){
        inp = inp ^ (polynom << i);
    }
    return inp;
}
/*------------------------------------------------------------------------------------*/

int uncrypt(int inp, int polynom)
{
    int i;

    for(i=31;i>=0;i--){
        inp = inp ^ (polynom << i);
    }
    return inp;
}
/*------------------------------------------------------------------------------------*/
void cypt_init()
{
    if(N.seq.flag != 0x12345678){
        //значить число для авторизації не згенеровано
        N.seq.seed = CTC_bit.COUNTER;
        N.seq.seed += CTC_bit.COUNTER;
        N.seq.seed *= CTC_bit.COUNTER;
        N.seq.seed += YEAR;
        N.seq.seed += MONTH;
        N.seq.seed += HOUR;
        N.seq.seed += MIN;
        N.seq.seed += SEC;
        N.seq.seed += CTC_bit.COUNTER;
        if(N.seq.seed){
            N.seq.flag = 0x12345678; //якщо не дорівнює нулю
            N.seq.time = 0;
            N.seq.etap = 0;
            N.seq.req = 0;
            N.seq.ans[0] = 0;
            N.seq.ans[1] = 0;
            N.seq.flag_ok = 0;
        }

        spi1_init();
        while(spi1_sts_bit.end_rx == 0);

        int addr =(int)&N.seq - (int)&N;
        wr_rmtrn(addr, &N.seq, 8*sizeof(int));
        while(spi1_sts_bit.end_rx == 0);
        rd_rmtrn(addr, &N.seq, 8*sizeof(int));
        while(spi1_sts_bit.end_rx == 0);
    }
}

/*------------------------------------------------------------------------------------*/
void crypt_work()
{
    int tmp, tmp1,tmp2;

    switch(N.seq.etap){
      case 0:
        if(N.seq.flag == 0x12345678){
            //у випадку коли з генерацією все гаразд
            N.seq.req = crypt(N.seq.seed,polynom_1);
            tmp = uncrypt(N.seq.ans[1],polynom_2);
            tmp1 = tmp & 0xf0f0ff0f;
            tmp2 = ((tmp & 0x0f000000)>>16) | ((tmp & 0x000f0000)>>12) | ((tmp & 0x000000f0)>>4);
            //перевіряємо ключ 1, порівнюємо по масці ключ 2 із зерном і превіряємо чи час ненульовий
            if((N.seq.ans[0] == crypt(N.seq.req,polynom_2)) && (tmp1 == (N.seq.seed&0xf0f0ff0f)) && tmp2){
                N.seq.flag_ok = 1;
                N.seq.time = tmp2 * 24 * 60;
                N.seq.etap = 1;

                spi1_init();
                while(spi1_sts_bit.end_rx == 0);

                int addr =(int)&N.seq.time - (int)&N;
                wr_rmtrn(addr, &N.seq.time, 3*sizeof(int));
                while(spi1_sts_bit.end_rx == 0);
                rd_rmtrn(addr, &N.seq.time, 3*sizeof(int));
                while(spi1_sts_bit.end_rx == 0);
            }
            else N.seq.flag_ok = 0;
        }else{
            //коли проблеми з генерацією
            N.seq.seed = 0;
            N.seq.req = 0;
            N.seq.ans[0] = 0;
            N.seq.ans[1] = 0;
            N.seq.flag_ok = 0;
            N.seq.time = 0;
        }
        break;

      case 1:
        if(minute != MIN){
            //якщо час не нульовий і не 0xFFF-вічний код
            if(N.seq.time && (N.seq.time != 4095)){
                N.seq.time--;

                spi1_init();
                while(spi1_sts_bit.end_rx == 0);
                int addr =(int)&N.seq.time - (int)&N;
                wr_rmtrn(addr, &N.seq.time, sizeof(int));
                while(spi1_sts_bit.end_rx == 0);
                rd_rmtrn(addr, &N.seq.time, sizeof(int));
                while(spi1_sts_bit.end_rx == 0);
            }

            //перевіряємо чи закінчився час роботи  і щоб була перша зміна 8-00
            if((N.seq.time == 0)&&(HOUR==8)){
                N.seq.flag = 0;//зняти ознаку генерації зерна
                cypt_init();//генерація нового коду -запиту
            }
        }
        minute = MIN;
        break;
    }
}
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
