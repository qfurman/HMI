//#include "usercode.h"
#include <intrinsics.h>
#include <stdbool.h>
#include "iolpc2138.h"
//#include <inarm.h>

#include "macros.h"
#include "type.h"
#include "timers.h"
#include "fonts.h"

#include    "LPC_SysControl.h"
#include    "LPC2138_sys_cnfg.h"
#include    "LPC_Vic.h"

//port 1
#define RD_wr   25
#define RS	24
//port 0
#define d6	22
#define E   26
#define TOUCH_lcd 31
#define RES_LCD 30

/*****************************************************************************/
#define set_cursot_type         0x5d
#define set_cursor_right        0x4c
#define set_cursor_left         0x4d
#define set_cursor_up           0x4e
#define set_cursor_down         0x4f

#define set_cursor_address      0x46
#define read_cursor_address     0x47
#define write_display_memory    0x42
#define read_display_memory     0x43

#define lcd_set_address(w)  lcd_put_command(set_cursor_address); lcd_put_data(lobyte(w)); lcd_put_data(hibyte(w));
/*****************************************************************************/
char lcd_buf[9600];
/*****************************************************************************/
__arm void lcd_put_command(char command)
{
	set_P1(RS);
    //set_P0(E);
    //FIO0SET3 = 1<<E%8;
    *((char*)0x3FFFC01B) = 1<<E%8;
    /*
	FIO1CLR = 0x00ff0000;
	FIO1SET = (command<<16);
    */

    *((char*)0x3FFFC036) = command;

	//clr_P0(E);
    //FIO0CLR3 = 1<<E%8;
    *((char*)0x3FFFC01F) = 1<<E%8;
	clr_P1(RS);
    //FIO1SET = 0x00ff0000;
    *((char*)0x3FFFC03A) = 0xff;
	
}
/*****************************************************************************/
__arm void lcd_put_data(char data)
{
    //set_P0(E);
    //FIO0SET3 = 1<<E%8;
    *((char*)0x3FFFC01B) = 1<<E%8;

    /*
    FIO1CLR = 0x00ff0000;
	FIO1SET = (data<<16);
    */

    //*((char*)0x3FFFC016) = data;   //FIO1PIN2
    *((char*)0x3FFFC036) = data;

    //clr_P0(E);
    //FIO0CLR3 = 1<<E%8;
    *((char*)0x3FFFC01F) = 1<<E%8;
    //FIO1SET = 0x00ff0000;
    *((char*)0x3FFFC03A) = 0xff;

	
}
/*****************************************************************************/
void lcd_cls(void)
{
unsigned short	i;

	lcd_put_command(0x46);
	lcd_put_data(0x00);
	lcd_put_data(0x00);

//    lcd_put_command(write_display_memory);
//    for(i=0;i<1200;i++)lcd_put_data(i);
/*
	i=9600;
	lcd_put_command(write_display_memory);
	do{
		 lcd_put_data(0xf0);
	}while(--i);
*/
	//i = 23168;
    i=32768;
    lcd_put_command(write_display_memory);
	do
	{
		lcd_put_data(0x00);
	}
	while(--i);

}

/*****************************************************************************/
/*
void lcd_put_font(const char*font,int count,int addr)
{
	lcd_set_address(addr);
	lcd_put_command(write_display_memory);
	do
	{
		lcd_put_data(*font++);
	}
	while(--count);
}
*/
/*****************************************************************************/
typedef struct
{
    unsigned  char fc   :2; //cursor
    unsigned  char fp0  :2; //layer0
    unsigned  char fp1  :2; //layer1
    unsigned  char fp2  :2; //layer2
}disp;
typedef struct
{
    unsigned  char mx   :2; //composition
    unsigned  char dm1  :1; //mode
    unsigned  char dm2  :1; //mode
    unsigned  char ov   :1; //overlay
    unsigned  char un   :3; //unused
}ovlay;

/*****************************************************************************/
//disp display;
union
{
    char display;
    disp disp_bit;
};
union
{
    char overlay;
    ovlay ovlay_bit;
};

