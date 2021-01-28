#include "iolpc2138.h"
#include "defines.h"
#include "fonts.h"
#include "type.h"
#include "menu.hpp"
#include "uart_0.h"
#include "uart_1.h"

#include "main.hpp"
#include "lcd_middle.h"
#include "touch.h"
#include "spi.h"
#include "text_menu.cpp"

#include "lcd.h"
#include "timers.h"

#include "modbus.h"
#include "math.h"
#include "pich_view.h"
#include "graf.h"

/*
#define menu_key    6
#define up          2
#define left        1
#define right       4
#define down        3
#define enter       5
#define enc_ent     7
//#define inc         7
//#define dec         8
//���� 1 � �� 7 �� �� ������ �������
*/
#define menu_key    13
#define up          4
#define left        12
#define right       8
#define down        16
#define enter       15
#define enc_ent     17

PARAMETRS N;
AIU_ADD aiu_add[13]; //�������� ��������� ��� ��U
AIU aiu[13]; //����� ���������� �����
AOU aou; //���������� �����
int aou_curr;//��� ���������� ������
QGA Qga;// ������� ���� � ������


RTF general_purpose_flags;
//0 - ���� ���������
//1 - ���� ������

DUMP dump;//��������� ��� ����������� ��������� ����

ARCH_VIEW one;//��� ������� �� ���� ������ ��� ���������

int password_test;//��� ��������� ������
/*************************************************************************************************\
* ���������� ����� ��� ���������� ����� �� ����
\*************************************************************************************************/
struct cell{
    char sublevel;      //������ ����� �� �����
    char lcd_flash_row; //�����, �� �����
    SUB *menu_ptr;//������
    char menu_size;     //�����
};

struct cell map;//��������� ��������� ����

#define menu_stack_size 16
struct{
    struct cell one_cell[menu_stack_size];
    char index;
}menu_stack;

typedef struct {//��� ���� ��������� ���� �������� ���� ����
    char input_row;    //����� �� ���������� �����������
    char fetch; //0 - ��������� ����� 1- ��������� �������� ����� 2- �������� ���������� �����
    void (*alt)();//����  �� ������� ���� �� ������� ��������������� �������������� �������� ����� �� ���� � ��������
    SUB *object;//���� �� ������ �����������
    char flash;//������� ������� �� ��������
    bool menu;//���� 1 �� �������� �� ����, ���� 0 �� �������� �����
}MENU_STATUS;

MENU_STATUS menu_status;

int  encB;//������ ��� ��������

void cpy_xx(char *xptr_dest, char *xptr_src, unsigned short count)//�����
{
    do{*xptr_dest++ = *xptr_src++;}
    while(--count);
}

void clr_xx(char *xptr_dest, unsigned short count)//������� ������
{
    do{*xptr_dest++ = 0;}
    while(--count);
}

unsigned char menu_push()//������� ������� ��� �������� �������� � ���� � 0 ���� ����� �����
{
    if(menu_stack.index == menu_stack_size)return 0;
    cpy_xx((char*)&menu_stack.one_cell[menu_stack.index++], (char*)&map, sizeof(map));

    return 1;
}

unsigned char menu_pop()//������� 1 ��� �������� ������� � 0 ��� ���������� �����
{
    if(menu_stack.index == 0)return 0;
    cpy_xx((char*)&map, (char*)&menu_stack.one_cell[--menu_stack.index],sizeof(map));
    clr_xx((char*)&menu_stack.one_cell[menu_stack.index],sizeof(map));//�������� �����
    return 1;
}

void menu_init()
{
    char *ptr;
    char i;

    i = sizeof(menu_stack);
    ptr = (char*)&menu_stack;
    do{*ptr = 0;ptr++;}while(--i);

    clr_xx((char*)&menu_status,sizeof(menu_status));

    map.sublevel = 0;
    map.lcd_flash_row = 0;
    map.menu_ptr = &menu0[0];
    map.menu_size = sizeof(menu0)/sizeof(SUB)-1;
}

typedef struct{
char no_refresh :1;
char input      :1;//�����������
char enter_bit  :1;
char request_for:1;
char was_make   :1;
}INPUT_STATUS;

union{
    INPUT_STATUS i_sts_bit;
    char input_status;
};




int new_temperature;//���� ����������� � ��������
int t_Cool;//����������� �������� �����
void default_key(char dkey);
/*-----------------------------------------------------------------------------------------------*/
// ��������� ���� �� �������
/*-----------------------------------------------------------------------------------------------*/
char last_i;//��� ���������� �� ���������� ����� �����
char press_cntr;//˳������� ��������� ������
char lcdF;//������ ��� ������������

