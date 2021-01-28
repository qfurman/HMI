#include "iolpc2138.h"
#include "type.h"
#include "lcd.h"
#include "fonts.h"
#include <stdio.h>
#include <string.h>

#include "objects.hpp"
#include "processing.hpp"
#include "keys.hpp"

#define StartYear 1
#define FirstDayOfStartYear 1
#define FirstYearIsLeap 0

//зміщення календаря
#define offsetRow       50
#define offsetColumn    9



//тривалість місців
char day_tab[2][13] =
{
    { 0,31,28,31,30,31,30,31,31,30,31,30,31} ,
    { 0,31,29,31,30,31,30,31,31,30,31,30,31}
};
//"сьогоднішній" день
char day_now = DOM;

//визначення дня тижня
char dayOfWeek(int dayOfYear, int year)
{
     int iNumberOfLeap;
     int week_day;

     year -= StartYear;
     iNumberOfLeap = year/4 - year/100 + year/400 + FirstYearIsLeap;
     week_day = (year + iNumberOfLeap + FirstDayOfStartYear +
                       (dayOfYear-1)) % 7;
     if (week_day == 0) week_day = 7;


     return week_day;
}

//визначення чи високосний рік
char Leap(int year)
{
    char leap;
    if ((year%4 == 0 && year%100 != 0) || year%400 == 0) leap = 1;
    else leap = 0;
    return leap;
}

//визначення дня року
int dayOfYear(char day, char month, int year)
{
     int dayofyear = 0;
     char ii, leap;

     /* reference Ritchie&Kernighan */
     leap = Leap(year);

     for (ii = 1; ii < month; ii++)
           dayofyear += day_tab[leap][ii];

     dayofyear += day;

     return dayofyear;
}


//генерування календаря на один місяць
void one_month()
{
    char month = num[3].number;
    int year = num[2].number;
    char dayOFweek, day=1, k, l;
    char dayINmonth;
    char l_str[40];
    bool highlight;
    num_fild*cap;

    //вікнo
    //lcd_buf_window(offsetRow-3*8, offsetColumn-3 ,20*8, 27);

    strcpy(l_str," П  В  С  Ч  П  С  Н ");
    set_font(fixedsys);
    lcd_buf_put_text(offsetRow, offsetColumn, l_str, 0, true);

    dayOFweek = dayOfWeek(dayOfYear(1, month, year), year);
    dayINmonth = day_tab[Leap(year)][month];

    for(k=1; k<6; k++)
    {
        for(l=1; l<8; l++)
        {
            if(((k==1)&&(l < dayOFweek))||(day > dayINmonth))
            {
                strcpy(l_str,"   ");
                highlight = false;
            }
            else
            {
                cap = new num_fild(num_fild::no_change, num_fild::no_change,  k*18+offsetRow, (l-1)*3+offsetColumn,  &fixedsys[0], true, "%2.0f", day);

                if((!control_keys::touch_block)&&(!control_keys::out_from_change))highlight = cap->hit_test();

                if(highlight||(day == day_now))
                {
                    day_now = day;
                    cap->select = true;
                }
                else cap->select = false;
                cap->text();
                day++;
            }
            //lcd_buf_put_text(k*18+offsetRow, (l-1)*3+offsetColumn, l_str, 0, highlight);
            delete cap;

        }
    }

}