void lcd_init(void)
{
    char i;

    //конфігурація ліній В/В
    FIO0DIR |= 1<<E | 1<<TOUCH_lcd | 1<<RES_LCD;
    FIO0SET = 1<<E | 1<<TOUCH_lcd;//ЗАПИСАТИ ОДИНИЦІ
    FIO1DIR |= 0xff<<16 | 1<<RS | 1<<RD_wr;
    FIO1SET = 0xff<<16 | 1<<RS | 1<<RD_wr;//ЗАПИСАТИ ОДИНИЦІ
    //*((char*)0x3FFFC002) = 0xff;//FIO0DIR2

    i = 0;
    //інціалізація ліній В/В
    clr_P0(RES_LCD);//ресетнути дисплей
    clr_P0(E);
    clr_P1(RS);//дефолт - запис читання даних
	clr_P1(RD_wr);//дефолт запис
    while(--i);
    set_P0(RES_LCD);//підняти лінію ресет дисплея
    clr_P0(TOUCH_lcd);

    ovlay_bit.mx = 1;
    ovlay_bit.dm1 = 1;
    ovlay_bit.dm2 = 1;
    ovlay_bit.ov = 1;
    ovlay_bit.un = 0;

    disp_bit.fc = 0;
    disp_bit.fp0 = 1;
    disp_bit.fp1 = 2;
    disp_bit.fp2 = 1;

//SYSTEM SET
	lcd_put_command(0x40);       //SYSTEM SET
	lcd_put_data(0x30);          //HW_SETUP 0x30
	lcd_put_data(0x07);          //FX
	lcd_put_data(0x07);          //FY
	lcd_put_data(0x27);          //CR    320/8-1
	lcd_put_data(0x54);          //TC/R  40-1+4
	lcd_put_data(0xef);          //L/F   240-1
	lcd_put_data(0x28);          //APL   40
	lcd_put_data(0x00);          //APH

//SCROLL:
	lcd_put_command(0x44);       //SCROLL
	lcd_put_data(0x00);          //SAD1L
	lcd_put_data(0x00);          //SAD1H
	lcd_put_data(0xf0);          //SL1
	lcd_put_data(0x80);          //SAD2L
	lcd_put_data(0x25);          //SAD2H
	lcd_put_data(0xf0);          //SL2
	lcd_put_data(0x00);          //SAD3L
	lcd_put_data(0x4b);          //SAD3H
	lcd_put_data(0x80);          //SAD4L
	lcd_put_data(0x70);          //SAD4H

//HDOT_SCR:
	lcd_put_command(0x5a);       //HDOT_SCR
	lcd_put_data(0);             //NO SCROLL

//OVERLAY:
	lcd_put_command(0x5b);       //OVERLAY:
	lcd_put_data(overlay);          //OR 1c

//DISP_OFF:
	lcd_put_command(0x58);       //DISP_of
	lcd_put_data(0x5e);          //

//CRSW
	lcd_put_command(0x46);       //CRSW
	lcd_put_data(0x00);          //
	lcd_put_data(0x00);          //

//CSR_FORM:
	lcd_put_command(0x5d);       //CSR_FORM
	lcd_put_data(0x05);          //
	lcd_put_data(0x87);          //

//CSR_DIR:
    lcd_put_command(0x4c);       //CSR_DIR

//CLEAR_DISPLAY
    lcd_cls();

//CSR_DIR:
    lcd_put_command(0x4c);       //CSR_DIR

//DISP_ON:
	lcd_put_command(0x59);       //DISP_ON
	lcd_put_data(display);
/*
//CGRAM ADR:
	lcd_put_command(0x5c);       //SET CGRAM ADR
	lcd_put_data(0x00);          //SAGL
    lcd_put_data(0x50);          //SAGH
//	lcd_put_font(&curier[8*(128+64)],8*128,0x5400);
*/
    set_P0(TOUCH_lcd);
}

