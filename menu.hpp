#ifndef __MENU_INC
#define __MENU_INC

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "ramtron.h"
#include "LPC_Rtc.h"
#include "main.hpp"


extern PARAMETRS N;
extern AIU_ADD aiu_add[13]; //додаткові параметри для АІU
extern AIU aiu[13]; //плати аналогових входів
extern AOU aou; //аналоговий вихід
extern int aou_curr;//для переахунку струму
extern QGA Qga;// витрата газу і повітря

extern RTF general_purpose_flags;

//частоник
extern DUMP dump;//структура для відображення вмістимого флеш
extern int flash_point;//місце запису у флеш, збільшити на 256 після успішного запису;

extern int encB;//ознаки для енкодера

TIME_ARCH new_time(int sec);
extern ARCH_VIEW one;//для закачки із флеш архіву для перегляду



extern int password_test;//для введеного пароля

extern int GLOBAL_ALARM;//aварії від регуляторів 0..7 і частотника 8
extern int LAST_GLOBAL_ALARM;//aварії від регуляторів 0..7 і частотника 8


extern unsigned short alarm_record_addr;
extern int encoder;//для блокування енкодера
extern int encF;//функція кнопки в даний момент

//віртуальні таймери
extern union{
    TIME_ARCH virt_time[10];
    int virt_time_[10];
};

extern int wd_count;


extern int new_temperature;//нова температура
extern int t_Cool;//температура холодних кінців
extern int force;//це для прискорення переміщення курсора

/*************************************************************************************************\
*   base class for menu
*
*
\*************************************************************************************************/
#define SUB_NUM 0
#define SUB_BOOL 1
#define SUB_PARITY 2

#define SUB_TIME 4
#define SUB_DATE 5
#define SUB_DAY 6
#define SUB_CR_LF 7
#define SUB_HEX 8
#define SUB_123 9
#define SUB_TC 10
#define SUB_comp 11
#define SUB_sts 12


class SUB{
  public:
    const MENU_MAN *manage;//на структуру управління меню
    //int number;//саме число
    //SUB(char type_,char *format_, char  change_, char fract_){type = type_;format = format_; change = change_; fract = fract_;}
    SUB(const MENU_MAN *manage_){manage = manage_;}
    //виведення в рядок
    void print(char* dest)
    {
        int count;
        char ch;
        bool flag=false;
        char *ptr;

        switch(manage->type){
          case SUB_HEX:
          case SUB_NUM:
            count = sprintf(dest,manage->format,*manage->number);
            if(manage->fract){              //якщо є знаки після коми
                dest += count;              //поінтемо на кінекь тобто на нуль
                do{
                    ch = *dest;
                    *(dest+1) = ch;         //перестановка символа ліворуч
                    dest--;                 //на одмн символ ліворуч
                    if(isdigit(ch)&&!flag){ //якщо цифра
                        flag = true;        //знайдено останню цифру з кінця
                        count = manage->fract;//кількість яку треба переставити
                    }
                }while(--count);            //пошук першої цифри з кінця
                *(dest+1) = '.';            //ставимо крапку
                if(*dest == ' ')*dest = '0';//для чисел із незаповненням 0 лівих позицій поставити нуль замість пробіла
            }
            break;

          case SUB_BOOL://BOOL
            sprintf(dest,manage->format,*manage->number? " ON":"OFF");
            break;

          case SUB_PARITY:
            switch(*manage->number){
              default:
              case 0: ptr = "НеПАР";break;
              case 1: ptr = " ПАР ";break;
              case 2: ptr = "Уст.1";break;
              case 3: ptr = "Уст.0";break;
            }
            sprintf(dest,manage->format,ptr);
            break;

          case SUB_TIME:
                //LPC_Rtc_Time_t Time;
                //if(RTC_GetTime(&Time))
            sprintf(dest,manage->format,*((int*)0xE0024028),*((int*)0xE0024024),*((int*)0xE0024020));//h:m:s
            break;

          case SUB_DATE:
            LPC_Rtc_Date_t pdate;
            //sprintf(dest,manage->format,*((int*)0xE0024028),*((int*)0xE0024024),*((int*)0xE0024020));//h:m:s
            RTC_GetDate(&pdate);
            sprintf(dest,manage->format,pdate.day, pdate.month, pdate.year);
            break;

          case SUB_DAY:
            switch(*((int*)0xE0024030)){//dow
                case 0: ptr = "       НЕДІЛЯ       ";break;
                case 1: ptr = "      ПОНЕДІЛОК     ";break;
                case 2: ptr = "       ВІВТОРОК     ";break;
                case 3: ptr = "        СРЕДА       ";break;
                case 4: ptr = "        ЧЕТВЕР      ";break;
                case 5: ptr = "       П'ЯТНИЦЯ     ";break;
                case 6: ptr = "       СУБОТА       ";break;
                default: ptr = "    НЕВІРНА ДАТА!   ";break;
            }
            sprintf(dest,manage->format,ptr);
            return;

          case SUB_CR_LF:
            switch(*manage->number){
            default:
              case 0: ptr = "\2\377\377  \377\377\3";break;
              case 1: ptr = "\2CR  \377\377\3";break;
              case 2: ptr = "\2CR  LF\3";break;
            }
            sprintf(dest,manage->format,ptr);
            break;

          case SUB_TC:
            switch(*manage->number){
              default:
              case 0: ptr = "0 - Температура в зоні горіння";break;
              case 1: ptr = "1 - Температура в зоні фідера";break;
            }
            sprintf(dest,manage->format,ptr);
            break;

          case SUB_123:
            switch(*manage->number){
              default:
              case 0: ptr = "0 - mA";break;
              case 1: ptr = "1 - ТХА(K)";break;
              case 2: ptr = "2 - ТПР(B)";break;
              case 3: ptr = "3 - ТСМ100";break;
              case 4: ptr = "4 - ТСП100";break;
            }
            sprintf(dest,manage->format,ptr);
            break;

          case SUB_comp:
            switch(*manage->number){
              default:
              case 0: ptr = "0 - Термокомпенсації Немає";break;
              case 1: ptr = "1 - Термокомпенсація Є";break;
            }
            sprintf(dest,manage->format,ptr);
            break;

          case SUB_sts:
            switch(*manage->number){
              default:
              case 0: ptr = "Состояние датчика:        НОРМА";break;
              case 1: ptr = "Состояние датчика:        ОБРЫВ";break;
            }
            sprintf(dest,manage->format,ptr);
            break;

        }
    }// END print