void processing(void)
{
    unsigned char i,offset;//,key_1;
/*
    if(menu_status.menu == false){
        //���� �������� ����� �� ��������� ���������� � ������� �����
        if((Z1>128)&&(Z2<3968)){
             if(highlight == false)menu_status.menu = true;
        }else highlight = false;

    }else  sheet_captions();
*/
    sheet_captions();
    
    if(menu_status.menu == true){//���� ����� �������� ����

        if(menu_status.alt){
            menu_status.alt();
            //key_1 = kays_pop();
            return;
        }

        for(i=0;i<(240/(*(DefaultFont+1)*8)-0);i++){

            if(map.sublevel+i > map.menu_size)offset = map.sublevel+i - map.menu_size-1;
            else offset = map.sublevel+i;
            //���� � ���� ������ �� ������ �����
            if(i > map.menu_size){
                LCD_put_str(i,0,"                    ");
                continue;
            }

            if(map.menu_ptr[offset].manage->pre_funct == 0 || map.menu_ptr[offset].manage->pre_funct()==0){
                map.menu_ptr[offset].print(LCD_str);
            }
            else map.menu_ptr[offset].manage->pre_funct();

            if((menu_status.fetch == 2) && (offset == menu_status.input_row)){//���� ��� �����c �����������
                //�������� �����
                char width;
                char *ptr;
                width = 320/(*(DefaultFont)*8);
                ptr = LCD_input_str_;
                do{
                    *ptr = ' ';
                    ptr++;
                }while(--width);
                ptr = '\0';

                //������������
                LCD_put_str(i,0,LCD_input_str);
                if(time_10mS >= 50 || encB&0x3){
                    char *ptr_flash = LCD_crsr_position - &LCD_input_str[0] + &LCD_input_str_[0];//��������� ������� ������������
                    char pos_flash = menu_status.flash;//������� ������� �� ��������
                    do *ptr_flash++ = '';
                    while(--pos_flash);
                    LCD_highlight(i, 0, LCD_input_str_);
                    //LCD_highlight_crsr(i,0,LCD_input_str_);
                }
            }
            else{
                //��������� ������ ����� �� �������
                LCD_put_str(i,0,LCD_str);
                if(i == map.lcd_flash_row){
                    /*
                    if((CTC>32768+6553)? 1:((CTC<6553)? 0:((CTC>32768)? 0:1)))LCD_put_str(i,0,LCD_str);
                    else LCD_put_str(i,0,"                    ");
                    */
                    LCD_highlight(i,0);
                }
                //else
            }
            //create_fill(40,(*(DefaultFont+1)*8));
            //lcd_buf_write_array(0,i*(*(DefaultFont+1)*8),&keys[0],*(DefaultFont+1)*8,40,3,false);

         }

        //������ ����� ��� ��������� �� ����� ����
        if(((Z1>128)&&(Z2<3968))){
            i = y / (*(DefaultFont+1)*8);
            if(i == last_i){
                if(++press_cntr > 2){
                    if((i<(240/(*(DefaultFont+1)*8)-2))&&(i <= map.menu_size)){  //
                        if((menu_status.fetch == 0) && !touched){
                            map.lcd_flash_row = i;
                            kays_push(enter);
                            touched = true;
                        }else{
                            if(!touched && (i == map.lcd_flash_row)){
                                kays_push(enter);
                                touched = true;
                            }
                        }
                    }
                }
                if((i<(240/(*(DefaultFont+1)*8)-2))&&(i <= map.menu_size)){
                        if((menu_status.fetch == 0) && !touched){
                            map.lcd_flash_row = i;
                        }
                }
            }else  press_cntr =0;
            last_i = i;
        }
        else{
            touched = false;
            press_cntr =0;
        }

        //  ������� �����
        i = kays_pop();
        switch(i){
            case menu_key:  menu_KEY(); break;
          /*
            case inc:        break;
            case dec:        break;
            */
            case enter:     enter_key(); break;
            case enc_ent:   encoder_key(); break;
            case right:     right_key(); break;
            case up:        up_key(); break;
            case down:      down_key(); break;
            case left:      left_key(); break;
            default: if(i)default_key(i);break;
        }

    }
    else{//�������� �����
      i = kays_pop();  
      if( i == menu_key || i == enc_ent)menu_status.menu = true;//������ � ����� �������� ����
      else{
        
        LPC_Rtc_Time_t Time;
        RTC_GetTime(&Time);
        sprintf(LCD_str,"%02d:%02d:%02d",Time.hour,Time.minute,Time.second);//h:m:s
        LCD_put_str(0, 32,LCD_str);
        
        sprintf(&LCD_str[0],"%+4.1f\260C   %+4d\260C ",(float)t_Cool/10,new_temperature);
        LCD_put_str(0, 0,LCD_str);
        
        //sprintf(&LCD_str[0],"X = %04d \nY = %04d\n Z1= %4d \nZ2= %4d",X,Y,Z1,Z2);
        /*
        sprintf(&LCD_str[0],"X = %04d \nY = %04d\nE= %4d\nZ2= %4d",X,Y,err.cntr[0],Z2);
        LCD_put_str(2, 0,LCD_str,curier);
        */
        /*sprintf(&LCD_str[0],"",Z11);
        LCD_put_str(5, 0,LCD_str,curier20);
        */
        
        unsigned short i = 1;
        char *tmp_ptr;
        const char *tmp_src;
        tmp_ptr = &lcd_buf[1600+7];
        tmp_src = &pich_view[0];
        for(i=0;i<sizeof(pich_view);i++){
          if(i&&(i%25==0)) tmp_ptr += 15;
          *tmp_ptr++ = *tmp_src++;
        }
        
        sprintf(&LCD_str[0],"%6.2f��",(float)Qga.Qa/100); LCD_put_str(4, 1,LCD_str); //������� ������
        sprintf(&LCD_str[0],"%6.2f��",(float)Qga.Qg/100); LCD_put_str(1, 19,LCD_str);//������� ����
        sprintf(&LCD_str[0],"%5.1f\260",(float)aiu_add[2].r.t/10); LCD_put_str(1, 11,LCD_str);//���������� ���� �� ��������
        sprintf(&LCD_str[0],"%5.1f\260",(float)aiu_add[4].r.t/10); LCD_put_str(2, 16,LCD_str);//����������� �����
        sprintf(&LCD_str[0],"%5.1f%%",(float)aiu[10].d.meo.out/10); LCD_put_str(3, 3,LCD_str);//������� ������
        sprintf(&LCD_str[0],"%5.1f%%",(float)aiu[9].d.meo.out/10); LCD_put_str(2, 29,LCD_str);//������� ���
        sprintf(&LCD_str[0],"%7.3f��",(float)aiu_add[12].r.g/1000); LCD_put_str(6, 0,LCD_str);//г����
        sprintf(&LCD_str[0],"%6.1f\260",(float)aiu_add[5].r.t/10); LCD_put_str(7, 2,LCD_str);//���������� Գ����
        sprintf(&LCD_str[0],"%6.1f\260",(float)aiu_add[3].r.t/10); LCD_put_str(7, 31,LCD_str);//���������� � ��� ������
        
        sprintf(&LCD_str[0],"%5.2f�",(float)aiu[12].d.vibr.T/100); LCD_put_str(3, 33,LCD_str);//����� �������
        sprintf(&LCD_str[0],"%5.2f�",(float)aiu[12].d.vibr.t/100); LCD_put_str(4, 33,LCD_str);//��� ��������� ��������
        
        int percent;
        percent = (aou.current * 10000)>>16;
        percent += (((aou.current * 10000) & 0xffff)>0x7fff)? 1:0;
        sprintf(&LCD_str[0],"%5.2fA",(float)pid_r[0].Sp/100); LCD_put_str(8, 2,LCD_str);//����� ������ ������� �������
        sprintf(&LCD_str[0],"%5.2fA",(float)aiu_add[11].r.g/100); LCD_put_str(9, 2,LCD_str);//�����
        sprintf(&LCD_str[0],"%6.2f%%",(float)percent /100); LCD_put_str(10, 2,LCD_str);//��������� �������
        sprintf(&LCD_str[0],"%5.1f\260",(float)aiu_add[8].r.t/10); LCD_put_str(10, 11,LCD_str);//���������� Գ�����
        
        if(aiu_add[7].r.Ohm > 1000)sprintf(&LCD_str[0],"%5.1f\260",(float)aiu_add[7].r.t/10);
        else sprintf(&LCD_str[0],"######");
        LCD_put_str(10, 18,LCD_str);//���������� �����������
        
        if(N.seq.flag_ok==0){
          sprintf(&LCD_str[0],"���������� � ������� �����! ���������!");
          LCD_put_str(11, 0,LCD_str);//�����������
        }
        
        if(aiu_add[6].r.Ohm > 1000)sprintf(&LCD_str[0],"%5.1f\260",(float)aiu_add[6].r.t/10);
        else sprintf(&LCD_str[0],"######");
        LCD_put_str(12, 0,LCD_str);//���������� ������, �������� ���� ����
        
        if(N.R.auto_on){ //��������� ����������� �������
          switch(Flr.regime){
          case 0: //�������� �������� ������� � ����� � �� ��������� ����� ������� � ����������
            //���� ������� ���� ��� ������ �������� ������� �� �������� ��� ��������� �������
            if(!Flr.power || !Flr.on){
              if(Flr.time_down > 3600) sprintf(&LCD_str[0],"��� ������������� ������� ����� ������");
              else sprintf(&LCD_str[0],"��� ������������� ������� %4d:%02d",Flr.time_down/60,Flr.time_down%60);
            }else{
              sprintf(&LCD_str[0],"");//�������� �����
            }
            break;
            
          case 1:
            //�������
            switch(Flr.phase){
            case 0:
              //������ ������ - ����������� �� ���������, 15�
              sprintf(&LCD_str[0],">5��. ����1 %4d:%02d �� %5.2f�",Flr.time/60,Flr.time%60,(float)N.R.c1/100);
              break;
            case 1:
              sprintf(&LCD_str[0],">5��. ����2 %4d:%02d �� %5.2f�",Flr.time/60,Flr.time%60,(float)N.R.c2/100);
              break;
            case 2:
              sprintf(&LCD_str[0],">5��. ����3 %4d:%02d �� %5.2f�",Flr.time/60,Flr.time%60,(float)N.pid[0].Sp/100);
              break;
            case 3: //  ��� �� ������������ �� �� �������� �������
              sprintf(&LCD_str[0],">5��. �����������");
              break;
            }
            break;
            
          case 2://�� 5 ��.
            switch(Flr.phase){
            case 0:
              sprintf(&LCD_str[0],"1��.<t<5��. ����1 %4d:%02d �� %5.2f�",Flr.time/60,Flr.time%60,(float)N.R.c5/100);
              break;
            case 1:
              sprintf(&LCD_str[0],"1��.<t<5��. ����2 %4d:%02d �� %5.2f�",Flr.time/60,Flr.time%60,(float)N.pid[0].Sp/100);
              break;
            case 2: //��� �� ������������ �� �� �������� �������
              sprintf(&LCD_str[0],"1��.< t <5��. �����������");
              break;
            }
            break;
            
          case 3://�� 1 ��.
            switch(Flr.phase){
            case 0:
              sprintf(&LCD_str[0],"t<1��. ����1 %4d:%02d �� %5.2f�",Flr.time/60,Flr.time%60,(float)N.pid[0].Sp/100);
              break;
            case 1: //��� �� ������������ �� �� �������� �������
              sprintf(&LCD_str[0],"<1��. �����������");
              break;
            }
            break;
            
          case 4:
            switch(Flr.b_phase){
            case 0:
              sprintf(&LCD_str[0],"");//�������� �����
              break;
            case 1:
              //������ ������ - ����������� �� ���������, 15�
              sprintf(&LCD_str[0],"����������� ����1 %4d:%02d �� %5.2f�",Flr.time/60,Flr.time%60,(float)N.R.c_b1/100);
              break;
            case 2:
              sprintf(&LCD_str[0],"����������� ����2 %4d:%02d �� %5.2f�",Flr.time/60,Flr.time%60,(float)N.R.c_b2/100);
              break;
            case 3:
              sprintf(&LCD_str[0],"����������� ����3 %4d:%02d �� 0�",Flr.time/60,Flr.time%60);
              break;
            case 4: //  ��� �� ������������ �� �� �������� �������
              sprintf(&LCD_str[0],"����������� ��������!");
              break;
            }
            break;
            
          }
          LCD_put_str(13, 0,LCD_str,true);
        }
        
        tmp_ptr = &LCD_str[0];
        for(i=0;i<14;i++){
          if(aum.err & (1<<i)){
            if(i<9){
              sprintf(tmp_ptr,"%d ",i+1);
              tmp_ptr += 2;
            }else{
              sprintf(tmp_ptr,"%d ",i+1);
              tmp_ptr += 3;
            }
            
          }
        }
        if(aum.err){
          sprintf(tmp_ptr,"ERROR");
          LCD_put_str(14, 0,LCD_str,true);
        }
        
        /*
        sprintf(&LCD_str[0],"���       ������  "); LCD_put_str(i++, 0,LCD_str,true);
        sprintf(&LCD_str[0],"%7.3f�� %7.3f��",(float)aiu_add[0].r.g/1000,(float)aiu_add[1].r.g/1000); LCD_put_str(i++, 0,LCD_str);
        sprintf(&LCD_str[0],"  %5.1f%%    %5.1f%%",(float)aiu[9].d.meo.out/10,(float)aiu[10].d.meo.out/10); LCD_put_str(i++, 0,LCD_str);
        sprintf(&LCD_str[0],"  %5.1f%%    %5.1f%%",(float)aiu[9].d.meo.out/10,(float)aiu[10].d.meo.out/10); LCD_put_str(i++, 0,LCD_str);
        */
        
      }
    }
}

