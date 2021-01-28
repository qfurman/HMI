#include <inarm.h>
#include <iolpc2138.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
//#include "pict1.h"
#include "fonts.h"
#include "timers.h"
#include "processing.hpp"

#include "objects.hpp"
#include "calendar.hpp"
#include "rtc.h"



/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
num_fild num[]=
{
    num_fild (num_fild::no_change, num_fild::no_change,  180, 0,  &fixedsys[0], true, "%#5.2f", 9),
    num_fild (num_fild::no_change, num_fild::no_change,  180, 20, &fixedsys[0], true, "%#7.2f", 0),
    num_fild (num_fild::no_change, num_fild::no_change,  30,  26, &fixedsys[0], true, "%4.0f", YEAR),
    num_fild (num_fild::month,     num_fild::month_back, 30,  9,  &fixedsys[0], true, "%2.0f", MONTH),
    num_fild (num_fild::no_change, num_fild::time_back,  160, 9,  &fixedsys[0], true, "%02d:%02d:%02d", 0, true),
};
/*****************************************************************************/
sheet_keys main_keys[8] =
{
    #define yy 215
    #define m (2*3*8+2)

    sheet_keys( 0,    yy ,5,24,&arial24_digit[m*1]),
    sheet_keys( 5,    yy ,5,24,&arial24_digit[m*2]),
    sheet_keys(10,    yy ,5,24,&arial24_digit[m*3]),
    sheet_keys(15,    yy ,5,24,&arial24_digit[m*4]),
    sheet_keys(20,    yy ,5,24,&arial24_digit[m*5]),
    sheet_keys(25,    yy ,5,24,&arial24_digit[m*6]),
    sheet_keys(30,    yy ,5,24,&arial24_digit[m*7]),
    sheet_keys(35,    yy ,5,24,&wingdings22_clock[0]),
};
/*****************************************************************************/
/*****************************************************************************/
#define m (2*3*8+2)
#define a 15
#define b 70
control_keys num_buttons[] =
{
    control_keys(control_keys::zero, a+3,    b+72 ,3,24,&arial24_digit[m*0]),
    control_keys(control_keys::one,  a,      b    ,3,24,&arial24_digit[m*1]),
    control_keys(control_keys::two,  a+3,    b    ,3,24,&arial24_digit[m*2]),
    control_keys(control_keys::three,a+6,    b    ,3,24,&arial24_digit[m*3]),
    control_keys(control_keys::four, a,      b+24 ,3,24,&arial24_digit[m*4]),
    control_keys(control_keys::five, a+3,    b+24 ,3,24,&arial24_digit[m*5]),
    control_keys(control_keys::six,  a+6,    b+24 ,3,24,&arial24_digit[m*6]),
    control_keys(control_keys::seven,a,      b+48 ,3,24,&arial24_digit[m*7]),
    control_keys(control_keys::eight,a+3,    b+48 ,3,24,&arial24_digit[m*8]),
    control_keys(control_keys::nine, a+6,    b+48 ,3,24,&arial24_digit[m*9]),

    control_keys((control_keys::close), a,      b+72 ,3,24,&cancel[0]),
    control_keys((control_keys::ok),    a+6,    b+72 ,3,24,&ok[0]),
    control_keys((control_keys::up),    a+9,    b    ,3,24,&arrow_up[0]),
    control_keys((control_keys::down),  a+9,    b+24 ,3,24,&arrow_down[0]),
    control_keys((control_keys::right), a+9,    b+48 ,3,24,&arrow_right[0]),
    control_keys((control_keys::left),  a+9,    b+72 ,3,24,&arrow_left[0]),
};
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
// ініціалізація часового рядка в калкндарі