    //з рядка в число
    void scan(char* src)
    {
        int temp;
        char* ptr;
        char sign=0;

        switch(manage->type){
          case SUB_NUM:
            ptr = strchr(manage->format,'%');//пошук позиції розміщення числа в рядку-форматі
            if(ptr){
                ptr = (char*)(ptr - manage->format + src);//перенесення позиціїї в рядок з самим числом
                temp = atoi(ptr);//претворення з рядка в число "цілої" частини
                if(*ptr == '-')sign = 1;
                if(manage->fract){
                    ptr = strrchr(ptr,'.')+1;//на дробову частину
                    char ii = manage->fract;
                    do temp *= 10; while(--ii);//зсув на кількість дробових знаків
                    temp += sign? -atoi(ptr) : atoi(ptr);//додаємо "дробову" частину
                }
                temp = (temp<manage->min)? manage->min:((temp>manage->max)? manage->max:temp);
                *manage->number =  temp;
            }
            break;

          case SUB_BOOL://BOOL
            ptr = strrchr(manage->format,'%');//пошук позиції розміщення числа в рядку-форматі
            if(ptr){
                ptr = (char*)(ptr - manage->format + src);//перенесення позиціїї в рядок з самим числом
                if(*ptr == 'O')*manage->number = 0;//тобто OFF
                else *manage->number = 1;//тобто ON
            }
            break;

          case SUB_PARITY:
            switch(*(strrchr(manage->format,'%')- manage->format + src+4)){
              default:
              case 'т': *manage->number = 0;break;
              case ' ': *manage->number = 1;break;
              case '1': *manage->number = 2;break;
              case '0': *manage->number = 3;break;
            }
            break;

          case SUB_TIME:
            char hour_,min,sec;

            ptr = strchr(src,':');
            hour_ = atoi(ptr-2);
            if(hour_ <= 23)*((int*)0xE0024028) = hour_;

            min = atoi(ptr+1);
            if(min <= 59)*((int*)0xE0024024) = min;

            ptr = strchr(ptr+1,':');
            sec = atoi(ptr+1);
            if(sec <= 59)*((int*)0xE0024020) = sec;
            //arch_time = new_time(N.snap_time);//наступний час запису в архів
            return;

          case SUB_DATE:
            char month_,date_;
            LPC_Rtc_Date_t pdate;

            //копіювання прочитаної дати в записувану
            RTC_GetDate(&pdate);

            ptr = strchr(src,'.');
            date_ = atoi(ptr-2);
            if((date_ <= 31)||(date_ > 0))pdate.day = date_;

            month_ = atoi(ptr+1);
            if((month_ <= 12)||(month_ > 0))pdate.month = month_;

            ptr = strchr(ptr+1,'.');
            pdate.year = atoi(ptr+1);

            RTC_SetDate(&pdate);
            //arch_time = new_time(N.snap_time);//наступний час запису в архів
            return;

          case SUB_CR_LF:
            ptr = strrchr(manage->format,'%')- manage->format + src+1;
            switch(*ptr){
              default:
              case '\377': *manage->number = 0;break;
              case 'C':  if(*(ptr+4)==0xff)*manage->number = 1; else *manage->number = 2;break;
            }
            break;

          case SUB_HEX:
            ptr = strchr(manage->format,'%');//пошук позиції розміщення числа в рядку-форматі
            if(ptr){
                ptr = (char*)(ptr - manage->format + src);//перенесення позиціїї в рядок з самим числом
                do{ptr++;}while(*ptr >='0');//пошук кінця числа
                ptr--; // назад в число
                char hex_iter,ch;
                hex_iter = 0;
                //претворення з hex в число
                temp = 0;
                do{
                    ch = *ptr;
                    temp |= ((ch>0x39)?(ch&0x0f)+9:ch&0x0f)<<hex_iter*4;
                    hex_iter++;
                    ptr--;
                }while(*ptr>='0'&&*ptr<='F');

                temp = (temp<manage->min)? manage->min:((temp>manage->max)? manage->max:temp);
                *manage->number =  temp;
            }
            break;

          case SUB_TC:
            switch(*(strrchr(manage->format,'%')- manage->format + src)){
              default:
              case '0': *manage->number = 0;break;
              case '1': *manage->number = 1;break;
            }
            break;

          case SUB_123:
            switch(*(strrchr(manage->format,'%')- manage->format + src)){
              default:
              case '0': *manage->number = 0;break;
              case '1': if(*(strrchr(manage->format,'%')- manage->format + src+1)==' ')*manage->number = 1;
                        else *manage->number = 10;
                        break;
              case '2': *manage->number = 2;break;
              case '3': *manage->number = 3;break;
              case '4': *manage->number = 4;break;
              case '5': *manage->number = 5;break;
              case '6': *manage->number = 6;break;
              case '7': *manage->number = 7;break;
              case '8': *manage->number = 8;break;
              case '9': *manage->number = 9;break;
            }
            break;

          case SUB_comp:
            switch(*(strrchr(manage->format,'%')- manage->format + src)){
              default:
              case '0': *manage->number = 0;break;
              case '1': *manage->number = 1;break;
            }
            break;
        }

        //якщо параметр знаходиться позамежами структури параметрів то не здійснювати запис у рамтрон
        if((int)manage->number<(int)&N || (int)manage->number>(int)&N+sizeof(N))return;

        spi1_init();
        while(spi1_sts_bit.end_rx == 0);

        int addr =(int)manage->number - (int)&N;
        wr_rmtrn(addr, manage->number, sizeof(int));
        while(spi1_sts_bit.end_rx == 0);
        rd_rmtrn(addr, manage->number, sizeof(int));
        while(spi1_sts_bit.end_rx == 0);

    }// end scan
};//end class SUB