/*****************************************************************************/
void lcd_ready(void)
{
    //int i;

    FIO1DIR &=~((1)<<((d6)));     //P1.22 (D6) config as input

    clr_P1(RS);
	set_P1(RD_wr);
    set_P0(E);

    while(!(FIO1PIN & 0x00400000));
	while(FIO1PIN & 0x00400000);

    clr_P0(E);
    clr_P1(RD_wr);
	set_P1(RS);
	
    FIO1DIR |= ((1)<<((d6)));     //P1.22 (D6) config as output

}
/*****************************************************************************/
void lcd_busy(void)
{
    int i;

    FIO1DIR &=~((1)<<((d6)));     //P1.22 (D6) config as input

    clr_P1(RS);
	set_P1(RD_wr);
    set_P0(E);
	do
	{
		i = FIO1PIN & 0x00400000;
	}
	while(!i);

    clr_P0(E);
	clr_P1(RD_wr);
	set_P1(RS);

    FIO1DIR |= ((1)<<((d6)));     //P1.22 (D6) config as output
}
/*****************************************************************************/
char lcd_read(void)
{
    char byte;

	lcd_put_command(read_display_memory);

    FIO1DIR &=~(0x00ff0000);  //config P1.23 - P1.16 as input

    set_P1(RD_wr);
    set_P1(RS);
    set_P0(E);

    byte = FIO1PIN>>24;

    clr_P0(E);
    clr_P1(RD_wr);
    clr_P1(RS);

    FIO1DIR |= (0x00ff0000);  //config P1.23 - P1.16 as output

	return(byte);
}
/*****************************************************************************/
// x <=319; y <= 239; layer 0-2; color 0-white, 1-black.