void sys_time_init()
{
    char*time_ptr;
    time_ptr = (char*)&num[4].number;

    *time_ptr++ = SEC;
    *time_ptr++ = MIN;
    *time_ptr = HOUR;
}
/*****************************************************************************/
/*****************************************************************************/
// запис нового часу і дати
void new_time()
{
    char*time_ptr;
    time_ptr = (char*)&num[4].number;

    YEAR = num[2].number;
    MONTH = num[3].number;
    DOM = day_now;

    DOY = dayOfYear(DOM, MONTH, YEAR);
    DOW = dayOfWeek(DOY, YEAR)-1;

    SEC = *time_ptr++;
    MIN = *time_ptr++;
    HOUR = *time_ptr;
    control_keys::close_window();
}
/*****************************************************************************/
void going_idle()
{
/*
    PCONP_bit.PCTIM0 = 1;
  PCONP_bit.PCTIM1 = 1;
  PCONP_bit.PCURT0 = 1;
  PCONP_bit.PCURT1 = 1;
  PCONP_bit.PCPWM0 = 1;
  PCONP_bit.PCI2C = 1;
  PCONP_bit.PCSPI = 1;
  __disable_interrupt();

  PCON_bit.IDL = 1;
    */
}
/*****************************************************************************/
control_keys time_keys[] =
{
    control_keys(control_keys::close_window, a+10, b+90, 3, 24, &cancel[0]),
    control_keys(new_time,                   a+14, b+90, 3, 24, &ok[0]),
};
/*****************************************************************************/
control_keys keys_6[] =
{
    control_keys(control_keys::close_window, a+5,  b+78, 3, 24, &cancel[0]),
    control_keys(going_idle,                   a+9, b+78, 3, 24, &ok[0]),
};
/*****************************************************************************/
/*****************************************************************************/
text win_6[] =
{
    text(24, 5,  &fixedsys[0],"Перейти в режим"),
    text(40, 5,  &fixedsys[0],"енергозбереження чи ні?"),
};
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
form window_[] =
{
//    form(char sheet_, num_fild*param_, char param__, control_keys*button_, char button__, form_f funct_ptr_=0, char funct_=0, int rows_=0, int columns_=0, int hights_=0, int widths_=0, text*_txt=0, char txt__=0);
    form(6, 0, 0, keys_6, sizeof(keys_6)/sizeof(control_keys), 0, 0, 16, 1, 160, 27, win_6, sizeof(win_6)/sizeof(text)),
    form(7, &num[2], 3, time_keys, 2, one_month, 1, 26, 6, 160, 27),
    //вікно з клавіатурою для вводу
    form(128, 0, 0, num_buttons, sizeof(num_buttons)/sizeof(control_keys), control_keys::prepare_input, 1, b-4*8, a-1, 17*8, 14),
};
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

void sheet_view()
{
    char i,k;

    for(i=0;i<sh_seq.num;i++)
    {
        for(k=0;k<sizeof(window_)/sizeof(form);k++)
        {
            if(window_[k].sheet == sh_seq.seq[i])
            {
                form::refresh(&window_[k]);
                break;
            }
        }
    }
}
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
void sheet_captions()
{
    char i;
    bool highlight;
    //if(press_flag)return;

    for(i=0;i<sizeof(main_keys)/sizeof(sheet_keys);i++)
    {
        if((sheet_keys::sheet != 128)&&(tolerance(main_keys[i].x,main_keys[i].y,main_keys[i].width,main_keys[i].hight)))
        {
            press_flag = true;  //блокування клавіш на 100mS
            sheet_keys::sheet = i;  // дана сторінка робиться активною
            char ii;
            //превірка чи вже є таке вікно, якщо є, то премістити його в кінець послідовності
            for(ii=0;ii<sh_seq.num;ii++)
            {
                if((sh_seq.seq[ii]==i)&&(ii != (sh_seq.num - 1)))
                {
                    char l;
                    for(l=ii;l<sh_seq.num-1;l++)sh_seq.seq[l] = sh_seq.seq[l+1];
                    sh_seq.seq[l] = i;
                    break;
                }
            }
            // якщо немає то вставити
            if(sh_seq.seq[sh_seq.num-1] != i)
            {
                sh_seq.seq[sh_seq.num] = i;
                sh_seq.num++;
            }
         }
        // виділення активної сторінки
        if(i == sheet_keys::sheet)highlight = true;
        else highlight = false;
        // генерація клавіші
        create_key(main_keys[i].width, main_keys[i].hight, highlight, main_keys[i].pict);
        lcd_buf_write_array(main_keys[i].x, main_keys[i].y, &keys[0], main_keys[i].hight, main_keys[i].width, 0, false);
    }
}
/*****************************************************************************/
/*****************************************************************************/