/*-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/        
const char menu_key_arr[]={'0','1','2','3','3','4','5','6','6','7','8','9','9','9','0'};
void default_key(char dkey)
{
    if(i_sts_bit.was_make)input_status = 0;
    if(menu_status.fetch == 2){
    //    if(map.menu_ptr[menu_status.input_row].type < SUB_DAY){
            if(isdigit(*LCD_crsr_position)){
                if(dkey < 15)*LCD_crsr_position = menu_key_arr[dkey];
            }
            else{
                if(*LCD_crsr_position == '+')*LCD_crsr_position = '-';
                else
                    if(*LCD_crsr_position == '-')*LCD_crsr_position = '+';
            }
            RIGHT();
     //   }
    }
}

/*-----------------------------------------------------------------------------------------------*/
void enter_key(void)
{
    unsigned char i;

    if(menu_status.fetch == 0){//���� ���� � ����� ��������
        //����� ����� ������� ���� 0..map.menu_size-1
        if((i = map.sublevel+map.lcd_flash_row) > map.menu_size)i -= map.menu_size+1;

        if(map.menu_ptr[i].manage->next){//���� � �������� ���� ������ ���� �� �������� �� ��
            if(map.menu_ptr[i].manage->pre_enter)map.menu_ptr[i].manage->pre_enter(0);//�������� ������ ������� ���������� ��. �����

            menu_push();//�������� � ���� ������� �������
            map.menu_size = map.menu_ptr[i].manage->size;
            map.menu_ptr = (SUB*)map.menu_ptr[i].manage->next;
            map.lcd_flash_row = 0;
            map.sublevel = 0;
            return;
        }
        else{
            /*
            if(map.menu_ptr[menu_status.input_row].manage->pre_enter)
                map.menu_ptr[menu_status.input_row].manage->pre_enter();//�������� ������ ������� ���������� ��. �����
            */
            if(map.menu_ptr[i].manage->change){//�������� ����� ��������
                menu_status.input_row = i;//����� �� ���� ���������� ���� �����
                menu_status.fetch = 1;//��������� �������� ����� ��� �������� �����������

                menu_status.object = map.menu_ptr + menu_status.input_row;//��'��� �� ����Բ�Ӫ����

                void(*ptr)(char) = (void(*)(char)) map.menu_ptr[menu_status.input_row].manage->pre_enter;
                if(ptr)ptr(menu_status.input_row);//�������� ������ ������� ���������� ��. �����
            }
        }
    }
    else{//���� ���� � ����� �����������
        if(map.menu_ptr[menu_status.input_row].manage->post_enter == 0 || map.menu_ptr[menu_status.input_row].manage->post_enter()==0)
            map.menu_ptr[menu_status.input_row].scan(LCD_input_str);

        menu_status.fetch = 0;//���������� � ����� ��������
    }
}
/*-----------------------------------------------------------------------------------------------*/        
void encoder_key(void)
{
    unsigned char i;

    if(menu_status.fetch == 0){//���� ���� � ����� ��������
        //����� ����� ������� ���� 0..map.menu_size-1
        if((i = map.sublevel+map.lcd_flash_row) > map.menu_size)i -= map.menu_size+1;

        if(map.menu_ptr[i].manage->next){//���� � �������� ���� ������ ���� �� �������� �� ��
            if(map.menu_ptr[i].manage->pre_enter)map.menu_ptr[i].manage->pre_enter(0);//�������� ������ ������� ���������� ��. �����

            menu_push();//�������� � ���� ������� �������
            map.menu_size = map.menu_ptr[i].manage->size;
            map.menu_ptr = (SUB*)map.menu_ptr[i].manage->next;
            map.lcd_flash_row = 0;
            map.sublevel = 0;
            return;
        }
        else{
            /*
            if(map.menu_ptr[menu_status.input_row].manage->pre_enter)
                map.menu_ptr[menu_status.input_row].manage->pre_enter();//�������� ������ ������� ���������� ��. �����
            */
            if(map.menu_ptr[i].manage->change){//�������� ����� ��������
                menu_status.input_row = i;//����� �� ���� ���������� ���� �����
                menu_status.fetch = 1;//��������� �������� ����� ��� �������� �����������

                menu_status.object = map.menu_ptr + menu_status.input_row;//��'��� �� ����Բ�Ӫ����

                void(*ptr)(char) = (void(*)(char)) map.menu_ptr[menu_status.input_row].manage->pre_enter;
                if(ptr)ptr(menu_status.input_row);//�������� ������ ������� ���������� ��. �����
                if(map.menu_ptr[i].manage->type != SUB_BOOL)encB = 1;//������ ��������������� ��������� ��������
                else encB = 0x11;//������ �� ���������� �� ��������� ��������                
            }
        }
    }
    else{
      switch (encB){
      case 0x00:
        encB = 1;//������ ��������������� ��������� ��������
        break;
      case 0x01:
      case 0x11:      
        //���� ���� � ����� �����������
        if(map.menu_ptr[menu_status.input_row].manage->post_enter == 0 || map.menu_ptr[menu_status.input_row].manage->post_enter()==0)
          map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
        menu_status.fetch = 0;//���������� � ����� ��������
        encB = 0;//�������� ���������� ������� ����������� ��������� �����
        break;
      case 0x03:
        encB = 0;//�������� ���������� ������� ����������� ��������� �����
        break;
        
      }        
    }
}

