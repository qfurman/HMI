#include "defines.h"
#include "Calc.h"

extern PARAMETRS N;


//---ДІАЛОГ ПАРОЛЬ
#define pass(NAME)\
    const MENU_MAN PASS_ ## NAME ={SUB_NUM,1,"ПАРОЛЬ ?     %06d ",&password_test,(int)& ## NAME ## _PASS_ |0x80000000,0x7fffffff,0,0,sizeof( NAME ## _PASS_)/sizeof(SUB)-1,0,preEnterNum,postEnter_pass_dialog,UP,DOWN,LEFT,RIGHT};\
    SUB PASS_ ## NAME ## _= SUB(&PASS_ ## NAME);
//---КІНЕЦЬ ДІАЛОГ ПАРОЛЬ



/*--------------------------------------------------------------------------------------------------------------------/
/   АВАРІЇ
/--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------/
/   Парoль
/--------------------------------------------------------------------------------------------------------------------*/
const MENU_MAN PASS_G[]={
    {SUB_NUM,1,"Введіть новий пароль      %06d ",&N.password,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_BOOL,1,"Сенсорна панель              %s ",    &N.touch,0,1,      0,0,0,0,preEnterBool,   postEnter_parity_enable1,   UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
//    {SUB_HEX,0,"Код запит активації     %08X ",    &N.seq.req,0x80000000,0x7fffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
//    {SUB_HEX,1,"Ключ активації 1        %08X ",    &N.seq.ans[0],0x80000000,0x7fffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
//    {SUB_HEX,1,"Ключ активації 2        %08X ",    &N.seq.ans[1],0x80000000,0x7fffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
//    {SUB_BOOL,0,"Статус                       %s ",    &N.seq.flag_ok,0,0,      0,0,0,0,preEnterBool,   postEnter_parity_enable1,   UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
//    {SUB_NUM,0,"Час                     %08d",  (int*)&N.seq.time,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"WATCHDOG                %08d ",(int*)&wd_count,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
SUB PASS_G_[]={
    SUB(&PASS_G[0]),
    SUB(&PASS_G[1]),
    SUB(&PASS_G[2]),
//    SUB(&PASS_G[3]),
//    SUB(&PASS_G[4]),
//    SUB(&PASS_G[5]),
//    SUB(&PASS_G[6]),
};

const MENU_MAN PASS[]={
                                                                //знаковий біт для усунення спроби обмеження знизу
    {SUB_NUM,1,"ВВЕДІТЬ ПАРОЛЬ      %06d ",&password_test,(int)&PASS_G_ |0x80000000,0x7fffffff,0,0,sizeof(PASS_G_)/sizeof(SUB)-1,0,preEnterNum,postEnter_pass_replace,UP,DOWN,LEFT,RIGHT}
};

SUB PASS_[]={
    SUB(&PASS[0]),
};
//ЗАГОЛОВОК
const MENU_MAN PASS__={
    SUB_NUM,0,"    Управління паролем     ",0,0,0,0,&PASS_,sizeof(PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0
};
/*--------------------------------------------------------------------------------------------------------------------/
/   ВРЕМЯ И ДАТА
/--------------------------------------------------------------------------------------------------------------------*/
const MENU_MAN DAY={
    SUB_DAY,0,"%s",0,0,0,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT
};
const MENU_MAN TIME_PASS={
    SUB_TIME,1,"    %#02d:%#02d:%#02d   ",0,0,0,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT
};
const MENU_MAN DATE_PASS={
    SUB_DATE,1,"   %#02d.%#02d.%#04d  ",0,0,0,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT
};
SUB TIME_DATE_PASS[]={
    SUB(&TIME_PASS),
    SUB(&DATE_PASS),
    SUB(&DAY),
};

//---ДІАЛОГ ПАРОЛЬ
const MENU_MAN PASS_TIME={
                                                               //знаковий біт для усунення спроби обмеження знизу
    SUB_NUM,1,"ПАРОЛЬ ?     %06d ",&password_test,(int)&TIME_DATE_PASS |0x80000000,0x7fffffff,0,0,sizeof(TIME_DATE_PASS)/sizeof(SUB)-1,0,preEnterNum,postEnter_pass_dialog,UP,DOWN,LEFT,RIGHT
};
/*
SUB PASS_TIME_[]={
    SUB(&PASS_TIME),
};
*/
//---КІНЕЦЬ ДІАЛОГ ПАРОЛЬ


const MENU_MAN TIME={
    SUB_TIME,0,"    %#02d:%#02d:%#02d   ",0,0,0,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT
};
const MENU_MAN DATE={
    SUB_DATE,0,"    %#02d.%#02d.%#04d  ",0,0,0,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT
};
/*
const MENU_MAN PASS_TIME__={
    SUB_NUM,0,"ИЗМЕНЕНИЕ ПАРАМЕТРОВ",0,0,0,0,&PASS_TIME_,sizeof(PASS_TIME_)/sizeof(SUB)-1,0,0,0,0,0,0,0
};
*/

SUB TIME_DATE[]={
    SUB(&TIME),
    SUB(&DATE),
    SUB(&DAY),
    //SUB(&PASS_TIME__),
    SUB(&PASS_TIME),
};
//ЗАГОЛОВОК

const MENU_MAN TIMEDATE={
    SUB_NUM,0,"    Дата і час                  ",0,0,0,0,&TIME_DATE,sizeof(TIME_DATE)/sizeof(SUB)-1,0,0,0,0,0,0,0
};
/*--------------------------------------------------------------------------------------------------------------------/
/   UARTs
/--------------------------------------------------------------------------------------------------------------------*/
#define uart_macro(n)\
    {SUB_NUM,   1,"Швидкість %06d бод",     &N.UART ## n.bitrate,2400,115200,     0,0,0,0,preEnterNum,    postEnterBitrate ## n,          UP, DOWN, LEFT, RIGHT},\
    {SUB_NUM,   1,"Біти даних        %01d ", &N.UART ## n.word_length,5,8, 0,0,0,0,preEnterNum,    postEnter_word_length ## n,     UP, DOWN, LEFT, RIGHT},\
    {SUB_NUM,   1,"Стоп біти         %01d ", &N.UART ## n.stop_bit,1,2, 0,0,0,0,preEnterNum,    postEnter_stop_bit ## n,        UP, DOWN, LEFT, RIGHT},\
    {SUB_BOOL,  1,"Біт парності    %s ",    &N.UART ## n.parity_enable,0,0,      0,0,0,0,preEnterBool,   postEnter_parity_enable ## n,   UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},\
    {SUB_PARITY,1,"Тип парності  %s ",      &N.UART ## n.parity_select,0,0,      0,0,0,0,preEnterParity, postEnter_parity_select ## n,   UP_PARITY, DOWN_PARITY, UP_PARITY, DOWN_PARITY},\
    {SUB_BOOL,  1,"Cимплекс режим  %s ",    &N.UART ## n.simplex,0,0,      0,0,0,0,preEnterBool,   0,   UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},\
    {SUB_NUM,   1,"Час відповіді %03d мС",  &N.UART ## n.wait_respond,1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,   1,"Пауза         %03d мС",  &N.UART ## n.timeout,1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_HEX,   1,"АДРЕСА           %02X ", &N.UART ## n.simplex,1,0xff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},

//N.UART1.simplex використовується як адреса

#define uart_macroS(n)\
    SUB(&UART ## n[0]),\
    SUB(&UART ## n[1]),\
    SUB(&UART ## n[2]),\
    SUB(&UART ## n[3]),\
    SUB(&UART ## n[4]),\
    SUB(&UART ## n[5]),\
    SUB(&UART ## n[6]),\
    SUB(&UART ## n[7]),


#define uart1_macroS(n)\
    SUB(&UART ## n[0]),\
    SUB(&UART ## n[1]),\
    SUB(&UART ## n[2]),\
    SUB(&UART ## n[3]),\
    SUB(&UART ## n[4]),\
    SUB(&UART ## n[5]),\
    SUB(&UART ## n[8]),\
    SUB(&UART ## n[6]),


const MENU_MAN UART0[]={uart_macro(0)}; SUB uart0[]={uart_macroS(0)};//UART0
const MENU_MAN UART1[]={uart_macro(1)}; SUB uart1[]={uart1_macroS(1)};//UART1

//   ЗАГОЛОВКИ ДЛЯ UART 0,1
const MENU_MAN RS485[]={
    {SUB_NUM,1,"Iнтерфейс RS422  Внутрішній         ",0,0,0,0,&uart0,sizeof(uart0)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Iнтерфейс RS485  Зовнішній          ",0,0,0,0,&uart1,sizeof(uart1)/sizeof(SUB)-1,0,0,0,0,0,0,0}
};

SUB UART_HEAD[]={
    SUB(&RS485[0]),
    SUB(&RS485[1]),
};//ЗАГОЛОВОК
const MENU_MAN UART_HEAD_HEAD={
    SUB_NUM,0,"    Послідовні інтерфейси      ",0,0,0,0,&UART_HEAD,sizeof(UART_HEAD)/sizeof(SUB)-1,0,0,0,0,0,0,0
};


/*####################################################################################################*/
/*##############################    Параметри   ######################################################*/
/*####################################################################################################*/
/*####################################################################################################*/

#define aiu_macro(n)\
    {SUB_NUM,0,"                            %06d mV",  (int*)&aiu_add[n].r.volt,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,0,"                             %05d mА", (int*)&aiu_add[n].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,0,"                             %05d Ohm", (int*)&aiu_add[n].r.Ohm,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,0,"                             %05d \260",  (int*)&aiu_add[n].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_123,0,"%s",  (int*)&N.aiu_r[n].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},\
    {SUB_comp,0,"%s",  (int*)&N.aiu_r[n].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},\
    {SUB_HEX,0,"КОД АЦП                      %06X",   (int*)&aiu[n].adc,1,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},\
    {SUB_HEX,0,"КОД АЦП середній             %06X",   (int*)&aiu[n].ADC,1,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},\
    {SUB_HEX,0,"Зміщення АЦП                 %06X",   (int*)&aiu[n].offset,0,0xffffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},\
    {SUB_HEX,0,"Підсилення АЦП               %06X",   (int*)&aiu[n].gain,0,0xffffff,0,0,0,0,  preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},\
    {SUB_NUM,0,"Коеф.Підсилювача (степінь числа2) %1d",   (int*)&aiu[n].Kg,0,0x07,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,0,"Кількість вибірок усереднення    %02d",   (int*)&aiu[n].K,0,0x40,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},

#define aiu_macroS(n)\
    SUB(&AIU_Cn ##n[0]),\
    SUB(&AIU_Cn ##n[1]),\
    SUB(&AIU_Cn ##n[2]),\
    SUB(&AIU_Cn ##n[3]),\
    SUB(&AIU_Cn ##n[4]),\
    SUB(&AIU_Cn ##n[5]),\
    SUB(&AIU_Cn ##n[6]),\
    SUB(&AIU_Cn ##n[7]),\
    SUB(&AIU_Cn ##n[8]),\
    SUB(&AIU_Cn ##n[9]),\
    SUB(&AIU_Cn ##n[10]),\
    SUB(&AIU_Cn ##n[11]),

const MENU_MAN AIU_Cn00[]={aiu_macro(0)};
SUB reg_ch00[]={aiu_macroS(00)};
const MENU_MAN AIU_Cn01[]={aiu_macro(1)};
SUB reg_ch01[]={aiu_macroS(01)};
const MENU_MAN AIU_Cn02[]={aiu_macro(2)};
SUB reg_ch02[]={aiu_macroS(02)};
const MENU_MAN AIU_Cn03[]={aiu_macro(3)};
SUB reg_ch03[]={aiu_macroS(03)};
const MENU_MAN AIU_Cn04[]={aiu_macro(4)};
SUB reg_ch04[]={aiu_macroS(04)};
const MENU_MAN AIU_Cn05[]={aiu_macro(5)};
SUB reg_ch05[]={aiu_macroS(05)};
const MENU_MAN AIU_Cn06[]={aiu_macro(6)};
SUB reg_ch06[]={aiu_macroS(06)};
const MENU_MAN AIU_Cn07[]={aiu_macro(7)};
SUB reg_ch07[]={aiu_macroS(07)};
const MENU_MAN AIU_Cn08[]={aiu_macro(8)};
SUB reg_ch08[]={aiu_macroS(08)};
const MENU_MAN AIU_Cn09[]={aiu_macro(9)};
SUB reg_ch09[]={aiu_macroS(09)};
const MENU_MAN AIU_Cn10[]={aiu_macro(10)};
SUB reg_ch10[]={aiu_macroS(10)};
const MENU_MAN AIU_Cn11[]={aiu_macro(11)};
SUB reg_ch11[]={aiu_macroS(11)};
const MENU_MAN AIU_Cn12[]={aiu_macro(12)};
SUB reg_ch12[]={aiu_macroS(12)};

/*##################################   Все тежсаме Tiльkи з паролем  #################################*/

#define aiu_macroP(n)\
    {SUB_NUM,0,"                            %06d mV",  (int*)&aiu_add[n].r.volt,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,0,"                             %05d mА", (int*)&aiu_add[n].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,0,"                             %05d Ohm", (int*)&aiu_add[n].r.Ohm,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,0,"                             %05d \260",  (int*)&aiu_add[n].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_123,1,"%s",  (int*)&N.aiu_r[n].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},\
    {SUB_comp,1,"%s",  (int*)&N.aiu_r[n].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},\
    {SUB_HEX,0,"КОД АЦП                      %06X",   (int*)&aiu[n].adc,1,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},\
    {SUB_HEX,0,"КОД АЦП середній             %06X",   (int*)&aiu[n].ADC,1,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},\
    {SUB_HEX,1,"Зміщення АЦП                 %06X",   (int*)&aiu[n].offset,0,0xffffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},\
    {SUB_HEX,1,"Підсилення АЦП               %06X",   (int*)&aiu[n].gain,0,0xffffff,0,0,0,0,  preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},\
    {SUB_NUM,1,"Коеф.Підсилювача (степінь числа2) %1d",   (int*)&aiu[n].Kg,0,0x07,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,1,"Кількість вибірок усереднення    %02d",   (int*)&aiu[n].K,0,0x40,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,1,"Запуск автокалібрування АЦП  ",   (int*)&aiu[n].sts,0,0xffff,0,0,0,0,(void(*)(char))postEnter_aui0_ws_ram,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,0,"                              %03d \260",   (int*)&aiu[n].temperature,0,0x07,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},

#define aiu_macroSP(n)\
    SUB(&AIU_CnP ##n[0]),\
    SUB(&AIU_CnP ##n[1]),\
    SUB(&AIU_CnP ##n[2]),\
    SUB(&AIU_CnP ##n[3]),\
    SUB(&AIU_CnP ##n[4]),\
    SUB(&AIU_CnP ##n[5]),\
    SUB(&AIU_CnP ##n[6]),\
    SUB(&AIU_CnP ##n[7]),\
    SUB(&AIU_CnP ##n[8]),\
    SUB(&AIU_CnP ##n[9]),\
    SUB(&AIU_CnP ##n[10]),\
    SUB(&AIU_CnP ##n[11]),\
    SUB(&AIU_CnP ##n[12]),\
    SUB(&AIU_CnP ##n[13]),

const MENU_MAN AIU_CnP00[]={aiu_macroP(0)};
SUB aiu_chP00[]={aiu_macroSP(00)};
const MENU_MAN AIU_CnP01[]={aiu_macroP(1)};
SUB aiu_chP01[]={aiu_macroSP(01)};
const MENU_MAN AIU_CnP02[]={aiu_macroP(2)};
SUB aiu_chP02[]={aiu_macroSP(02)};
const MENU_MAN AIU_CnP03[]={aiu_macroP(3)};
SUB aiu_chP03[]={aiu_macroSP(03)};
const MENU_MAN AIU_CnP04[]={aiu_macroP(4)};
SUB aiu_chP04[]={aiu_macroSP(04)};
const MENU_MAN AIU_CnP05[]={aiu_macroP(5)};
SUB aiu_chP05[]={aiu_macroSP(05)};
const MENU_MAN AIU_CnP06[]={aiu_macroP(6)};
SUB aiu_chP06[]={aiu_macroSP(06)};
const MENU_MAN AIU_CnP07[]={aiu_macroP(7)};
SUB aiu_chP07[]={aiu_macroSP(07)};
const MENU_MAN AIU_CnP08[]={aiu_macroP(8)};
SUB aiu_chP08[]={aiu_macroSP(08)};
const MENU_MAN AIU_CnP09[]={aiu_macroP(9)};
SUB aiu_chP09[]={aiu_macroSP(09)};
const MENU_MAN AIU_CnP10[]={aiu_macroP(10)};
SUB aiu_chP10[]={aiu_macroSP(10)};
const MENU_MAN AIU_CnP11[]={aiu_macroP(11)};
SUB aiu_chP11[]={aiu_macroSP(11)};
const MENU_MAN AIU_CnP12[]={aiu_macroP(12)};
SUB aiu_chP12[]={aiu_macroSP(12)};

//   ЗАГОЛОВКИ ДЛЯ каналів
const MENU_MAN aiu_chP1[]={
    {SUB_NUM,1,"Канал 1 Витрата газу",0,0,0,0,&aiu_chP00,sizeof(aiu_chP00)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 2 Витрата повітря",0,0,0,0,&aiu_chP01,sizeof(aiu_chP01)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 3 Температура газів, що відходять",0,0,0,0,&aiu_chP02,sizeof(aiu_chP02)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 4 Температура в зоні горіння",0,0,0,0,&aiu_chP03,sizeof(aiu_chP03)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 5 Температура суміші газ-повітря",0,0,0,0,&aiu_chP04,sizeof(aiu_chP04)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 6 Температура фідера",0,0,0,0,&aiu_chP05,sizeof(aiu_chP05)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 7 Температура води",0,0,0,0,&aiu_chP06,sizeof(aiu_chP06)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 8 Температура замаслювача",0,0,0,0,&aiu_chP07,sizeof(aiu_chP07)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 9 Температура фільєрної пластини",0,0,0,0,&aiu_chP08,sizeof(aiu_chP08)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 10 Витрата газу",0,0,0,0,&aiu_chP09,sizeof(aiu_chP09)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 11 Витрата повітря",0,0,0,0,&aiu_chP10,sizeof(aiu_chP10)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 12 Струм фільєрної пластини",0,0,0,0,&aiu_chP11,sizeof(aiu_chP11)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 13 Рівень базальту в печі",0,0,0,0,&aiu_chP12,sizeof(aiu_chP12)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
};

SUB aiu1P_HEAD[]={
    SUB(&aiu_chP1[0]),
    SUB(&aiu_chP1[1]),
    SUB(&aiu_chP1[2]),
    SUB(&aiu_chP1[3]),
    SUB(&aiu_chP1[4]),
    SUB(&aiu_chP1[5]),
    SUB(&aiu_chP1[6]),
    SUB(&aiu_chP1[7]),
    SUB(&aiu_chP1[8]),
    SUB(&aiu_chP1[9]),
    SUB(&aiu_chP1[10]),
    SUB(&aiu_chP1[11]),
    SUB(&aiu_chP1[12]),
};

//кінець запароленого

//   ЗАГОЛОВКИ ДЛЯ каналів
const MENU_MAN aiu_ch1[]={
    {SUB_NUM,1,"Канал 1 Витрата газу",0,0,0,0,&reg_ch00,sizeof(reg_ch00)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 2 Витрата повітря",0,0,0,0,&reg_ch01,sizeof(reg_ch01)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 3 Температура газів, що відходять",0,0,0,0,&reg_ch02,sizeof(reg_ch02)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 4 Температура в зоні горіння",0,0,0,0,&reg_ch03,sizeof(reg_ch03)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 5 Температура суміші газ-повітря",0,0,0,0,&reg_ch04,sizeof(reg_ch04)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 6 Температура фідера",0,0,0,0,&reg_ch05,sizeof(reg_ch05)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 7 Температура води",0,0,0,0,&reg_ch06,sizeof(reg_ch06)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 8 Температура замаслювача",0,0,0,0,&reg_ch07,sizeof(reg_ch07)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 9 Температура фільєрної пластини",0,0,0,0,&reg_ch08,sizeof(reg_ch08)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 10 Витрата газу",0,0,0,0,&reg_ch09,sizeof(reg_ch09)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 11 Витрата повітря",0,0,0,0,&reg_ch10,sizeof(reg_ch10)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 12 Струм фільєрної пластини",0,0,0,0,&reg_ch11,sizeof(reg_ch11)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 13 Рівень базальту в печі",0,0,0,0,&reg_ch12,sizeof(reg_ch12)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"ВВЕДІТЬ ПАРОЛЬ      %06d ",&password_test,(int)&aiu1P_HEAD |0x80000000,0x7fffffff,0,0,sizeof(aiu1P_HEAD)/sizeof(SUB)-1,0,preEnterNum,postEnter_pass_dialog,UP,DOWN,LEFT,RIGHT}
};

SUB aiu_HEAD[]={
    SUB(&aiu_ch1[0]),
    SUB(&aiu_ch1[1]),
    SUB(&aiu_ch1[2]),
    SUB(&aiu_ch1[3]),
    SUB(&aiu_ch1[4]),
    SUB(&aiu_ch1[5]),
    SUB(&aiu_ch1[6]),
    SUB(&aiu_ch1[7]),
    SUB(&aiu_ch1[8]),
    SUB(&aiu_ch1[9]),
    SUB(&aiu_ch1[10]),
    SUB(&aiu_ch1[11]),
    SUB(&aiu_ch1[12]),
    SUB(&aiu_ch1[13]),
};

const MENU_MAN AIU_HEAD_HEAD={
    SUB_NUM,0,"    Налаштування вимірювачів          ",0,0,0,0,&aiu_HEAD,sizeof(aiu_HEAD)/sizeof(SUB)-1,0,0,0,0,0,0,0
};

/*####################################################################################################*/
/*############################## індивідуальне налаштування кожного каналу ###########################*/
/*####################################################################################################*/
/*--------------------------------------------------------------*/
//струмові канали
/*--------------------------------------------------------------*/
//витрата газу запаролена
const MENU_MAN AIU_1P[]={
    {SUB_NUM,0,"Перепад газу                %06d Па",  (int*)&aiu_add[0].r.g,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"                             %05d mА",  (int*)&aiu_add[0].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Витрата газу                %06d мЗ",  (int*)&Qga.Qg,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Положення засувки вихід       %04d %%",  (int*)&aiu[9].d.meo.out,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Полож. засувки Газу завдання  %04d %%",  (int*)&aiu[9].d.meo.setup,0,1000,1,0,0,0,preEnterNum,postEnter_aui10_ws_ram,UP,DOWN,LEFT,RIGHT},
    //{SUB_NUM,1,"Максимальний час руху        %05d с",  (int*)&aiu[9].mult,0,65535,2,0,0,0,preEnterNum,postEnter_aui10_ws_flash,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Перепад що відповідає 4мА   %06d Па",  (int*)&N.aiu_r[0].g_4,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Перепад що відповідає 20мА  %06d Па",  (int*)&N.aiu_r[0].g_20,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    //{SUB_NUM,1,"Щільність газу              %04d г/мЗ",  (int*)&N.G_A.pg,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Коеф. витрати газу          %06d",  (int*)&N.G_A.kg,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB AIU_1PASS_[]={
    SUB(&AIU_1P[0]),
    SUB(&AIU_1P[1]),
    SUB(&AIU_1P[2]),
    SUB(&AIU_1P[3]),
    SUB(&AIU_1P[4]),
    SUB(&AIU_1P[5]),
    SUB(&AIU_1P[6]),
    SUB(&AIU_1P[7]),
    //SUB(&AIU_1P[8]),
    //SUB(&AIU_1P[9]),
};
//----------
//витрата газу без паролю
const MENU_MAN AIU_1[]={
    {SUB_NUM,0,"Перепад газу                %06d Па",  (int*)&aiu_add[0].r.g,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"                             %05d mА",  (int*)&aiu_add[0].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Витрата газу                %06d мЗ",  (int*)&Qga.Qg,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Положення засувки вихід       %04d %%",  (int*)&aiu[9].d.meo.out,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Полож. засувки Газу завдання  %04d %%",  (int*)&aiu[9].d.meo.setup,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    //{SUB_NUM,0,"Максимальний час руху        %05d с",  (int*)&aiu[9].mult,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Перепад що відповідає 4мА   %06d Па",  (int*)&N.aiu_r[0].g_4,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Перепад що відповідає 20мА  %06d Па",  (int*)&N.aiu_r[0].g_20,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    //{SUB_NUM,0,"Щільність газу              %04d г/мЗ",  (int*)&N.G_A.pg,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Коеф. витрати газу          %06d",  (int*)&N.G_A.kg,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB AIU_1_[]={
    SUB(&AIU_1[0]),
    SUB(&AIU_1[1]),
    SUB(&AIU_1[2]),
    SUB(&AIU_1[3]),
    SUB(&AIU_1[4]),
    SUB(&AIU_1[5]),
    SUB(&AIU_1[6]),
    SUB(&AIU_1[7]),
    //SUB(&AIU_1[8]),
    //SUB(&AIU_1[9]),
};

/*--------------------------------------------------------------*/
//витрата повітря запаролена
const MENU_MAN AIU_2P[]={
    {SUB_NUM,0,"Перепад повітря             %06d Па",  (int*)&aiu_add[1].r.g,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"                             %05d mА",  (int*)&aiu_add[1].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Витрата повітря             %06d мЗ",  (int*)&Qga.Qa,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Положення засувки вихід       %04d %%",  (int*)&aiu[10].d.meo.out,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Положення засувки завдання    %04d %%",  (int*)&aiu[10].d.meo.setup,0,0x7fffffff,1,0,0,0,preEnterNum,postEnter_aui11_ws_ram,UP,DOWN,LEFT,RIGHT},
    //{SUB_NUM,1,"Максимальний час руху        %05d с",  (int*)&aiu[10].mult,0,0x7fffffff,2,0,0,0,preEnterNum,postEnter_aui11_ws_flash,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Перепад що відповідає 4мА   %06d Па",  (int*)&N.aiu_r[1].g_4,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Перепад що відповідає 20мА  %06d Па",  (int*)&N.aiu_r[1].g_20,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    //{SUB_NUM,1,"Щільність повітря           %04d г/мЗ",  (int*)&N.G_A.pa,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Коеф. витрати повітря       %06d",  (int*)&N.G_A.ka,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Коеф.співіднош.Повітря/Газ  %06d",  (int*)&N.G_A.K,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB AIU_2PASS_[]={
    SUB(&AIU_2P[0]),
    SUB(&AIU_2P[1]),
    SUB(&AIU_2P[2]),
    SUB(&AIU_2P[3]),
    SUB(&AIU_2P[4]),
    SUB(&AIU_2P[5]),
    SUB(&AIU_2P[6]),
    SUB(&AIU_2P[7]),
    SUB(&AIU_2P[8]),
    //SUB(&AIU_2P[9]),
    //SUB(&AIU_2P[10]),
};
//----------
//витрата повітря без паролю
const MENU_MAN AIU_2[]={
    {SUB_NUM,0,"Перепад повітря             %06d Па",  (int*)&aiu_add[1].r.g,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"                             %05d mА",  (int*)&aiu_add[1].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Витрата повітря             %06d мЗ",  (int*)&Qga.Qa,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Положення засувки вихід       %04d %%",  (int*)&aiu[10].d.meo.out,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Положення засувки завдання    %04d %%",  (int*)&aiu[10].d.meo.setup,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    //{SUB_NUM,0,"Максимальний час руху        %05d с",  (int*)&aiu[10].mult,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Перепад що відповідає 4мА   %06d Па",  (int*)&N.aiu_r[1].g_4,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Перепад що відповідає 20мА  %06d Па",  (int*)&N.aiu_r[1].g_20,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    //{SUB_NUM,0,"Щільність повітря           %04d г/мЗ",  (int*)&N.G_A.pa,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Коеф. витрати повітря       %06d",  (int*)&N.G_A.ka,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Коеф.співіднош.Газ/Повітря  %06d",  (int*)&N.G_A.K,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB AIU_2_[]={
    SUB(&AIU_2[0]),
    SUB(&AIU_2[1]),
    SUB(&AIU_2[2]),
    SUB(&AIU_2[3]),
    SUB(&AIU_2[4]),
    SUB(&AIU_2[5]),
    SUB(&AIU_2[6]),
    SUB(&AIU_2[7]),
    SUB(&AIU_2[8]),
    //SUB(&AIU_2[9]),
    //SUB(&AIU_2[10]),
};

/*--------------------------------------------------------------*/
//Струм фільєрної пластини запаролений
const MENU_MAN AIU_12P[]={
    {SUB_NUM,0,"Струм фільєрної пластини    %06d А ",  (int*)&aiu_add[11].r.g,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"                             %05d mА",  (int*)&aiu_add[11].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_HEX,1,"Завдання управління струмом   %04X",  (int*)&aou.current,0,0xffff,0,0,0,0,preEnterNum,postEnter_aou_ws_ram,UP_hex,DOWN_hex,LEFT,RIGHT},
    {SUB_NUM,1,"Струм   що відповідає 4мА   %06d А ",  (int*)&N.aiu_r[11].g_4,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Струм   що відповідає 20мА  %06d А ",  (int*)&N.aiu_r[11].g_20,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB AIU_12PASS_[]={
    SUB(&AIU_12P[0]),
    SUB(&AIU_12P[1]),
    SUB(&AIU_12P[2]),
    SUB(&AIU_12P[3]),
    SUB(&AIU_12P[4]),
};
//----------------------
//Струм фільєрної пластини  без паролю
const MENU_MAN AIU_12[]={
    {SUB_NUM,0,"Струм фільєрної пластини    %06d А ",  (int*)&aiu_add[11].r.g,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"                             %05d mА",  (int*)&aiu_add[11].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Струм   що відповідає 4мА   %06d А ",  (int*)&N.aiu_r[11].g_4,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Струм   що відповідає 20мА  %06d А ",  (int*)&N.aiu_r[11].g_20,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB AIU_12_[]={
    SUB(&AIU_12[0]),
    SUB(&AIU_12[1]),
    SUB(&AIU_12[2]),
    SUB(&AIU_12[3]),
};
/*--------------------------------------------------------------*/
//рівень базальту запаролений
const MENU_MAN AIU_13P[]={
    {SUB_NUM,0,"Рівень базальту в печі      %06d мм",  (int*)&aiu_add[12].r.g,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"                             %05d mА",  (int*)&aiu_add[12].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
//    {SUB_NUM,1,"Інтенсивність вібрації         %03d %%",  (int*)&aiu[12].d.vibr.duty,0,100,0,0,0,0,preEnterNum,postEnter_aui13_ws_ram,UP,DOWN,LEFT,RIGHT},
//    {SUB_NUM,1,"Період роботи вібратора      %05d С",  (int*)&aiu[12].d.vibr.T,0,0xffff,2,0,0,0,preEnterNum,postEnter_aui13_ws_ram,UP,DOWN,LEFT,RIGHT},
//    {SUB_NUM,1,"Час активності вібратора      %04d %%",  (int*)&aiu[12].d.vibr.t,0,1000,1,0,0,0,preEnterNum,postEnter_aui13_ws_ram,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Інтенсивність вібрації         %03d %%",  (int*)&N.vibr.duty,0,100,0,0,0,0,preEnterNum,postEnter_aui13_ws_ram,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Період роботи вібратора      %05d С",  (int*)&N.vibr.T,500,0xffff,2,0,0,0,preEnterNum,postEnter_aui13_ws_ram,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час активності вібратора     %05d с",  (int*)&N.vibr.t,0,300,2,0,0,0,preEnterNum,postEnter_aui13_ws_ram,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Рівень  що відповідає 4мА   %06d мм",  (int*)&N.aiu_r[12].g_4,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Рівень  що відповідає 20мА  %06d мм",  (int*)&N.aiu_r[12].g_20,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB AIU_13PASS_[]={
    SUB(&AIU_13P[0]),
    SUB(&AIU_13P[1]),
    SUB(&AIU_13P[2]),
    SUB(&AIU_13P[3]),
    SUB(&AIU_13P[4]),
    SUB(&AIU_13P[5]),
    SUB(&AIU_13P[6]),
};

//------------------
//рівень базальту без паролю
const MENU_MAN AIU_13[]={
    {SUB_NUM,0,"Рівень базальту в печі      %06d мм",  (int*)&aiu_add[12].r.g,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"                             %05d mА",  (int*)&aiu_add[12].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Інтенсивність вібрації         %03d %%",  (int*)&N.vibr.duty,0,100,0,0,0,0,preEnterNum,postEnter_aui13_ws_ram,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Період роботи вібратора      %05d С",  (int*)&N.vibr.T,500,0xffff,2,0,0,0,preEnterNum,postEnter_aui13_ws_ram,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час активності вібратора     %05d с",  (int*)&N.vibr.t,0,300,2,0,0,0,preEnterNum,postEnter_aui13_ws_ram,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Рівень  що відповідає 4мА   %06d мм",  (int*)&N.aiu_r[12].g_4,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Рівень  що відповідає 20мА  %06d мм",  (int*)&N.aiu_r[12].g_20,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB AIU_13_[]={
    SUB(&AIU_13[0]),
    SUB(&AIU_13[1]),
    SUB(&AIU_13[2]),
    SUB(&AIU_13[3]),
    SUB(&AIU_13[4]),
    SUB(&AIU_13[5]),
    SUB(&AIU_13[6]),
};

/*--------------------------------------------------------------*/
// канали з термопарами і термоопорами
/*--------------------------------------------------------------*/
//Температура газів що відходять запаролений
const MENU_MAN AIU_3P[]={
    {SUB_NUM,0,"Температура газів що відх.   %05d \260",  (int*)&aiu_add[2].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,1,"%s",  (int*)&N.aiu_r[2].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,1,"%s",  (int*)&N.aiu_r[2].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_3PASS_[]={
    SUB(&AIU_3P[0]),
    SUB(&AIU_3P[1]),
    SUB(&AIU_3P[2]),
};

//------------------
//Температура газів що відходять без паролю
const MENU_MAN AIU_3[]={
    {SUB_NUM,0,"Температура газів що відх.   %05d \260",  (int*)&aiu_add[2].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,0,"%s",  (int*)&N.aiu_r[2].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,0,"%s",  (int*)&N.aiu_r[2].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_3_[]={
    SUB(&AIU_3[0]),
    SUB(&AIU_3[1]),
    SUB(&AIU_3[2]),
};
/*--------------------------------------------------------------*/
//Температура в зоні горіння запаролений
const MENU_MAN AIU_4P[]={
    {SUB_NUM,0,"Температура в зоні горіння   %05d \260",  (int*)&aiu_add[3].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,1,"%s",  (int*)&N.aiu_r[3].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,1,"%s",  (int*)&N.aiu_r[3].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_4PASS_[]={
    SUB(&AIU_4P[0]),
    SUB(&AIU_4P[1]),
    SUB(&AIU_4P[2]),
};

//------------------
//без паролю
const MENU_MAN AIU_4[]={
    {SUB_NUM,0,"Температура в зоні горіння   %05d \260",  (int*)&aiu_add[3].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,0,"%s",  (int*)&N.aiu_r[3].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,0,"%s",  (int*)&N.aiu_r[3].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_4_[]={
    SUB(&AIU_4[0]),
    SUB(&AIU_4[1]),
    SUB(&AIU_4[2]),
};
/*--------------------------------------------------------------*/
//Температура газоповітряної суміші запаролений
const MENU_MAN AIU_5P[]={
    {SUB_NUM,0,"Температура суміші газ-пов.  %05d \260",  (int*)&aiu_add[4].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,1,"%s",  (int*)&N.aiu_r[4].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,1,"%s",  (int*)&N.aiu_r[4].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_5PASS_[]={
    SUB(&AIU_5P[0]),
    SUB(&AIU_5P[1]),
    SUB(&AIU_5P[2]),
};

//------------------
//без паролю
const MENU_MAN AIU_5[]={
    {SUB_NUM,0,"Температура суміші газ-пов.  %05d \260",  (int*)&aiu_add[4].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,0,"%s",  (int*)&N.aiu_r[4].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,0,"%s",  (int*)&N.aiu_r[4].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_5_[]={
    SUB(&AIU_5[0]),
    SUB(&AIU_5[1]),
    SUB(&AIU_5[2]),
};
/*--------------------------------------------------------------*/
//Температура фідера запаролений
const MENU_MAN AIU_6P[]={
    {SUB_NUM,0,"Температура фідера           %05d \260",  (int*)&aiu_add[5].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,1,"%s",  (int*)&N.aiu_r[5].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,1,"%s",  (int*)&N.aiu_r[5].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_6PASS_[]={
    SUB(&AIU_6P[0]),
    SUB(&AIU_6P[1]),
    SUB(&AIU_6P[2]),
};

//------------------
//без паролю
const MENU_MAN AIU_6[]={
    {SUB_NUM,0,"Температура фідера           %05d \260",  (int*)&aiu_add[5].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,0,"%s",  (int*)&N.aiu_r[5].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,0,"%s",  (int*)&N.aiu_r[5].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_6_[]={
    SUB(&AIU_6[0]),
    SUB(&AIU_6[1]),
    SUB(&AIU_6[2]),
};
/*--------------------------------------------------------------*/
//Температура води запаролений
const MENU_MAN AIU_7P[]={
    {SUB_NUM,0,"Температура води             %05d \260",  (int*)&aiu_add[6].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,1,"%s",  (int*)&N.aiu_r[6].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,1,"%s",  (int*)&N.aiu_r[6].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_7PASS_[]={
    SUB(&AIU_7P[0]),
    SUB(&AIU_7P[1]),
    SUB(&AIU_7P[2]),
};

//------------------
//без паролю
const MENU_MAN AIU_7[]={
    {SUB_NUM,0,"Температура води             %05d \260",  (int*)&aiu_add[6].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,0,"%s",  (int*)&N.aiu_r[6].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,0,"%s",  (int*)&N.aiu_r[6].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_7_[]={
    SUB(&AIU_7[0]),
    SUB(&AIU_7[1]),
    SUB(&AIU_7[2]),
};
/*--------------------------------------------------------------*/
//Температура замаслювача запаролений
const MENU_MAN AIU_8P[]={
    {SUB_NUM,0,"Температура замаслювача      %05d \260",  (int*)&aiu_add[7].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,1,"%s",  (int*)&N.aiu_r[7].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,1,"%s",  (int*)&N.aiu_r[7].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_8PASS_[]={
    SUB(&AIU_8P[0]),
    SUB(&AIU_8P[1]),
    SUB(&AIU_8P[2]),
};

//------------------
//без паролю
const MENU_MAN AIU_8[]={
    {SUB_NUM,0,"Температура замаслювача      %05d \260",  (int*)&aiu_add[7].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,0,"%s",  (int*)&N.aiu_r[7].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,0,"%s",  (int*)&N.aiu_r[7].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_8_[]={
    SUB(&AIU_8[0]),
    SUB(&AIU_8[1]),
    SUB(&AIU_8[2]),
};
/*--------------------------------------------------------------*/
//Температура фільєрної пластини запаролений
const MENU_MAN AIU_9P[]={
    {SUB_NUM,0,"Температура фільєрної пласт. %05d \260",  (int*)&aiu_add[8].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,1,"%s",  (int*)&N.aiu_r[8].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,1,"%s",  (int*)&N.aiu_r[8].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_9PASS_[]={
    SUB(&AIU_9P[0]),
    SUB(&AIU_9P[1]),
    SUB(&AIU_9P[2]),
};

//------------------
//без паролю
const MENU_MAN AIU_9[]={
    {SUB_NUM,0,"Температура фільєрної пласт. %05d \260",  (int*)&aiu_add[8].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_123,0,"%s",  (int*)&N.aiu_r[8].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
    {SUB_comp,0,"%s",  (int*)&N.aiu_r[8].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
};

SUB AIU_9_[]={
    SUB(&AIU_9[0]),
    SUB(&AIU_9[1]),
    SUB(&AIU_9[2]),
};
/*--------------------------------------------------------------*/


//   ЗАГОЛОВКИ ДЛЯ каналів
const MENU_MAN aiu_P1[]={
    {SUB_NUM,1,"Канал 1 (+10) Витрата газу",0,0,0,0,&AIU_1PASS_,sizeof(AIU_1PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 2 (+11)Витрата повітря",0,0,0,0,&AIU_2PASS_,sizeof(AIU_2PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 3 Температура газів, що відходять",0,0,0,0,&AIU_3PASS_,sizeof(AIU_3PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 4 Температура в зоні горіння",0,0,0,0,&AIU_4PASS_,sizeof(AIU_4PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 5 Температура суміші газ-повітря",0,0,0,0,&AIU_5PASS_,sizeof(AIU_5PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 6 Температура фідера",0,0,0,0,&AIU_6PASS_,sizeof(AIU_6PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 7 Температура води",0,0,0,0,&AIU_7PASS_,sizeof(AIU_7PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 8 Температура замаслювача",0,0,0,0,&AIU_8PASS_,sizeof(AIU_8PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 9 Температура фільєрної пластини",0,0,0,0,&AIU_9PASS_,sizeof(AIU_9PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 12 Струм фільєрної пластини",0,0,0,0,&AIU_12PASS_,sizeof(AIU_12PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 13 Рівень базальту в печі",0,0,0,0,&AIU_13PASS_,sizeof(AIU_13PASS_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
};

SUB aiuP_HEAD[]={
    SUB(&aiu_P1[0]),
    SUB(&aiu_P1[1]),
    SUB(&aiu_P1[2]),
    SUB(&aiu_P1[3]),
    SUB(&aiu_P1[4]),
    SUB(&aiu_P1[5]),
    SUB(&aiu_P1[6]),
    SUB(&aiu_P1[7]),
    SUB(&aiu_P1[8]),
    SUB(&aiu_P1[9]),
    SUB(&aiu_P1[10]),
};


//кінець запароленого


//   ЗАГОЛОВКИ ДЛЯ каналів
const MENU_MAN aiu_1[]={
    {SUB_NUM,1,"Канал 1 (+10)Витрата газу",0,0,0,0,&AIU_1_,sizeof(AIU_1_)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0},
    {SUB_NUM,1,"Канал 2 (+11)Витрата повітря",0,0,0,0,&AIU_2_,sizeof(AIU_2_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 3 Температура газів, що відходять",0,0,0,0,&AIU_3_,sizeof(AIU_3_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 4 Температура в зоні горіння",0,0,0,0,&AIU_4_,sizeof(AIU_4_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 5 Температура суміші газ-повітря",0,0,0,0,&AIU_5_,sizeof(AIU_5_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 6 Температура фідера",0,0,0,0,&AIU_6_,sizeof(AIU_6_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 7 Температура води",0,0,0,0,&AIU_7_,sizeof(AIU_7_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 8 Температура замаслювача",0,0,0,0,&AIU_8_,sizeof(AIU_8_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 9 Температура фільєрної пластини",0,0,0,0,&AIU_9_,sizeof(AIU_9_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 12 Струм фільєрної пластини",0,0,0,0,&AIU_12_,sizeof(AIU_12_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Канал 13 Рівень базальту в печі",0,0,0,0,&AIU_13_,sizeof(AIU_13_)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Введіть пароль доступу    %06d",&password_test,(int)&aiuP_HEAD |0x80000000,0x7fffffff,0,0,sizeof(aiuP_HEAD)/sizeof(SUB)-1,0,preEnterNum,postEnter_pass_dialog,UP,DOWN,LEFT,RIGHT}
};

SUB aiu1_HEAD[]={
    SUB(&aiu_1[0]),
    SUB(&aiu_1[1]),
    SUB(&aiu_1[2]),
    SUB(&aiu_1[3]),
    SUB(&aiu_1[4]),
    SUB(&aiu_1[5]),
    SUB(&aiu_1[6]),
    SUB(&aiu_1[7]),
    SUB(&aiu_1[8]),
    SUB(&aiu_1[9]),
    SUB(&aiu_1[10]),
    SUB(&aiu_1[11]),
};

const MENU_MAN AIU1_HEAD_HEAD={
    SUB_NUM,0,"    ПАРАМЕТРИ СИСТЕМИ                 ",0,0,0,0,&aiu1_HEAD,sizeof(aiu1_HEAD)/sizeof(SUB)-1,0,0,0,0,0,0,0
};

/*####################################################################################################*/
//регулятор струму eбез паролю

const MENU_MAN PID_menu[]={
    {SUB_NUM,1,"Завдання Струму Фільєри      %05d А",  (int*)&N.pid[0].Sp,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Завдання поточне             %05d А",  (int*)&pid_r[0].Sp,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0," Вхід                        %05d А",  (int*)&aiu_add[11].r.g,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1," Вихід                       %05d mА",  (int*)&aou_curr,0,20000,3,0,0,0,preEnterNum,postEnter_aou_ws_Curr_ram,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Коеф. пропорційності         %04d ",  (int*)&N.pid[0].Kp,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час інтегрування             %04d ",  (int*)&N.pid[0].Ti,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час диференціювання          %04d ",  (int*)&N.pid[0].Td,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Період квантування           %04d ",  (int*)&N.pid[0].Ts,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Діапазон зміни рег. парам.   %05d А",  (int*)&N.pid[0].Range,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Діапазон зміни вих. сигналу  %05d ",  (int*)&N.pid[0].Out_Range,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Швидкість зміни завдання     %05d А/с",  (int*)&N.pid[0].Speed,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Мін. значення вих. сигналу   %05d %%",  (int*)&N.pid[0].Out_min,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Макс.значення вих. сигналу   %05d %%",  (int*)&N.pid[0].Out_max,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Аварійний поріг верхній      %05d А",  (int*)&N.pid[0].Alarm_h,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Аварійний поріг нижній       %05d А",  (int*)&N.pid[0].Alarm_l,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_BOOL,1,"Регулятор Вкл./Викл.         %s",  (int*)&N.pid[0].on_off,0,0,0,0,0,0,preEnterBool,0,UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
};

SUB PID_menu_[]={
    SUB(&PID_menu[0]),
    SUB(&PID_menu[1]),
    SUB(&PID_menu[2]),
    SUB(&PID_menu[3]),
    SUB(&PID_menu[4]),
    SUB(&PID_menu[5]),
    SUB(&PID_menu[6]),
    SUB(&PID_menu[7]),
    SUB(&PID_menu[8]),
    SUB(&PID_menu[9]),
    SUB(&PID_menu[10]),
    SUB(&PID_menu[11]),
    SUB(&PID_menu[12]),
    SUB(&PID_menu[13]),
    SUB(&PID_menu[14]),
    SUB(&PID_menu[15]),
};


const MENU_MAN PID_HEAD={
    SUB_NUM,0,"    РЕГУЛЯТОР СТРУМУ                  ",0,0,0,0,&PID_menu_,sizeof(PID_menu_)/sizeof(SUB)-1,0,0,0,0,0,0,0
};

/*--------------------------------------------------------------*/
/*####################################################################################################*/
/*####################################################################################################*/
//Регулятор витрати газу без паролю

const MENU_MAN PID1_menu[]={
    {SUB_NUM,1,"Завдання температури в печі  %05d \260",  (int*)&N.pid[1].Sp,1,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Завдання поточне             %05d \260",  (int*)&pid_r[1].Sp,1,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0," Вхід                        %05d \260",  (int*)&Qga.T_RG,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Положення засувки вихід       %04d %%",  (int*)&aiu[9].d.meo.out,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Коеф. пропорційності         %04d ",  (int*)&N.pid[1].Kp,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час інтегрування             %04d ",  (int*)&N.pid[1].Ti,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час диференціювання          %04d ",  (int*)&N.pid[1].Td,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Період квантування           %04d ",  (int*)&N.pid[1].Ts,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Діапазон зміни рег. парам.   %05d \260",  (int*)&N.pid[1].Range,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Діапазон зміни вих. сигналу  %05d ",  (int*)&N.pid[1].Out_Range,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Швидкість зміни завдання     %05d /с",  (int*)&N.pid[1].Speed,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Мін. значення вих. сигналу   %05d %%",  (int*)&N.pid[1].Out_min,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Макс.значення вих. сигналу   %05d %%",  (int*)&N.pid[1].Out_max,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Аварійний поріг верхній      %05d \260",  (int*)&N.pid[1].Alarm_h,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Аварійний поріг нижній       %05d \260",  (int*)&N.pid[1].Alarm_l,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_TC,1,"%s",  (int*)&N.pid[1].TC,0,0,0,0,0,0,preEnter123,0,UP_TC, UP_TC, UP_TC, UP_TC},
    {SUB_BOOL,1,"Регулятор Вкл./Викл.         %s",  (int*)&N.pid[1].on_off,0,0,0,0,0,0,preEnterBool,0,UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
};

SUB PID1_menu_[]={
    SUB(&PID1_menu[0]),
    SUB(&PID1_menu[1]),
    SUB(&PID1_menu[2]),
    SUB(&PID1_menu[3]),
    SUB(&PID1_menu[4]),
    SUB(&PID1_menu[5]),
    SUB(&PID1_menu[6]),
    SUB(&PID1_menu[7]),
    SUB(&PID1_menu[8]),
    SUB(&PID1_menu[9]),
    SUB(&PID1_menu[10]),
    SUB(&PID1_menu[11]),
    SUB(&PID1_menu[12]),
    SUB(&PID1_menu[13]),
    SUB(&PID1_menu[14]),
    SUB(&PID1_menu[15]),
    SUB(&PID1_menu[16]),
};


const MENU_MAN PID1_HEAD={
    SUB_NUM,0,"    РЕГУЛЯТОР ВИТРАТИ ГАЗУ            ",0,0,0,0,&PID1_menu_,sizeof(PID1_menu_)/sizeof(SUB)-1,0,0,0,0,0,0,0
};

/*--------------------------------------------------------------*/
const MENU_MAN AUTO_menu[]={
    {SUB_BOOL,1,"Автоматичний вихід на режим  %s",  (int*)&N.R.auto_on,0,0,0,0,0,0,preEnterBool,0,UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
    {SUB_NUM,1,"Холодний час 1               %05d с",  (int*)&N.R.t1,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Холодний струм 1             %05d А",  (int*)&N.R.c1,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Холодний час 2               %05d с",  (int*)&N.R.t2,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Холодний струм 2             %05d А",  (int*)&N.R.c2,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Холодний час 3               %05d с",  (int*)&N.R.t3,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"До 5 хв. час 1               %05d с",  (int*)&N.R.t5_1,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"До 5 хв. струм 1             %05d А",  (int*)&N.R.c5,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"До 5 хв. час 2               %05d с",  (int*)&N.R.t5_2,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"До 1 хв. час 1               %05d с",  (int*)&N.R.t_1,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_BOOL,1,"Запуск охолодження фільєри   %s",  (int*)&Flr.down_ramp,0,0,0,0,0,0,preEnterBool,0,UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
    {SUB_NUM,1,"Час охолодження 1            %05d с",  (int*)&N.R.t_b1,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Струм охолодження 1          %05d с",  (int*)&N.R.c_b1,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час охолодження 2            %05d с",  (int*)&N.R.t_b2,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Струм охолодження 2          %05d с",  (int*)&N.R.c_b2,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час охолодження 3            %05d с",  (int*)&N.R.t_b3,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB AUTO_menu_[]={
    SUB(&AUTO_menu[0]),
    SUB(&AUTO_menu[1]),
    SUB(&AUTO_menu[2]),
    SUB(&AUTO_menu[3]),
    SUB(&AUTO_menu[4]),
    SUB(&AUTO_menu[5]),
    SUB(&AUTO_menu[6]),
    SUB(&AUTO_menu[7]),
    SUB(&AUTO_menu[8]),
    SUB(&AUTO_menu[9]),
    SUB(&AUTO_menu[10]),
    SUB(&AUTO_menu[11]),
    SUB(&AUTO_menu[12]),
    SUB(&AUTO_menu[13]),
    SUB(&AUTO_menu[14]),
    SUB(&AUTO_menu[15]),
};


const MENU_MAN AUTO_HEAD={
    SUB_NUM,0,"    ПАРАМЕТРИ АВТОМАТИЧНОГО ЗАПУСКУ   ",0,0,0,0,&AUTO_menu_,sizeof(AUTO_menu_)/sizeof(SUB)-1,0,0,0,0,0,0,0
};


/*####################################################################################################*/
//Регулятор витрати повітря без паролю

const MENU_MAN PID2_menu[]={
    {SUB_NUM,1,"Завдання                     %05d мЗ",  (int*)&N.pid[2].Sp,2,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Завдання поточне             %05d мЗ",  (int*)&pid_r[2].Sp,2,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0," Вхід                        %05d мЗ",  (int*)&Qga.Qa,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Положення засувки вихід       %04d %%",  (int*)&aiu[10].d.meo.out,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Коеф. пропорційності         %04d ",  (int*)&N.pid[2].Kp,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час інтегрування             %04d ",  (int*)&N.pid[2].Ti,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час диференціювання          %04d ",  (int*)&N.pid[2].Td,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Період квантування           %04d ",  (int*)&N.pid[2].Ts,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Діапазон зміни рег. парам.   %05d мЗ",  (int*)&N.pid[2].Range,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Діапазон зміни вих. сигналу  %05d ",  (int*)&N.pid[2].Out_Range,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Швидкість зміни завдання     %05d /с",  (int*)&N.pid[2].Speed,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Мін. значення вих. сигналу   %05d %%",  (int*)&N.pid[2].Out_min,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Макс.значення вих. сигналу   %05d %%",  (int*)&N.pid[2].Out_max,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Аварійний поріг верхній      %05d м3",  (int*)&N.pid[2].Alarm_h,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Аварійний поріг нижній       %05d м3",  (int*)&N.pid[2].Alarm_l,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_BOOL,1,"Регулятор Вкл./Викл.         %s",  (int*)&N.pid[2].on_off,0,0,0,0,0,0,preEnterBool,0,UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
    {SUB_BOOL,1,"Робота за витратою газу      %s",  (int*)&N.pid[2].cascad,0,0,0,0,0,0,preEnterBool,0,UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
};

SUB PID2_menu_[]={
    SUB(&PID2_menu[0]),
    SUB(&PID2_menu[1]),
    SUB(&PID2_menu[2]),
    SUB(&PID2_menu[3]),
    SUB(&PID2_menu[4]),
    SUB(&PID2_menu[5]),
    SUB(&PID2_menu[6]),
    SUB(&PID2_menu[7]),
    SUB(&PID2_menu[8]),
    SUB(&PID2_menu[9]),
    SUB(&PID2_menu[10]),
    SUB(&PID2_menu[11]),
    SUB(&PID2_menu[12]),
    SUB(&PID2_menu[13]),
    SUB(&PID2_menu[14]),
    SUB(&PID2_menu[15]),
    SUB(&PID2_menu[16]),
};


const MENU_MAN PID2_HEAD={
    SUB_NUM,0,"    РЕГУЛЯТОР ВИТРАТИ ПОВІТРЯ         ",0,0,0,0,&PID2_menu_,sizeof(PID2_menu_)/sizeof(SUB)-1,0,0,0,0,0,0,0
};

/*--------------------------------------------------------------*/
/*####################################################################################################*/
SUB USER_menu_[]={
    SUB(&AIU_13P[2]),
    SUB(&AIU_13P[3]),
    SUB(&AIU_13P[4]),
    SUB(&PID_menu[0]),
    SUB(&PID1_menu[0]),
    SUB(&AIU_1P[4]),
};


const MENU_MAN USER_HEAD={
    SUB_NUM,0,"    МЕНЮ З ПАРАМЕТРАМИ ДЛЯ ОПЕРАТОРА  ",0,0,0,0,&USER_menu_,sizeof(USER_menu_)/sizeof(SUB)-1,0,0,0,0,0,0,0
};

/*--------------------------------------------------------------*/
/*####################################################################################################*/
/*--------------------------------------------------------------------------------------------------------------------/
/   прегляд флеш
/--------------------------------------------------------------------------------------------------------------------*/
const MENU_MAN FLASH_DUMP_PASS[]={
    {SUB_NUM,1,"Період архівування         %04d с",(int*)&N.snap_time,1,0x7fffffff,0,0,0,0,preEnterNum,postEnter_new_snap_time,UP,DOWN,LEFT,RIGHT},
//    {SUB_NUM,1,"Адрес следующей записи     %06X ",(int*)&arch_flash_addr,0,0x7ff00,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
//    {SUB_NUM,1,"WATCHDOG %010d ",(int*)&wd_count,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT}
};
SUB FLASH_DUMP_PASS_[]={
    SUB(&FLASH_DUMP_PASS[0]),
//    SUB(&FLASH_DUMP_PASS[1]),
//    SUB(&FLASH_DUMP_PASS[2]),
};



const MENU_MAN FLASH_DUMP[]={
    {SUB_NUM,0,"Період архівування         %04d с",(int*)&N.snap_time,1,0x7fffffff,0,0,0,0,preEnterNum,postEnter_new_snap_time,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Час до наступного запису   %04d с",(int*)&time_to_arch,3,255,0,0,0,0,preEnterNum,postEnter_new_snap_time,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"ЗМІНА ПАРАМЕТРІВ ЧЕРЕЗ ПАРОЛЬ %06d ",&password_test,(int)&FLASH_DUMP_PASS_ |0x80000000,0x7fffffff,0,0,sizeof(FLASH_DUMP_PASS_)/sizeof(SUB)-1,0,preEnterNum,postEnter_pass_dialog,UP,DOWN,LEFT,RIGHT}
};


SUB FLASH_DUMP_[]={
    SUB(&FLASH_DUMP[0]),
    SUB(&FLASH_DUMP[1]),
    SUB(&FLASH_DUMP[2]),
    //SUB(&FLASH_DUMP[3]),
    //SUB(&FLASH_DUMP[4]),
    //SUB(&FLASH_DUMP[5]),
//    SUB(&FLASH_DUMP[6]),
};
//ЗАГОЛОВОК
const MENU_MAN FLASH_DUMP__={
    SUB_NUM,0,"    ПАРАМЕТРИ АРХІВУВАННЯ ",0,0,0,0,&FLASH_DUMP_,sizeof(FLASH_DUMP_)/sizeof(SUB)-1,0,0,0,0,0,0,0
};

/*--------------------------------------------------------------------------------------------------------------------/
/   графіки
/--------------------------------------------------------------------------------------------------------------------*/



//ЗАГОЛОВОК
const MENU_MAN FLASH_DUMP__G[]={
    //SUB_NUM,0,"    ПРОСМОТР АРХИВА В ВИДЕ ГРАФИКОВ",0,0,0,0,&FLASH_DUMP_G_,sizeof(FLASH_DUMP_G_)/sizeof(SUB)-1,0,0,0,0,0,0,0
    {SUB_NUM,2,"ПЕРЕГЛЯД АРХІВУ З ОСТАННЬОГО ЗАПИСУ",0,0,0,0,0,0,0,(void(*)(char))postEnter_arch_flash_g,0,0,0,0,0},
    {SUB_NUM,1,"ПЕРЕГЛЯД АРХІВУ-СТОРІНКА(0-16383) %05d",(int*)&arch_page,0,16383,0,0,0,0,preEnterNum,postEnter_arch_flash_P,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Сторінка наступного запису        %05d",(int*)&next_arch_page,0,0,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_HEX,1,"ПЕРЕГЛЯД АРХІВУ-АДРЕСА           %06X ",(int*)&N.archive_addr,0,0x0007ffff,0,0,0,0,preEnterNum,postEnter_arch_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
    {SUB_NUM,0,"Адреса наступного запису         %06X ",(int*)&arch_flash_addr,0,0,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

//витрата газу
const MENU_MAN scal_Gg[]={
    {SUB_NUM,1,"Макс. Ордината графіка   %04d мЗ",   (int*)&N.full[0],0,0xffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Мін.  Ордината графіка   %04d мЗ",   (int*)&N.nill[0],0x8000ffff,0xffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Число градуювальних відміток великих %1d ",   (int*)&N.scal[0].big,1,5,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Число градуювальних відміток малих  %02d ",  (int*)&N.scal[0].small,1,10,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB scal_GgS[]={
    SUB(&scal_Gg[0]),
    SUB(&scal_Gg[1]),
    SUB(&scal_Gg[2]),
    SUB(&scal_Gg[3]),
};
//витрата повітря
const MENU_MAN scal_Ga[]={
    {SUB_NUM,1,"Макс. Ордината графіка   %04d мЗ",   (int*)&N.full[1],0,0xffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Мін.  Ордината графіка   %04d мЗ",   (int*)&N.nill[1],0x8000ffff,0xffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Число градуювальних відміток великих %1d ",   (int*)&N.scal[1].big,1,5,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Число градуювальних відміток малих  %02d ",  (int*)&N.scal[1].small,1,10,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
SUB scal_GaS[]={
    SUB(&scal_Ga[0]),
    SUB(&scal_Ga[1]),
    SUB(&scal_Ga[2]),
    SUB(&scal_Ga[3]),
};

#define scal_macro(n)\
    {SUB_NUM,1,"Макс. Ордината графіка   %05d \260",   (int*)&N.full ## [n],0,0xffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,1,"Мін.  Ордината графіка  %+06d \260",   (int*)&N.nill ## [n],0x8000ffff,0xffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,1,"Число градуювальних відміток великих %1d ",   (int*)&N.scal ## [n] ## .big,1,5,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,1,"Число градуювальних відміток малих  %02d ",  (int*)&N.scal ## [n] ## .small,1,10,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\

#define scal_macroS(n)\
    SUB(&REG_Cn ##n[0]),\
    SUB(&REG_Cn ##n[1]),\
    SUB(&REG_Cn ##n[2]),\
    SUB(&REG_Cn ##n[3]),

const MENU_MAN REG_Cn02[]={scal_macro(2)};
SUB scal02[]={scal_macroS(02)};
const MENU_MAN REG_Cn03[]={scal_macro(3)};
SUB scal03[]={scal_macroS(03)};
const MENU_MAN REG_Cn04[]={scal_macro(4)};
SUB scal04[]={scal_macroS(04)};

//струм
const MENU_MAN scal_I[]={
    {SUB_NUM,1,"Макс. Ордината графіка   %04d А",   (int*)&N.full[5],0,0xffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Мін.  Ордината графіка   %04d А",   (int*)&N.nill[5],0x8000ffff,0xffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Число градуювальних відміток великих %1d ",   (int*)&N.scal[5].big,1,5,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Число градуювальних відміток малих  %02d ",  (int*)&N.scal[5].small,1,10,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
SUB scal_IS[]={
    SUB(&scal_I[0]),
    SUB(&scal_I[1]),
    SUB(&scal_I[2]),
    SUB(&scal_I[3]),
};

//період вібрації
const MENU_MAN scal_V[]={
    {SUB_NUM,1,"Макс. Ордината графіка   %05d с",   (int*)&N.full[6],0,0xffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Мін.  Ордината графіка   %05d с",   (int*)&N.nill[6],0x8000ffff,0xffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Число градуювальних відміток великих %1d ",   (int*)&N.scal[6].big,1,5,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Число градуювальних відміток малих  %02d ",  (int*)&N.scal[6].small,1,10,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};

SUB scal_VS[]={
    SUB(&scal_V[0]),
    SUB(&scal_V[1]),
    SUB(&scal_V[2]),
    SUB(&scal_V[3]),
};


//   ЗАГОЛОВКИ ДЛЯ каналів
const MENU_MAN scal1[]={
    {SUB_NUM,1,"Параметри графіка витрати газу",0,0,0,0,&scal_GgS,sizeof(scal_GgS)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Параметри графіка витрати повітря",0,0,0,0,&scal_GaS,sizeof(scal_GaS)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Параметри графіка температури диму",0,0,0,0,&scal02,sizeof(scal02)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Параметри графіка температури суміші",0,0,0,0,&scal03,sizeof(scal03)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Параметри графіка температури з.фідера",0,0,0,0,&scal04,sizeof(scal04)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Параметри графіка струму",0,0,0,0,&scal_IS,sizeof(scal_IS)/sizeof(SUB)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"Параметри графіка періоду вібрації",0,0,0,0,&scal_VS,sizeof(scal_VS)/sizeof(SUB)-1,0,0,0,0,0,0,0},
};

SUB scal1_HEAD[]={
    SUB(&FLASH_DUMP__G[0]),
    SUB(&FLASH_DUMP__G[1]),
    SUB(&FLASH_DUMP__G[2]),
    SUB(&FLASH_DUMP__G[3]),
    SUB(&FLASH_DUMP__G[4]),
    SUB(&scal1[0]),
    SUB(&scal1[1]),
    SUB(&scal1[2]),
    SUB(&scal1[3]),
    SUB(&scal1[4]),
    SUB(&scal1[5]),
    SUB(&scal1[6]),
    SUB(&FLASH_DUMP__),
};

const MENU_MAN scal1_HEAD_HEAD={
    SUB_NUM,0,"    ПЕРЕГЛЯД АРХІВУ                      ",0,0,0,0,&scal1_HEAD,sizeof(scal1_HEAD)/sizeof(SUB)-1,0,(void(*)(char))preEnter_Ident,0,0,0,0,0
};

/*####################################################################################################*/
/*####################################################################################################*/
/*####################################################################################################*/




/*--------------------------------------------------------------------------------------------------------------------/
/   КОРЕНЕВЕ МЕНЮ
/--------------------------------------------------------------------------------------------------------------------*/
SUB menu0[]={
    SUB(&USER_HEAD),
    SUB(&AIU1_HEAD_HEAD),
    SUB(&PID_HEAD),
    SUB(&PID1_HEAD),
    SUB(&PID2_HEAD),
    SUB(&AUTO_HEAD),
    SUB(&AIU_HEAD_HEAD),
    SUB(&scal1_HEAD_HEAD),
    SUB(&UART_HEAD_HEAD),
    SUB(&TIMEDATE),
    SUB(&PASS__),




};
/*--------------------------------------------------------------------------------------------------------------------/
/   КІНЕЦЬ МЕНЮ
/--------------------------------------------------------------------------------------------------------------------*/


