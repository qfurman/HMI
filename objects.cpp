
#include "type.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include "lcd.h"
#include "fonts.h"
#include "processing.hpp"
#include "rtc.h"
#include "timers.h"

/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
// ����������� �������� ����
struct sheet_seq
{
    char num;
    char seq[20];
}sh_seq;
/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
class text
{
    public:
    char row;               // �����
    char column;            // ��������
    char const *fnt_ptr;    // �����
    char const *str;        // �����

    text(char row_, char column_, const char *fnt_ptr_,char const *str_);
};
text::text(char row_, char column_, const char *fnt_ptr_,char const *str_)
{
    row = row_;
    column = column_;
    fnt_ptr = fnt_ptr_;
    str = str_;
}
/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
class caption
{
    public:
                            // ���������� ��������� ����� ���� ������
    char x,y;               // 0 <= x <= 39, 0 <= y <= 239
    char width,hight;       // ������ � ������ � ������
    char const *pict;       // �������� �� �������� ����� � ������

    caption(char x_,char y_,char width_,char hight_,char const *pict_);
};

// constructor
caption::caption(char x_,char y_,char width_,char hight_,char const *pict_)
{
    x = x_;
    y = y_;
    width = width_;
    hight = hight_;
    pict = pict_;

}

/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
/*****************************************************************************/
// ���� ������� ����
class num_fild
{
    public:
    char row,column;    // ���������� ��������� ����� ���� row = 0..239 column = 0..39
    char const*font;    // �����
    bool change;        // ����� ��������
    bool select;        // ����
    bool time_f;        // ���
    char const *format; // ������ �����
    static char out_str[40];   // ����� ��� ��������� �����
    int count;          // ������� ������� � �����
    double number;      // ���������
    static double*back; // ������� �����
    static char len;    // ������ ���� ��� �����
    static char offset; // ������� ���� ��� �����
    char*(*func_pre)(); // �������� �� �����������
    char*(*func_post)();// �������� ���� �����������
    static double*ptr_number;//��������������� � ��������� ��������

    num_fild(char*(*func_pre_)(), char*(*func_post_)(), char row_, char column_, char const*font_, bool change_, const char*format_, double number_, bool time_f_= false);
    void contour();     // ��������� �������
    void text();        // ����� �����
    void highlight(bool flag);  // ��������
    bool hit_test();    // �������� ����������
    void copy_out();    // ��������� ����� ��� ��������
    static char*no_change();//do nothing
    static char*month();// ����� ������ ����� �� �����
    static char*month_back();//����� �����
    static char*time_();// ��� ��������� ����
    static char*time_back();//����� ��� �����
};
// constructor
num_fild::num_fild(char*(*func_pre_)(), char*(*func_post_)(), char row_, char column_, char const*font_, bool change_, const char*format_, double number_, bool time_f_)
{
    time_f = time_f_;
    func_pre = func_pre_;
    func_post = func_post_;
    row = row_;
    column = column_;
    font = font_;
    change = change_;
    format = format_;
    number = number_;
    count = sprintf(out_str,format,number);

};

// ��������� �������
void num_fild::contour()
{
    create_fill(*font*count,(*(font+1)*8));
    lcd_buf_write_array(column,row,&keys[0],*(font+1)*8,*font*count,0,false);
}

// ����� �����
void num_fild::text()
{
    char *ptr;
    ptr_number = &number;
    //enter_func = func_post; //����������� ��� ���������� ������ �����
    ptr = func_pre();

    if(time_f)  //������� ������
    {
        char*time_ptr;
        time_ptr = (char*)ptr_number;
        count = sprintf(out_str, format, *(time_ptr+2), *(time_ptr+1), *time_ptr);
    }
    else count = sprintf(out_str,format,number);

    fnt_ptr = font;
    if(ptr)
    {
        //number = MONTH;
        count = strlen(ptr);
    }
    else ptr = &out_str[0];
    lcd_buf_put_text(row,column,ptr,3,select);//xor
}

// ��������
void num_fild::highlight(bool flag)
{
    int i;
    char ch;
    if(flag) ch = '';
    else ch = ' ';
    for(i=0;i<count;i++)
    {
        out_str[i] = ch;
    }
    fnt_ptr = font;
    layer = 1;
    lcd_buf_put_text(row,column,out_str,3,false);//xor
    layer = 0;
}