void lcd_draw_pix(int x,int y,char layer,char color)
{
    int addr;
    char byte;

    addr = x/8 + y*40 + layer*9600;
    lcd_busy();
    lcd_ready();
    lcd_set_address(addr);
    lcd_busy();
    lcd_ready();
    byte = lcd_read();
    if(color){byte |= 0x80>>(x%8);}
    else {byte &=~(0x80>>(x%8));}
    lcd_busy();
    lcd_ready();
    lcd_set_address(addr);
    lcd_put_command(write_display_memory);
    lcd_busy();
    lcd_ready();
    lcd_put_data(byte);
}
/*****************************************************************************/
/*****************************************************************************/
void lcd_write_byte(char byte,int x,int y,char layer) // x <=39; y <= 239 layer 0,1,2
{
    int addr;

    addr = x + y*40 + layer*9600;
    lcd_busy();
    lcd_ready();
    lcd_set_address(addr);
//    lcd_busy();
//    lcd_ready();
    lcd_put_command(write_display_memory);
//    lcd_busy();
//    lcd_ready();
    lcd_put_data(byte);

}
/*****************************************************************************/
char read_byte(int x,int y,char layer) // x <=39; y <= 239 layer 0,1,2
{
    int addr;

    addr = x + y*40 + layer*9600;
    lcd_busy();
    lcd_ready();

    lcd_set_address(addr);
    lcd_busy();
    lcd_ready();

    return(lcd_read());

}
/*****************************************************************************/
void lcd_line(int x0,int y0,int x1,int y1,char layer,char color)
{
    float k,b,y;
    int i;
    signed char delta;
    char byte,data;

    if(x0==x1)
    {
        if(y0==y1)
        {
            lcd_draw_pix(x0,y0,layer,color);
        }
        else
        {
            if(y0<y1) {delta = 1;}
            else {delta = -1;}
            i = y0;
            do
            {
                lcd_draw_pix(x0,i,layer,color);
                i += delta;
            }
            while(i!=(y1+delta));
            return;
        }
    }
    if(y0==y1)
    {
        if(x0>x1){i=x0;x0=x1;x1=i;}
        i = x0;
        do
        {
            if((i%8 == 0)&&(i <= x1-7))
            {
                lcd_write_byte((color*0xff),i/8,y0,layer);
                i += 8;
            }
            else
            {
                byte = 0;
                if(i == x0)
                {
                    do{byte = (byte<<1)+1;}
                    while(++i%8);
                }
                else
                {
                    do{byte = (byte>>1)+0x80;}
                    while(++i < (x1+1));

                }

                data = read_byte((i-1)/8,y0,layer);
                if(color){data|=byte;}
                else{data&=~(byte);}
                lcd_write_byte(data,(i-1)/8,y0,layer);
            }
        }
        while(i < (x1+1));
        return;
    }

    k = y1;k -= y0;
    k /= (x1-x0);
    b = y0-k*x0;

    if(x0<x1) {delta = 1;}
    else {delta = -1;}
    i = x0;
    do
    {
        y = k*i+b;
        if((y-(int)y)>=0.5)y++;
        lcd_draw_pix(i,(int)y,layer,color);
        i += delta;
    }
    while(i!=(x1+delta));

    if(y0<y1) {delta = 1;}
    else {delta = -1;}
    i = y0;
    do
    {
        y = (i-b)/k;
        lcd_draw_pix((int)y,i,layer,color);
        i += delta;
    }
    while(i!=(y1+delta));

}
/*****************************************************************************/
// x <=39; y <= 239; layer 0-2; *data - data pointer;
//row - number of row; column -number of column;
void lcd_write_memory(int x,int y,char layer,const char *data,int row,int column)
{
    int addr,i;

    addr = x+y*40+layer*9600;
    do
    {
        lcd_busy();
        lcd_ready();
        lcd_set_address(addr);
        lcd_busy();
        lcd_ready();
        lcd_put_command(write_display_memory);
        i = column;
        do
        {
            lcd_busy();
            lcd_ready();
            lcd_put_data(*data++);
        }
        while(--i);
        addr +=40;
    }
    while(--row);
}
/*****************************************************************************/
/*****************************************************************************/
extern"C" void  __busy_ready(void);
void lcd_all_screen(char layer,const char *data)
{
    int i;
    char k;

    __disable_interrupt();
    clr_P0(TOUCH_lcd);
    //__disable_interrupt();
    __busy_ready();
    lcd_set_address(layer*9600);
    //lcd_put_command(write_display_memory);
    set_P0(TOUCH_lcd);
    __enable_interrupt();

    i = 9600;
	do
	{
        k = 25;
        __disable_interrupt();
        clr_P0(TOUCH_lcd);
        __busy_ready();
        lcd_put_command(write_display_memory);
        do{
            lcd_put_data(*data++);
        }		
		while(--k);
        set_P0(TOUCH_lcd);
        __enable_interrupt();
        i -= 25;
	}	
	while(i);


}
/*****************************************************************************/
const char *fnt_ptr;    //активний шрифт
char layer;             //активний прошарок для виводу тексту 0,1,2
// x <=39; y <= 239 layer 0,1,2
void lcd_put_text(int row,int column,char const *str)
{
    int addr;
    char i,k,l;
    const char *ch_ptr;

    addr = column + row*40 + layer*9600;
    lcd_put_command(set_cursor_down);

    while(*str)
    {
        ch_ptr = 8*(*str) * (*fnt_ptr) * (*(fnt_ptr+1)) + (fnt_ptr+2);

        k = *fnt_ptr;
        do
        {
            __busy_ready();
            lcd_set_address(addr);

            i = *(fnt_ptr+1);
            do{
                l = 8;
                __busy_ready();
                __disable_interrupt();
                lcd_put_command(write_display_memory);

                do{lcd_put_data(*ch_ptr++);}
                while(--l);

                __enable_interrupt();
            }while(--i);

            addr++;
            if((addr%40)==0)addr += (*(fnt_ptr+1)*8 -1)*40;
        }
        while(--k);
        str++;
    }
    lcd_put_command(set_cursor_right);
}
/*****************************************************************************/
//const char *fnt_ptr;    //активний шрифт
// x <=39; y <= 239
void lcd_buf_put_text(int row,int column,char const *str,char overlay,bool inv)
{
    int addr;
    int lcd_buf_index;
    char i,k;
    const char *ch_ptr;

    addr = column + row*40;

    while(*str)
    {
        ch_ptr = 8*(*str) * (*fnt_ptr) * (*(fnt_ptr+1)) + (fnt_ptr+2);
        if(*str++ == '\n'){  //переведення рядка
            column = 0;
            row += *(fnt_ptr+1) * 8; //добавити висоту символа в байтах
            addr = column + row*40;
            continue;
        }

        k = *fnt_ptr;//кількість байтів по ширині
        do
        {
            i = *(fnt_ptr+1) * 8;//кількість байтів по висоті у шрифті
            lcd_buf_index = addr;
            do
            {
                switch(overlay)
                {
                    case 0: //замінити
                            lcd_buf[lcd_buf_index] = *ch_ptr++;
                            break;
                    case 1: //AND
                            lcd_buf[lcd_buf_index] &= *ch_ptr++;
                            break;
                    case 2: //OR
                            lcd_buf[lcd_buf_index] |= *ch_ptr++;
                            break;
                    case 3: //XOR
                            lcd_buf[lcd_buf_index] ^= *ch_ptr++;
                            break;
                }
                if(inv)lcd_buf[lcd_buf_index] = ~lcd_buf[lcd_buf_index];
                lcd_buf_index += 40;
            }
            while(--i);

            addr++;
            if((addr%40)==0){
                addr += (*(fnt_ptr+1)*8 - 1)*40;//переведення на новий рядок
                row += *(fnt_ptr+1)*8;
                if(addr >= 9600)return;
            }
        }
        while(--k);
    }
}
/*****************************************************************************/
void lcd_buf_write_array(int column,int row, const char *data,int hight,int width, char overlay,bool inv)
{
    int addr,i;
    int lcd_buf_index;

    if(hight == 0 || width == 0)return;

    addr = column + row*40;

    do
    {
        i = hight;
        lcd_buf_index = addr;
        do
        {
            switch(overlay)
            {
                case 0: //замінити
                        lcd_buf[lcd_buf_index] = *data++;
                        break;
                case 1: //AND
                        lcd_buf[lcd_buf_index] &= *data++;
                        break;
                case 2: //OR
                        lcd_buf[lcd_buf_index] |= *data++;
                        break;
                case 3: //XOR
                        lcd_buf[lcd_buf_index] ^= *data++;
                        break;
            }
            if(inv)lcd_buf[lcd_buf_index] = ~lcd_buf[lcd_buf_index];
            lcd_buf_index += 40;
        }
        while(--i);
        addr++;
    }
    while(--width);
}
/*****************************************************************************/
// x <=319; y <= 239; layer 0-2; color 0-white, 1-black.