/*-----------------------------------------------------------------------------------------------*/

/*************************************************************************************************/

void menu_KEY(void);
void enter_key(void);
void encoder_key(void);
void right_key(void);
void up_key(void);
void down_key(void);
void left_key(void);
void UP();
void DOWN();
void RIGHT();
void LEFT();
void UP_hex();
void DOWN_hex();
void preEnterNum(char offset);

void UP_BOOL();
void preEnterBool(char offset);

void preEnterParity(char offset);
void UP_PARITY();
void DOWN_PARITY();

char postEnterBitrate0();//Для швидкості уарт0
char postEnter_word_length0();
char postEnter_stop_bit0();
char postEnter_parity_enable0();
char postEnter_parity_select0();

char postEnterBitrate1();//Для швидкості уарт0
char postEnter_word_length1();
char postEnter_stop_bit1();
char postEnter_parity_enable1();
char postEnter_parity_select1();

char preEnter_alarm_read();
char preEnter_alarm_read1();
char preEnter_alarm_read2();
char preEnter_alarm_read3();


char preEnter_alarm_read();
char preEnter_alarm_read1();
char preEnter_alarm_read2();
char preEnter_alarm_read3();

void flash_dump_alt();
char postEnter_dump_flash();

void flash_arch_alt();
char postEnter_arch_flash();
char postEnter_arch_flash_P();

char postEnter_new_snap_time();

char postEnter_pass_replace();
char postEnter_pass_dialog();


void preEnter_ALARM_QUIT(char i);
void preEnter_ALARM_RESET(char DUMMY);


char postEnter_aui0_ws_ram();
char postEnter_aui10_ws_ram();
char postEnter_aui11_ws_ram();
char postEnter_aui13_ws_ram();
char postEnter_aui0_ws_flash();

char postEnter_aui10_ws_flash();
char postEnter_aui11_ws_flash();

char postEnter_aou_ws_ram();
char postEnter_aou_ws_Curr_ram();

char postEnter_aui1_ws();
char postEnter_aui2_ws();

char preEnter_aui0_clr();
char preEnter_aui1_clr();
char preEnter_aui2_clr();

void preEnter_clbr0();
void preEnter_clbr1();
void preEnter_clbr2();


char postEnter_ws1();

char postEnter_arch_flash_g();

void preEnter_Ident();
void preEnter123(char offset);//для 123

void UP_TC();

void UP_123();
void DOWN_123();
char postEnter_123();

void UP_calibr();
void UP_comp();




void menu();
void menu_init();



#endif //__MENU_INC