/*-----------------------------------------------------------------------------------------------*/
//------------------------------------------------------------------------------------------------/
char LCD_init_crsr_position(void)
{
    LCD_crsr_position = LCD_input_str + 40 / *DefaultFont-1; //�� �����
    do{
      if(isdigit(*LCD_crsr_position) || (*LCD_crsr_position >= 'A' && *LCD_crsr_position <= 'F')){
        RIGHT();
        return 1;
      }
    }while(LCD_crsr_position--);
    return 0;
}
/*-----------------------------------------------------------------------------------------------*/
void right_key(void)
{
    if(i_sts_bit.was_make)input_status = 0;
    if(menu_status.fetch == 2){
       if(map.menu_ptr[menu_status.input_row].manage->right_funct)
        map.menu_ptr[menu_status.input_row].manage->right_funct();
    }//RIGHT();
}


/*-----------------------------------------------------------------------------------------------*/
void up_key(void)
{
    if(i_sts_bit.was_make)input_status = 0;
    if(menu_status.fetch == 2){
        if(map.menu_ptr[menu_status.input_row].manage->up_funct)
            map.menu_ptr[menu_status.input_row].manage->up_funct();
    }//UP();
    else{
        //��� ������������ ������
        if(map.lcd_flash_row > 0)map.lcd_flash_row--;
        else{
            //���� � ���� ����� ������ �� ������
            if(map.menu_size <= (240/(*(DefaultFont+1)*8)-2)-1)map.lcd_flash_row = map.menu_size;
            else{
                if(map.sublevel != 0) map.sublevel--;
                else map.sublevel = map.menu_size;
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*/
void down_key(void)
{
    if(i_sts_bit.was_make)input_status = 0;
    if(menu_status.fetch == 2){
        if(map.menu_ptr[menu_status.input_row].manage->down_funct)
        map.menu_ptr[menu_status.input_row].manage->down_funct();
    }//DOWN();
    else{
        //��� ������������ ������
        if(map.lcd_flash_row < (240/(*(DefaultFont+1)*8)-2)-1){
            if(map.lcd_flash_row < map.menu_size)map.lcd_flash_row++;
            else map.lcd_flash_row = 0;
        }
        else{
            //���� � ���� ����� ������ �� ������
            if(map.menu_size <= (240/(*(DefaultFont+1)*8)-2)-1)map.lcd_flash_row = 0;
            else{
                if(map.sublevel < map.menu_size) map.sublevel++;
                else map.sublevel = 0;
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*/
void left_key(void)
{
    if(i_sts_bit.was_make)input_status = 0;
    if(menu_status.fetch == 2){
        if(map.menu_ptr[menu_status.input_row].manage->left_funct)
        map.menu_ptr[menu_status.input_row].manage->left_funct();
    }//LEFT();
}

/*-----------------------------------------------------------------------------------------------*/
void menu_KEY(void)
{
    if(menu_status.fetch == 0){
        if(menu_pop()==0) menu_status.menu = false;//������� � ���� �� ������� �����
    }
    else menu_status.fetch = 0;//������ � ����� ��������
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void UP()
{
    if(*LCD_crsr_position == '9')*LCD_crsr_position = '0';
    else
        if(*LCD_crsr_position == '+')*LCD_crsr_position = '-';
        else
            if(*LCD_crsr_position == '-')*LCD_crsr_position = '+';
            else *LCD_crsr_position = *LCD_crsr_position + 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void DOWN()
{
    if(*LCD_crsr_position == '0')*LCD_crsr_position = '9';
    else
        if(*LCD_crsr_position == '+')*LCD_crsr_position = '-';
        else
            if(*LCD_crsr_position == '-')*LCD_crsr_position = '+';
            else *LCD_crsr_position = *LCD_crsr_position - 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void RIGHT()
{
    LCD_crsr_position++;
    if((LCD_crsr_position > &LCD_input_str[0]+320/(*(DefaultFont)*8)-1) || (*LCD_crsr_position <= ' ')){
        char *ptr;
        ptr = (char*)strchr(map.menu_ptr[menu_status.input_row].manage->format,'%');
        if(ptr)LCD_crsr_position = (ptr - map.menu_ptr[menu_status.input_row].manage->format + &LCD_input_str[0]);
        else{
            LCD_crsr_position = 0;
            menu_status.fetch = 0;
            return;
        }
    }
    if(*LCD_crsr_position =='.' || *LCD_crsr_position == ':'||*LCD_crsr_position == 'e')LCD_crsr_position++;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void LEFT()
{
    if((LCD_crsr_position != &LCD_input_str[0])&&(*(LCD_crsr_position-1) != ' '))LCD_crsr_position--;
    else
    {
        if(LCD_init_crsr_position_right() == 0){
            LCD_crsr_position = 0;
            menu_status.fetch = 0;
            return;
        }
    }
    if(*LCD_crsr_position == '.'||*LCD_crsr_position == ':'||*LCD_crsr_position == 'e')LCD_crsr_position--;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void UP_hex()
{
    if(*LCD_crsr_position == 'F')*LCD_crsr_position = '0';
    else
        if(*LCD_crsr_position == '9')*LCD_crsr_position = 'A';
        else *LCD_crsr_position = *LCD_crsr_position + 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void DOWN_hex()
{
    if(*LCD_crsr_position == '0')*LCD_crsr_position = 'F';
    else
        if(*LCD_crsr_position == 'A')*LCD_crsr_position = '9';
        else *LCD_crsr_position = *LCD_crsr_position - 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void preEnterNum(char offset)//��� ��������� �����
{
    map.menu_ptr[offset].print(LCD_str);//��������� � ����� �����������
    LCD_str[40 / *DefaultFont] = 0;//��� ���� ������� ������ �� ������������

    //�������� ����� ��� �����������
    int i;
    i = 40 / *DefaultFont;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//���� � ����� �����������
    if(LCD_init_crsr_position()){
        menu_status.fetch = 2;
        menu_status.flash = 1;//������� �����������
    }
    else menu_status.fetch = 0;//�������
}

/*************************************************************************************************\
\\  ������� BOOL - ����
\*************************************************************************************************/
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void UP_BOOL(){
    if(*LCD_crsr_position == 'O')memcpy(LCD_crsr_position," ON",3);
    else memcpy(LCD_crsr_position,"OFF",3);
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/

void preEnterBool(char offset)//��� BOOL
{
    char *ptr;

    map.menu_ptr[offset].print(LCD_str);//��������� � ����� �����������
    LCD_str[41] = 0;//��� ���� ������� ������ �� ������������

    //�������� ����� ��� �����������
    int i;
    i = 40 / *DefaultFont;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//���� � ����� �����������

    ptr = strrchr(map.menu_ptr[offset].manage->format,'%');//����� ������� ��������� ������ � �����-������
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].manage->format + LCD_input_str);//���������� �������
        menu_status.flash = 3;//������� �����������
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//�������
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/

void preEnterParity(char offset)//��� parity
{
    char *ptr;

    map.menu_ptr[offset].print(LCD_str);//��������� � ����� �����������
    LCD_str[21] = 0;//��� ���� ������� ������ �� ������������

    //�������� ����� ��� �����������
    int i;
    i = 40 / *DefaultFont;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//���� � ����� �����������

    ptr = strrchr(map.menu_ptr[offset].manage->format,'%');//����� ������� ��������� ������ � �����-������
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].manage->format + LCD_input_str);//���������� �������
        menu_status.flash = 5;//������� �����������
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//�������
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void UP_PARITY(){
    char tmp;
    char *ptr;

    switch(*(LCD_crsr_position+4)){
      default:
      case '�': tmp = 0;break;
      case ' ': tmp = 1;break;
      case '1': tmp = 2;break;
      case '0': tmp = 3;break;
    }
    if(--tmp > 3)tmp = 3;
    switch(tmp){
      default:
      case 0: ptr = "�����";break;
      case 1: ptr = " ��� ";break;
      case 2: ptr = "���.1";break;
      case 3: ptr = "���.0";break;
    }
    memcpy(LCD_crsr_position,ptr,5);
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void DOWN_PARITY(){
    char tmp;
    char *ptr;

    switch(*(LCD_crsr_position+4)){
      default:
      case '�': tmp = 0;break;
      case ' ': tmp = 1;break;
      case '1': tmp = 2;break;
      case '0': tmp = 3;break;
    }
    if(++tmp > 3)tmp = 0;
    switch(tmp){
      default:
      case 0: ptr = "�����";break;
      case 1: ptr = " ��� ";break;
      case 2: ptr = "���.1";break;
      case 3: ptr = "���.0";break;
    }
    memcpy(LCD_crsr_position,ptr,5);
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/

char postEnterBitrate0()//��� �������� ����0
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //uart0_bitrate(*map.menu_ptr[menu_status.input_row].manage->number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_word_length0()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //uart0_bitrate(*map.menu_ptr[menu_status.input_row].manage->number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_stop_bit0()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //uart0_bitrate(*map.menu_ptr[menu_status.input_row].manage->number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_enable0()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //uart0_parity_enable(*map.menu_ptr[menu_status.input_row].manage->number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_select0()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //uart0_parity_select(*map.menu_ptr[menu_status.input_row].manage->number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}



/*------------------------------------------------------------------------------------------------/
/   ������� �� �����     UART1
/------------------------------------------------------------------------------------------------*/

char postEnterBitrate1()//��� �������� ����1
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //uart1_bitrate(*map.menu_ptr[menu_status.input_row].manage->number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_word_length1()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //uart1_bitrate(*map.menu_ptr[menu_status.input_row].manage->number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_stop_bit1()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //uart1_bitrate(*map.menu_ptr[menu_status.input_row].manage->number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_enable1()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //uart1_parity_enable(*map.menu_ptr[menu_status.input_row].manage->number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_select1()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //uart1_parity_select(*map.menu_ptr[menu_status.input_row].manage->number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}

/*-----------------------------------------------------------------------------------------------*/






















/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_new_snap_time()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //arch_time = new_time(N.snap_time);
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����  ������  �����
/------------------------------------------------------------------------------------------------*/
char postEnter_pass_replace()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    if(password_test == 120978 ||password_test == N.password){
        //menu_push();//�������� � ���� ������� �������
        map.menu_size = map.menu_ptr[menu_status.input_row].manage->size;
        map.menu_ptr = (SUB*)((int)(map.menu_ptr[menu_status.input_row].manage->next_one) & 0x7fffffff);//������ �������� ��
        map.lcd_flash_row = 0;
        map.sublevel = 0;
        password_test = 0;
    }
    else{
        ERROR = 'Z';
        ERROR_TIME = 4;
    }

    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����  ������
/------------------------------------------------------------------------------------------------*/
char postEnter_pass_dialog()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    if(password_test == N.password){
        //menu_push();//�������� � ���� ������� �������
        map.menu_size = map.menu_ptr[menu_status.input_row].manage->size;
        map.menu_ptr = (SUB*)((int)(map.menu_ptr[menu_status.input_row].manage->next_one) & 0x7fffffff);//������ �������� ��
        map.lcd_flash_row = 0;
        map.sublevel = 0;
        password_test = 0;
    }
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����  ALARM
/------------------------------------------------------------------------------------------------*/
void preEnter_ALARM_QUIT(char i)
{


    IO0SET = 1<<25;//���в� ��ղ� ²�������� ���в�
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����  ALARM
/------------------------------------------------------------------------------------------------*/
void preEnter_ALARM_RESET(char i)
{
/*
    GLOBAL_ALARM = 0;//����� �Ѳ
    LAST_GLOBAL_ALARM = 0;
    for(i=0;i<8;i++){
        last_status_ml314[i] = 0;
    }

    IO0SET = 1<<25;//���в� ��ղ� ²�������� ���в�
*/
/*
    for(i=0;i<8;i++){
        if(ml314[i].status !=0x7F){
             last_status_ml314[i] &= ml314[i].status;//����� �� �����
             if(ml314[i].status == 0){
                 LAST_GLOBAL_ALARM &= ~(1<<i);//����� �� ���в� � ��������� �̲���
                 GLOBAL_ALARM &= ~(1<<i);//����� �� ���в� � ��������� �̲���
             }
        }
    }

    if(driver[SN_menu][1].data  & 0x80 == 0)//����� �� ����������
        LAST_GLOBAL_ALARM = GLOBAL_ALARM &= ~0x0100;//����� �� ���в� � ��������� �̲���
*/
    //���� �������� �������� ������
    for(i=0;i<9;i++){//0..7-���������� 8-���������
        if(*((int*)0xE0024014) <= virt_time_[i]){//���� ���� ������������ ���𳿿 �� �� ���������
            GLOBAL_ALARM &= ~(1<<(16+i));//����� ����� ��������
            LAST_GLOBAL_ALARM &= ~(1<<(16+i));//����� ����� ��������
        }
    }
}
/*------------------------------------------------------------------------------------------------/
/   ������� ��������������� ������������ �������
/------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------/
/   ������� ��������������� ������������ �������
/------------------------------------------------------------------------------------------------*/

/*===============================================================================================*/
/*------------------------------------------------------------------------------------------------/
/   ������� ��������������� ������������ �������  ���Բ��
/------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------/
/   ������� �� ����� ��� �������         GRAF GRAF
/------------------------------------------------------------------------------------------------*/
//------------------------------------------------------------------------------------------------/
//------------------------------------------------------------------------------------------------/
/*===============================================================================================*/
int force;//�� ��� ����������� ���������� �������
void flash_arch_alt_g()
{
    encB &= ~0x2;//������ ������ ��������� 
    //���� ������ �������� ���������
    if(!force)repeat =0;
    else repeat =1;
    
    switch(kays_pop()){

      case left:
        if(one.cursor){
            if(repeat)one.cursor = (one.cursor >= 8)? one.cursor-8 : 0;
            else one.cursor--;

        }
        else{
            one.cursor = 239;//����� ������� ������
            N.archive_addr = (N.archive_addr - 256 * 15) & 0x3FFF00;
            one.addr = N.archive_addr;
            flash_arch_read_g();
        }
        break;

      case right:
        if(one.cursor < 239){
            if(repeat)one.cursor = (one.cursor <= 239-8)? one.cursor+8 : 239;
            else one.cursor++;
        }
        else{
            one.cursor = 0;
            if(N.archive_addr != arch_flash_addr - 256*14)N.archive_addr = (N.archive_addr + 256*15) & 0x3FFF00;
            if(N.archive_addr == arch_flash_addr - 256*14){
                N.archive_addr = (arch_flash_addr - 256*14) & 0x3FFF00;
                one.addr = N.archive_addr;
                flash_arch_last();
                one.cursor = 239;//����� ������� ������
            }
            else{
                one.addr = N.archive_addr;
                flash_arch_read_g();
            }
        }
        break;

      case up:
        if(one.channal) one.channal--;
        else one.channal = 6;
        break;

      case down:
        if(one.channal < 6) one.channal++;
        else one.channal = 0;
        break;

    case enc_ent:
        switch (encB){
        case 0x00:
          encB = 1;//������ ��������������� ��������� ��������
          break;
        case 0x01:
          //���� ���� � ����� �����������
          encB = 0;//�������� ���������� ������� ����������� ��������� �����
          force = !force;//�� ��� ����������� ���������� �������
          break;
        case 0x03:
          encB = 0;//�������� ���������� ������� ����������� ��������� �����
          break;
        }        
      break;        
      
    case menu_key:
        menu_status.alt = 0;
        menu_status.fetch = 0;
        return;
    }

    flash_arch_disp_g();
}

/*===============================================================================================*/
char postEnter_arch_flash_g()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.alt = flash_arch_alt_g;
    N.archive_addr = (arch_flash_addr - 256*14) &  0x3FFF00;// �� ������ ����� //N.archive_addr & 0x7ff00;
    one.addr = N.archive_addr;
    one.channal = 0;
    one.cursor = 240/2;

    flash_arch_last();

    //flash_arch_read_g();
    flash_arch_disp_g();
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_arch_flash()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.alt = flash_arch_alt_g;
    N.archive_addr = N.archive_addr & 0x3fff00;
    one.addr = N.archive_addr;
    one.start_channal = 0;
    one.cursor = 240/2;
    flash_arch_read();
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_arch_flash_P()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.alt = flash_arch_alt_g;
    N.archive_addr = (arch_page * 256) & 0x3fff00;
    one.addr = N.archive_addr;
    one.start_channal = 0;
    one.cursor = 240/2;
    flash_arch_read();
    return 1;
}


/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����  ��� ���������� ������ ���������� � ���� ���� ������� ����� ����
/------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_ws1()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    all = 1;//���������� ������ ������ �������
    return 1;
}
/*-----------------------------------------------------------------------------------------------*/




/*#####################################   ��������� �������  ########################################*/
/*####################################################################################################*/
/*####################################################################################################*/

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
int for_identification;//��� ���� ��� ���������� � ���� ������� ���������� ���������
void preEnter_Ident()
{
    int i;
    //����� ����� ������� ���� 0..map.menu_size-1
    if((i = map.sublevel+map.lcd_flash_row) > map.menu_size)i -= map.menu_size+1;
    for_identification = i;
    //dosers_task[i].read_setup = 1;//���������� ������ ������� �������
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void preEnter123(char offset)//��� 123
{
    char *ptr;

    map.menu_ptr[offset].print(LCD_str);//��������� � ����� �����������
    LCD_str[41] = 0;//��� ���� ������� ������ �� ������������

    //�������� ����� ��� �����������
    int i;
    i = 40 / *DefaultFont;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//���� � ����� �����������

    ptr = strrchr(map.menu_ptr[offset].manage->format,'%');//����� ������� ��������� ������ � �����-������
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].manage->format + LCD_input_str);//���������� �������
        menu_status.flash = 40;//������� �����������
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//�������
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void DOWN_123()
{
    char tmp;
    char *ptr;

    tmp = *(LCD_crsr_position+1);
    if(isdigit(tmp))  tmp = (*(LCD_crsr_position)-0x30)*10 + (tmp - 0x30);
    else tmp = (*(LCD_crsr_position)-0x30);
    tmp++;
    if(tmp > 4) tmp = 0;

    switch(tmp){
              default:
              case 0: ptr = "0 - mA";break;
              case 1: ptr = "1 - ���(K)";break;
              case 2: ptr = "2 - ���(B)";break;
              case 3: ptr = "3 - ���100";break;
              case 4: ptr = "4 - ���100";break;
    }
    sprintf(LCD_crsr_position,ptr);
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void UP_TC()
{
    signed char tmp;
    char *ptr;

    tmp = *(LCD_crsr_position+1);
    if(isdigit(tmp))  tmp = (*(LCD_crsr_position)-0x30)*10 + (tmp - 0x30);
    else tmp = (*(LCD_crsr_position)-0x30);
    if(tmp == 0) tmp = 1;
    else tmp = 0;
    switch(tmp){
              default:
              case 0: ptr = "0 - ����������� � ��� ������";break;
              case 1: ptr = "1 - ����������� � ��� ������";break;
    }
    sprintf(LCD_crsr_position,ptr);
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void UP_123()
{
    signed char tmp;
    char *ptr;

    tmp = *(LCD_crsr_position+1);
    if(isdigit(tmp))  tmp = (*(LCD_crsr_position)-0x30)*10 + (tmp - 0x30);
    else tmp = (*(LCD_crsr_position)-0x30);
    tmp--;
    if(tmp < 0) tmp = 4;

    switch(tmp){
              default:
              case 0: ptr = "0 - mA";break;
              case 1: ptr = "1 - ���(K)";break;
              case 2: ptr = "2 - ���(B)";break;
              case 3: ptr = "3 - ���100";break;
              case 4: ptr = "4 - ���100";break;
    }
    sprintf(LCD_crsr_position,ptr);
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
bool no_setup = 0;
char postEnter_123()
{
    no_setup = true;
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    //req_type[for_identification] = 1;
    no_setup = false;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void UP_comp()
{
    char tmp;
    char *ptr;

    tmp = (*(LCD_crsr_position)-0x30);
    tmp++;
    if(tmp > 3) tmp = 0;

    switch(tmp){
              default:
              case 0: ptr = "0 - ���������������� ����";break;
              case 1: ptr = "1 - ���������������� �";break;
    }
    sprintf(LCD_crsr_position,ptr);
}

/*####################################################################################################*/
/*####################################################################################################*/
/*####################################################################################################*/
/*####################################################################################################*/

/*##################################  ������� ������ ���������   ###################################*/
char postEnter_aui0_ws_ram()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.fetch = 0;
    aiu[for_identification].cmd = 1;//��������� ����������������
    aum.task = (0x01 << for_identification*2);

    return 1;
}
/*####################################################################################################*/
char postEnter_aui0_ws_flash()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.fetch = 0;
    aum.task = (0x02 << for_identification*2);
    return 1;
}
/*####################################################################################################*/
char postEnter_aui10_ws_flash()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.fetch = 0;
    aum.task = (0x02 << 9*2);
    return 1;
}
/*####################################################################################################*/
char postEnter_aui11_ws_flash()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.fetch = 0;
    aum.task = (0x02 << 10*2);
    return 1;
}
/*####################################################################################################*/
char postEnter_aui10_ws_ram()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.fetch = 0;
    aum.task = (0x01 << 9*2);
    return 1;
}
/*####################################################################################################*/
char postEnter_aui11_ws_ram()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.fetch = 0;
    aum.task = (0x01 << 10*2);
    return 1;
}
/*####################################################################################################*/
char postEnter_aui13_ws_ram()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.fetch = 0;

    aiu[12].d.vibr.duty = N.vibr.duty;
    aiu[12].d.vibr.T = N.vibr.T;
    aiu[12].d.vibr.t = N.vibr.t;

    aum.task = (0x01 << 12*2);
    return 1;
}
/*####################################################################################################*/
char postEnter_aou_ws_ram()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.fetch = 0;
    aum.task = (0x01 << 13*2);
    return 1;
}
/*####################################################################################################*/
char postEnter_aou_ws_Curr_ram()
{
    map.menu_ptr[menu_status.input_row].scan(LCD_input_str);
    menu_status.fetch = 0;
    aou.current = (aou_curr << 16) / 20000;//���������� ��������� ������ AOU
    if(aou.current > 65535) aou.current = 65535;
    aum.task = (0x01 << 13*2);
    return 1;
}
/*####################################################################################################*/

//==============================================================================
//��� ��������
int encoder;//��� ���������� ��������
int encF;//������� ������ � ����� ������

void MA_IntHandler_EINT1( void )
{
  int encC;
  EXTINT_bit.EINT1 = 1;//����� ����������� 0
  
  if(FIO0PIN_bit.P0_3==1)return;

  encC = 10;
  while(--encC && (FIO0PIN_bit.P0_3==0));
  
  if(encC == 0 && encoder ==0){
    encoder = 2;
    if(FIO0PIN_bit.P0_2)encF = 1; //up key  
    else encF = -1; //���� key 
  }else encF=0;

}

//==============================================================================


void menu()
{
    processing();

    //root1[1].number = a;
    //root1[1].print(LCD_str);
    //root1[0].scan(LCD_str);
}