void lcd_buf_draw_pix(int x, int y, bool color)
{
    int lcd_buf_index;
    char byte;

    lcd_buf_index = x/8 + y*40;

    byte = lcd_buf[lcd_buf_index];
    if(color){byte |= 0x80>>(x%8);}
    else {byte &=~(0x80>>(x%8));}

    lcd_buf[lcd_buf_index] = byte;
}
/*****************************************************************************/
// x <=39; y <= 239
void lcd_buf_write_byte(char byte, int x, int y, char overlay)
{
    int lcd_buf_index;

    lcd_buf_index = x + y*40;

    switch(overlay)
    {
        case 0: //замінити
                lcd_buf[lcd_buf_index] = byte;
                break;
        case 1: //AND
                lcd_buf[lcd_buf_index] &= byte;
                break;
        case 2: //OR
                lcd_buf[lcd_buf_index] |= byte;
                break;
        case 3: //XOR
                lcd_buf[lcd_buf_index] ^= byte;
                break;
    }
}
/*****************************************************************************/
void lcd_buf_line(int x0,int y0,int x1,int y1,bool color)
{
    float k,b,y;
    int i;
    signed char delta;
    char byte;

    if(x0==x1)
    {
        if(y0==y1)
        {
            lcd_buf_draw_pix(x0,y0,color);
        }
        else
        {
            if(y0<y1) {delta = 1;}
            else {delta = -1;}
            i = y0;
            do
            {
                lcd_buf_draw_pix(x0,i,color);
                i += delta;
            }
            while(i!=(y1+delta));
            return;
        }
    }
    if(y0==y1)
    {
        if(x0>x1){i=x0;x0=x1;x1=i;}
        i = x0;
        do
        {
            if((i%8 == 0)&&(i <= x1-7))
            {
                lcd_buf_write_byte((color*0xff),i/8,y0,0);
                i += 8;
            }
            else
            {
                byte = 0;
                if(i == x0)
                {
                    do{byte = (byte>>1)+0x80;}
                    while(++i < x1);//while(++i%8);
                }
                else
                {
                    do{byte = (byte>>1)+0x80;}
                    while(++i < (x1+1));

                }
                char overlay;
                if(color)overlay = 2;//or
                else{byte =~byte; overlay = 1;/*and*/}
                lcd_buf_write_byte(byte,(i-1)/8,y0,overlay);
            }
        }
        while(i < (x1+1));
        return;
    }

    k = y1;k -= y0;
    k /= (x1-x0);
    b = y0-k*x0;

    if(x0<x1) {delta = 1;}
    else {delta = -1;}
    i = x0;
    do
    {
        y = k*i+b;
        if((y-(int)y)>=0.5)y++;
        lcd_buf_draw_pix(i,(int)y,color);
        i += delta;
    }
    while(i!=(x1+delta));

    if(y0<y1) {delta = 1;}
    else {delta = -1;}
    i = y0;
    do
    {
        y = (i-b)/k;
        lcd_buf_draw_pix((int)y,i,color);
        i += delta;
    }
    while(i!=(y1+delta));

}

