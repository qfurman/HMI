#include "type.h"
#include "fonts.h"
#include "lcd_middle.h"
#include "lcd.h"
#include "spi.h"

/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
typedef struct{
                            // координати верхнього лівого кута клавіші
    char x,y;               // 0 <= x <= 39, 0 <= y <= 239
    char width,hight;       // ширина і висота в байтах
    char const *pict;       // вказівник на внутрішній напис у клавіші
}CAPTION;

/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/

bool touched;// ознака того що це натиснення вже усвідомленно

/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#define yy 215
#define m (2*3*8+2)
/*
const CAPTION main_keys[] =
{
    { 2, yy ,5,24,&arrow_left[0]},
    { 8, yy ,5,24,&arrow_up[0]},
    {14, yy ,5,24,&arrow_down[0]},
    {20, yy ,5,24,&arrow_right[0]},
    {27, yy ,5,24,&ent[0]},
    {33, yy ,5,24,&mnu[0]},
   // sheet_keys(30,    yy ,5,24,&arial24_digit[m*7]),
   // sheet_keys(35,    yy ,5,24,&wingdings22_clock[0]),

};
*/

/*****************************************************************************/

void create_key(char width,char hight,bool inv,const char *charact)
{
    char k,l,fill;
    char x,y;   //ширина і висота внутрішнього зображення в х - байтах
    char corr_x,corr_y;
    char hor_l,hor_r,ver_u,ver_d;

    if(charact)
    {
        x = *charact++;
        y = *charact++;

        corr_x = (width - x)/2;
        corr_y = (hight - y)/2;

        hor_l = width - x - corr_x;
        hor_r = width - corr_x;
        ver_u = hight - y - corr_y;
        ver_d = hight - corr_y;
    }

    for(l=0;l<width;l++)
    {
        for(k=0;k<hight;k++)
        {
            if(k==1 || k==hight-2)fill = 0xff;
            else
                if(l>0 && l<width-1)fill = 0x00;
                else
                {
                    if(l)fill = 0x02;
                    else fill = 0x40;
                }
            if((l==0 || l==width-1) && (k==0 || k==hight-1))fill = 0x00;
            if((k==1 || k==hight-2) && l==0)fill = 0x7f;
            if((k==1 || k==hight-2) && l==width-1)fill = 0xfe;

            if((l>=hor_l)&&(l<hor_r)&&(k>=ver_u)&&(k<ver_d))
            {
                if((width - x)%2)
                {
                    if(l<hor_r-1)
                    {
                        keys[k+(l+1)*hight] = 0xf0 & *charact<<4;
                        keys[k+l*hight] |= fill ^ (*charact++ >> 4);
                    }
                    else keys[k+l*hight] |= fill;
                }
                else keys[k+l*hight] = fill ^ *charact++;
            }
            else
            {
                if((charact)&&(((width - x)%2)&&(l>=hor_l-1)&&(l<hor_r-1)&&(k>=ver_u)&&(k<ver_d)))
                {
                    keys[k+(l+1)*hight] = 0xf0 & *charact<<4;
                    keys[k+l*hight] = fill ^ *charact++ >> 4;
                }
                else keys[k+l*hight] = fill;
            }
            if(inv) keys[k+l*hight] = ~keys[k+l*hight];
        }
    }
}
/*****************************************************************************/
C_XY c_xy;//структура з коородинатами
/*****************************************************************************/
//діапазон значень координат в кодат АЦП
#define min_x 200
#define min_y 300
#define max_x 3850
#define max_y 3750
    int x,y;

bool tolerance(int x_, int y_, int width, int hight)
{

    x = (c_xy.x - min_x) * 320 / (max_x - min_x); //320
    y = 240 - (c_xy.y - min_y) * 240 / (max_y - min_y);//240

    if(x<0) return false;
    if(y<0) return false;
    if(x>320) return false;
    if(y>240) return false;

    x_= x_ * 8;
    width = width * 8;//аперевести з байтів в координати

    if(x>x_)
        if(x<(x_ + width))
            if(y>y_)
                if(y<(y_ + hight))return(true);
    return(false);
}
/*****************************************************************************/
KAY_STACK kays; //організація клавіатурного стека