// �������� ����������
bool num_fild::hit_test(){return tolerance(column, row, *font*count, *(font+1)*8);}

// ��������� ����� ��� ��������
void num_fild::copy_out()
{
    int i,ch_num;
    char ch = 0;
    len = 12;

    ch_num = strlen(out_str);
    if(ch_num>len)
    {
        len = ch_num;
        if(len%2)len++;
        offset = (len - 12)/2;
    }
    else offset = 0;
    strcpy(&input_string[(len - ch_num)/2],out_str);

    for(i=0;i<(len - ch_num)/2;i++)
    {
        input_string[i] = ' ';
    }
    for(i=i+ch_num;i<len;i++)
    {
        input_string[i] = ' ';
    }
    input_string[i] = 0;

    for(i=ch_num+(len - ch_num)/2;i>(len - ch_num)/2;i--)
    {
        if((input_string[i] == ' ')&&(i!=ch_num+(len - ch_num)/2))input_string[i] = '0';
        else
            if(input_string[i] == '+' || input_string[i] == '-')
            {
                ch = input_string[i];
                input_string[i] = '0';
            }
    }
    if(ch)input_string[i] = ch; //���� '+' ��� '-' ���� �
    else
        if((input_string[i] == ' '))input_string[i] = '0';
    init_crsr_position();
}
//do nothing
char*num_fild::no_change()
{
    return 0;
}
// �����
char*num_fild::month()
{
    char ptr;
    static char months[][11] =
    {
        {"          "},
        {" ѳ����   "},
        {" �����    "},
        {" �������� "},
        {" ������  "},
        {" �������  "},
        {" �������  "},
        {" ������   "},
        {" �������  "},
        {" �������� "},
        {" �������  "},
        {" �������� "},
        {" �������  "},
    };
    ptr = *ptr_number;
    return &months[ptr][0];
}
//����� ���������� �����
char*num_fild::month_back()
{
    int month;

    month = atoi(input_string);
    if((month>0)&&(month <13))*num_fild::back = month;

    return ((char*)1);
}
//����� ����
char*num_fild::time_back()
{
    char *ptr_sc;
    char hour,min,sec;

    ptr_sc = strchr(input_string,':');

    hour = atoi(input_string);
    if(hour > 23)return((char*)1);
    ptr_sc++;
    min = atoi(ptr_sc);
    if(min > 59)return((char*)1);
    ptr_sc = strchr(ptr_sc,':');
    sec = atoi(++ptr_sc);
    if(sec > 59)return((char*)1);

    ptr_sc = (char*)ptr_number;
    *ptr_sc = sec;
    *(ptr_sc+1) = min;
    *(ptr_sc+2) = hour;

    return((char*)1);
}
/*****************************************************************************/

char num_fild::out_str[40];
double *num_fild::back;
char num_fild::len;
char num_fild::offset;  // ������� ���� ��� �����
double*num_fild::ptr_number;

/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
class sheet_keys:public caption
{
    public:
    //bool chacked;           // ���������
    static char sheet;      // ����� ������� �������

    sheet_keys(char x_,char y_,char width_,char hight_,char const *pict_);
};

sheet_keys::sheet_keys(char x_,char y_,char width_,char hight_,char const *pict_)
:caption(x_,y_,width_,hight_,pict_)
{
    //chacked = false;
    sheet = 0;
}
/*****************************************************************************/
char sheet_keys::sheet;
/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
class control_keys:public caption
{
    public:
    static bool touch_block;    //����� ��������� ���� �������� ����
    static bool need_for_change;//������� � ��� ���������
    static bool out_from_change;//�������� ��������
    static bool was_1s; //1 ������� ������
    void(*func_ptr)();

