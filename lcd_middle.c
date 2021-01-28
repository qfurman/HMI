
#include <ctype.h>

#include "fonts.h"
#include "lcd_middle.h"
#include "lcd.h"



char LCD_str[512];
char LCD_input_str[48];
char LCD_input_str_[48];
char *LCD_crsr_position;

char keys[32*40];


void LCD_put_str(char row, char column,const char *string)
{
    set_font(DefaultFont);
    lcd_buf_put_text(row*(*(DefaultFont+1)*8),*DefaultFont * column,string,0,false);
}

void LCD_put_str(char row, char column,const char *string, bool inv)
{
    set_font(DefaultFont);
    lcd_buf_put_text(row*(*(DefaultFont+1)*8),*DefaultFont * column,string,0,inv);
}

void LCD_put_str(char row, char column,const char *string,const char *font)
{
    fnt_ptr = font;
    lcd_buf_put_text(row*(*(font+1)*8),*font * column,string,0,false);
}

void LCD_put_str(char row, char column,const char *string,const char *font, bool inv)
{
    fnt_ptr = font;
    lcd_buf_put_text(row*(*(font+1)*8),*font * column,string,0,inv);
}
char LCD_init_crsr_position_right(void)
{
    LCD_crsr_position = LCD_input_str + 40 / *DefaultFont-1; //на кінець
    do{
        if(isdigit(*LCD_crsr_position) || (*LCD_crsr_position >= 'A' && *LCD_crsr_position <= 'F'))return 1;
    }while(LCD_crsr_position--);

    return 0;
}

void LCD_highlight(char row, char column)
{
    char *ptr;
    char width;

    width = 320/(*(DefaultFont)*8);
    ptr = LCD_input_str_;

    do{
        *ptr = '';
        ptr++;  
    }while(--width);
    ptr = '\0';
    fnt_ptr = DefaultFont;
    lcd_buf_put_text(row*(*(DefaultFont+1)*8),column,LCD_input_str_,3,false);//xor
}

void LCD_highlight(char row, char column, char *ptr)
{
    fnt_ptr = DefaultFont;
    lcd_buf_put_text(row*(*(DefaultFont+1)*8),column,ptr,3,false);//xor
}
/*
void LCD_highlight_crsr(char row, char column, char*str)
{
    fnt_ptr = DefaultFont;
    layer = 1;
    lcd_put_text(row*(*(DefaultFont+1)*8),column, str);
    layer = 0;
}

void place_crsr(char row, char pos_flash) //номер рядка і кількість позицій що моргають
{
    char *ptr;
    char i;

    //заповнити рядок пробілами
    ptr = &LCD_input_str_[0];
    i = 20;
    do{
        *ptr=' ';
        ptr++;
    }while(--i);
    *ptr = '\0';

    char *ptr_flash = LCD_crsr_position - &LCD_input_str[0] + &LCD_input_str_[0];//початкова возиція поморгування
    do *ptr_flash++ = '';
    while(--pos_flash);
    LCD_highlight_crsr(row,0,LCD_input_str_);
}
*/
void create_fill(char width,char hight)
{
    char k,l,fill;

    for(l=0;l<width;l++)
    {
        for(k=0;k<hight;k++)
        {
            if(k==0 || k==hight-1)fill = 0xff;
            else
                if(l>0 && l<width-1)fill = 0x00;
                else
                {
                    if(l)fill = 0x01;
                    else fill = 0x80;
                }
            if((l==0 || l==width-1) && (k==0 || k==hight-1))fill = 0x00;
            if((k==0 || k==hight-1) && l==0)fill = 0x3f;
            if((k==1 || k==hight-2) && l==0)fill = 0x40;
            if((k==0 || k==hight-1) && l==width-1)fill = 0xfc;
            if((k==1 || k==hight-2) && l==width-1)fill = 0x02;

            keys[k+l*hight] = fill;

        }
    }
}