/*****************************************************************************/
void lcd_buf_DrawLine(int x, int y, int x1, int y1, char color)
{
    register int dx, dy, DX, DY, A_B, DX2, DY2;

    A_B = 0;
    dx = dy = 1;
    DX = x1 - x;
    DY = y1 - y;

    if (DX < 0) { DX = -DX; dx = -dx; }
    if (DY < 0) { DY = -DY; dy = -dy; }
    if (DX >= DY)
    {
	    DX2 = DX >> 1;
	
        for (; x != x1; x+= dx)
        {
             if(color < 2)lcd_buf_draw_pix(x, y, true);
             else if(x & 0x1) lcd_buf_draw_pix(x, y, true);
             A_B+= DY;
             if(A_B >= DX2) { y+= dy; A_B-= DX; }
        }
    }
    else
    {
        DY2 = DY >> 1;
        for (; y != y1; y+= dy)
        {
           if(color < 2)lcd_buf_draw_pix(x, y, true);
           else if(y & 0x1) lcd_buf_draw_pix(x, y, true);
           A_B+= DX;
           if (A_B >= DY2) { x+= dx; A_B-= DY; }
        }
    }
 }
/*****************************************************************************/
void lcd_buf_fill_window(int row, int column,int hight,int width, char data, char overlay,bool inv)
{
    int addr,i;
    int lcd_buf_index;

    addr = column + row*40;

    do
    {
        i = hight;
        lcd_buf_index = addr;
        do
        {
            switch(overlay)
            {
                case 0: //замінити
                        lcd_buf[lcd_buf_index] = data;
                        break;
                case 1: //AND
                        lcd_buf[lcd_buf_index] &= data;
                        break;
                case 2: //OR
                        lcd_buf[lcd_buf_index] |= data;
                        break;
                case 3: //XOR
                        lcd_buf[lcd_buf_index] ^= data;
                        break;
            }
            if(inv)lcd_buf[lcd_buf_index] = ~lcd_buf[lcd_buf_index];
            lcd_buf_index += 40;
        }
        while(--i);
        addr++;
    }
    while(--width);
}
/*****************************************************************************/
// x <=39; y <= 239
void lcd_buf_window(int row, int column, int hight, int width)
{
    //lcd_buf_fill_window(offsetRow-1*8, offsetColumn-1 ,15*8, 23, 0x55, 0, false);

    lcd_buf_fill_window(row, column, hight, width, 0x00, 0, false);
    //up
    lcd_buf_line(column*8+2, row, column*8+width*8-2, row, true);
    lcd_buf_line(column*8+1, row+1, column*8+width*8-1, row+1, true);
    //down
    lcd_buf_line(column*8+1, row+hight-1 , column*8+width*8-1, row+hight-1, true);
    lcd_buf_line(column*8+2, row+hight, column*8+width*8-2, row+hight, true);
    //left
    lcd_buf_line(column*8, row+2, column*8, row+hight-2, true);
    lcd_buf_line(column*8+1, row+1, column*8+1, row+hight-1, true);
    //right
    lcd_buf_line(column*8+width*8, row+2, column*8+width*8, row+hight-2, true);
    lcd_buf_line(column*8+width*8-1, row+1, column*8+width*8-1, row+hight-1, true);

}
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/