    control_keys(void(*func_ptr_)(),char x_,char y_,char width_,char hight_,char const *pict_);
    void processing();  // ������� �����
    static void close();
    static void ok();
    static void up();
    static void down();
    static void right();
    static void left();
    static void close_window();
    static void zero();
    static void one();
    static void two();
    static void three();
    static void four();
    static void five();
    static void six();
    static void seven();
    static void eight();
    static void nine();
    static void button_pres(char charact);
    static bool time_snap();
    static void prepare_input();
};
// �����������
control_keys::control_keys(void(*func_ptr_)(),char x_,char y_,char width_,char hight_,char const *pict_)
:caption(x_,y_,width_,hight_,pict_)
{
    func_ptr = func_ptr_;
}
// ��������
void control_keys::close()
{
    layer = 1;
    lcd_put_text(50,15-num_fild::offset,"            ");
    close_window();
}
// �������
void control_keys::processing()
{
    func_ptr();
}
//ok
void control_keys::ok()
{
    if(enter_func()){close(); return;}

    *num_fild::back = atof(input_string);
    close();
}
//up
void control_keys::up()
{
    if(input_string[cursor_position] == '9')input_string[cursor_position] = '0';
    else
        if(input_string[cursor_position] == '+')input_string[cursor_position] = '-';
        else
            if(input_string[cursor_position] == '-')input_string[cursor_position] = '+';
            else input_string[cursor_position]++;
}
//down
void control_keys::down()
{
    if(input_string[cursor_position] == '0')input_string[cursor_position] = '9';
    else
        if(input_string[cursor_position] == '+')input_string[cursor_position] = '-';
        else
            if(input_string[cursor_position] == '-')input_string[cursor_position] = '+';
            else input_string[cursor_position]--;
}
//right
void control_keys::right()
{
    input_string_[cursor_position] = ' ';
    cursor_position++;
    if(input_string[cursor_position] <= 0x20)
    {
        cursor_position = 0;
        while(input_string[cursor_position] <= 0x20)cursor_position++;
    }
    if(input_string[cursor_position]=='.' || input_string[cursor_position]== ':')cursor_position++;
    input_string_[cursor_position] = '';
}
//left
void control_keys::left()
{
    input_string_[cursor_position] = ' ';

    if(input_string[cursor_position] <= 0x20)
    {
        cursor_position = 0;
        while(input_string[cursor_position] <= 0x20)cursor_position++;
    }
    if((cursor_position != 0)&&(input_string[cursor_position-1] != 0x20))cursor_position--;
    else
    {
        if(input_string[cursor_position] <= 0x20)
        {
            cursor_position = 0;
            while(input_string[cursor_position] <= 0x20)cursor_position++;
        }
        else
        {
            do cursor_position++;
            while(input_string[cursor_position] > 0x20);
            cursor_position--;
        }
    }
    if(input_string[cursor_position] == '.'||input_string[cursor_position] == ':')cursor_position--;
    input_string_[cursor_position] = '';
}
//close window
void control_keys::close_window()
{
    sh_seq.num--;
    sheet_keys::sheet = sh_seq.seq[sh_seq.num-1];
    touch_block = true;
    need_for_change = false;
    out_from_change = false;
}
//button 0
void control_keys::zero()   {button_pres('0');}
void control_keys::one()    {button_pres('1');}
void control_keys::two()    {button_pres('2');}
void control_keys::three()  {button_pres('3');}
void control_keys::four()   {button_pres('4');}
void control_keys::five()   {button_pres('5');}
void control_keys::six()    {button_pres('6');}
void control_keys::seven()  {button_pres('7');}
void control_keys::eight()  {button_pres('8');}
void control_keys::nine()   {button_pres('9');}

// ������� ��������� ������
void control_keys::button_pres(char charact)
{
    //if(time_snap())return;

    if(input_string[cursor_position] < 0x30) control_keys::right();
    input_string[cursor_position] = charact;
    control_keys::right();

}
// ������ �������� �����
bool control_keys::time_snap()
{
    if(block_1s || block_100ms)return true;
    block_100ms = true;
    if(!was_1s){block_1s = true; was_1s = true;}
    return false;
}
// �������������� ����� � ���������� ��� �����
void control_keys::prepare_input()
{

    create_fill(num_fild::len, 24);
    lcd_buf_write_array(15-num_fild::offset,70-24,&keys[0],24, num_fild::len,0,false);

    fnt_ptr = &fixedsys[0];
    lcd_buf_put_text(50,15-num_fild::offset,input_string,3,false);//xor
    layer = 1;
    lcd_put_text(50,15-num_fild::offset,input_string_);

}
/*****************************************************************************/
bool control_keys::touch_block;
bool control_keys::need_for_change;
bool control_keys::out_from_change;
bool control_keys::was_1s;
/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/

typedef void form_funct(void);
typedef form_funct *form_f;