KAY_REPEAT kay_temp={0,0,0};
const KAY_REPEAT kay_set={10,5,0};//зразкові інтервали перша *20мс всі наступні *20мс

/******************************************************************************\
*   запис однієї кнопки в буфер
*   якщо буфер переповнений то вийти
\******************************************************************************/
void kays_push(unsigned char kay_)
{
    if(kays.index == kays_buf_size)return;
    kays.buf[kays.index++] = kay_;
}

/******************************************************************************\
*   виймання однієї кнопки з буфера
*   якщо буфер порожній результат 0
\******************************************************************************/
char kays_pop(void)
{
    unsigned char kay_;

    if(kays.index == 0)return 0;
    kay_ = kays.buf[--kays.index];
    kays.buf[kays.index] = 0;
    return kay_;
}
/******************************************************************************\
*   ініціалізація клавіатури
*   запускати після ініціалізації дисплея
\******************************************************************************/

void kays_init(void)
{
    kays.index = 0;         //init kays stack
}
/******************************************************************************\
* організації роботи клавіатури
\******************************************************************************/
unsigned char last_kay;
unsigned short tick;
bool repeat;
/******************************************************************************/
/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
//char touch_cntr;//щоб усунути хибні спрацювання
/*
bool highlight;

void sheet_captions()
{
    char i;
    char new_kay;//нова клавіша від 1 і далі
    //if(press_flag)return;

    for(i=0;i<sizeof(main_keys)/sizeof(CAPTION);i++){
        if(((Z1>128)&&(Z2<3968))&&tolerance(main_keys[i].x,main_keys[i].y,main_keys[i].width,main_keys[i].hight)){
            highlight = true;

            new_kay = i+1;
            if(new_kay != last_kay){
                //if(++touch_cntr > 2)
                {
                    kays_push(new_kay);
                    last_kay = new_kay;
                }
            }
            else{
                //touch_cntr = 0;
                if(kay_temp.first < kay_set.first){
                    kay_temp.first++;
                    repeat = false;
                }
                else{
                    if(kay_temp.all_next < kay_set.all_next){
                        kay_temp.all_next++;
                        kays_push(new_kay);
                    }
                    else{
                        if(kay_temp.all_next_next == kay_set.all_next_next){
                            kay_temp.all_next_next = 0;
                            repeat = true;
                            kays_push(new_kay);
                        }
                        else kay_temp.all_next_next++;
                    }
                }
            }

        }
        else{
            highlight = false;
            if(i == last_kay - 1){
                last_kay = 0;
                kay_temp.first = 0;
                kay_temp.all_next = 0;
                kay_temp.all_next_next = 0;
            }
        }
        // генерація клавіші
        create_key(main_keys[i].width, main_keys[i].hight, highlight, main_keys[i].pict);
        lcd_buf_write_array(main_keys[i].x, main_keys[i].y, &keys[0], main_keys[i].hight, main_keys[i].width, 0, false);
    }
}
*/
/*****************************************************************************/
void sheet_captions()
{
    char new_kay;//нова клавіша від 1 і далі

    if(skey){
      new_kay = skey;
      if(new_kay != last_kay){
          kays_push(new_kay);
          last_kay = new_kay;
      }
      else{
        if(kay_temp.first < kay_set.first){
          kay_temp.first++;
          repeat = false;
        }
        else{
          if(kay_temp.all_next < kay_set.all_next){
            kay_temp.all_next++;
            kays_push(new_kay);
          }
          else{
            if(kay_temp.all_next_next == kay_set.all_next_next){
              kay_temp.all_next_next = 0;
              repeat = true;
              kays_push(new_kay);
            }
            else kay_temp.all_next_next++;
          }
        }
      }
    }
    else{
      if(skey != last_kay){
        last_kay = 0;
        kay_temp.first = 0;
        kay_temp.all_next = 0;
        kay_temp.all_next_next = 0;
      }
    }
    
}
/*****************************************************************************/