class form
{
    public:
    char sheet;             // ����� ������� �������
    num_fild*param;         // ���������
    char param_;            // ������� ���������
    control_keys*button;    // ������
    char button_;           // ������� ������
    form_f funct_ptr;       // ������� ��������
    char funct;             // ������� ������� ��������
    int rows;
    int columns;
    int hights;
    int widths;
    text*txt;               // �����
    char txt_;              // ������� ����� ������

    form(char sheet_, num_fild*param_, char param__, control_keys*button_, char button__, form_f funct_ptr_=0, char funct_=0, int rows_=0, int columns_=0, int hights_=0, int widths_=0, text*_txt=0, char txt__=0);
    static void refresh(form*_form);    // ���������
};

form::form(char sheet_, num_fild*_param, char param__, control_keys*_button, char button__, form_f funct_ptr_, char funct_, int rows_, int columns_, int hights_, int widths_, text*_txt, char txt__)
{
    sheet = sheet_;
    param = _param;
    param_ = param__;
    button = _button;
    button_ = button__;
    funct_ptr = funct_ptr_;
    funct = funct_;
    rows = rows_;
    columns = columns_;
    hights = hights_;
    widths = widths_;
    txt = _txt;
    txt_ = txt__;
}
//���������
void form::refresh(form*_form)
{
    char i,l;
    bool block;
    char index=0;


    if(coordinates == false)
    {
        control_keys::touch_block = false;    //������������� �����
        block_1s = false;       time_1s = 0;
        block_100ms = false;    time_100ms = 0;
        control_keys::was_1s = false;
    }
    //if(_form->sheet != sheet_keys::sheet)return;

    if(_form->hights !=0)lcd_buf_window(_form->rows, _form->columns, _form->hights, _form->widths);

    //������

    form_f*ptr;
    form_f ptr_f;

    ptr = &_form->funct_ptr;

    for(i=0;i<_form->funct;i++)
    {
        ptr_f = *(ptr++);
        ptr_f();
    }

    //�����
    for(i=0;i<_form->txt_;i++)
    {
        fnt_ptr = (_form->txt+i)->fnt_ptr;
        lcd_buf_put_text((_form->txt+i)->row, (_form->txt+i)->column, (_form->txt+i)->str, 0, false);
    }

    //���������
    for(i=0;i<_form->param_;i++)
    {

        if((_form->param+i)->time_f)
        {
            time_num_fild[index++] = &(_form->param+i)->number;
            time_num_fild[index] = 0;
        }

        (_form->param+i)->contour();
        (_form->param+i)->text();

        if((!control_keys::touch_block)&&(!control_keys::out_from_change)&&((_form->param+i)->hit_test()))
        {
            if(!(_form->param+i)->select)
            {
                for(l=0;l<_form->param_;l++)
                {
                    if((_form->param+i)->select)
                    {
                        block = true;
                        break;
                    }
                    else block = false;
                }
                if(block) continue;

                //�������� ���� � ���������� ��� �����
                sh_seq.seq[sh_seq.num] = 128;
                sh_seq.num++;
                sheet_keys::sheet =128;

                control_keys::need_for_change = true;
                control_keys::out_from_change = true;

                num_fild::back = &(_form->param+i)->number;
                (_form->param+i)->copy_out();
                (_form->param+i)->select = true;
                enter_func = (_form->param+i)->func_post; //����������� ��� ���������� ������ �����
            }
        }
        else
        {
            if((_form->param+i)->select)
            {
                if(!control_keys::need_for_change)(_form->param+i)->select = false;
            }
        }
    }

    //������
    for(i=0;i<_form->button_;i++)
    {
        if((!control_keys::touch_block)&&(_form->sheet == sheet_keys::sheet)&&(tolerance((_form->button+i)->x, (_form->button+i)->y, (_form->button+i)->width, (_form->button+i)->hight))&&(!control_keys::touch_block))
        {
            create_key((_form->button+i)->width, (_form->button+i)->hight, true, (_form->button+i)->pict);
            if(!control_keys::time_snap())(_form->button+i)->processing();
        }
        else
        {
            create_key((_form->button+i)->width, (_form->button+i)->hight, false, (_form->button+i)->pict);
        }
        lcd_buf_write_array((_form->button+i)->x, (_form->button+i)->y, &keys[0], (_form->button+i)->hight, (_form->button+i)->width, 0, false);
    }



}

/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/








