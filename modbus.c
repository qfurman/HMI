#include <iolpc2138.h>
#include "type.h"
#include "macros.h"
#include "uart_0.h"
#include "uart_1.h"
#include "menu.hpp"
#include "timers.h"
#include "main.hpp"

#define modbufsize 256

//декларація атрибутів модбасу

#pragma pack(1)
union{
    char modbuf[modbufsize];

    struct{
        char address;
        char function;
        char exeption;
    }exception;//повідомлення про виключну ситуацію

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }read_req;//читання Запит

    struct{
        char address;
        char function;
        char byte_count;
        unsigned short reg[125];
    }read_resp;//читання відповідь

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_req;//запис одного регістра Запит

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_resp;//запис одного регістра відповідь

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
        unsigned char byte_count;
        unsigned short reg[123];
    }write_mult_req;//запис регістрів Запит

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }write_mult_resp;//запис регістрів відповідь

}buf[2];

//регістри модбаса

struct COMM{
    unsigned short addr   :   4;
    unsigned short req    :   4;    //0-read 1-write
    unsigned short reserv :   4;
    unsigned short status :   4;    //1-busy 2-succes 3-error
};
typedef COMM COMM;


void error();
void start_transmit();
void transmit_prepare(char count);

#define litle_endian(n) ((char)((n)>>8)|(char)(n)<<8)
#define big_endian(n) ((char)((n)>>8)|(char)(n)<<8)




//для внутрішнього інтерфейсу --------------------------------

const char aou_tx[6]={0x0e,0x03,0x00,0x00,0x00,0x03};

bool all = 0;//писати всі в т ч таймери
bool all_r = 0;//запит було відправлено
bool request_type;//тип читати чи писати
bool w_s_error;//помилка

char device;
//організація обміну з аналоговими модулями
/* на кожен виділяється по два біта в задачах і запитах, аля AIU можливо три варіатни обміну -
звичайний, запис оперативних параметрів і запис постійних параметрів і запис оперативних і постійних одночасно
Для AOU - звичайний і запис одного вихідного параметру
всього задіяно 28 бітів
*/
struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//девайси що не відповідають належним чином
    unsigned short err_cntr;//лічильник помилок

}aum;

//для внутрішнього інтерфейсу кінець --------------------------------



/*-------------------------------------------------------------------------------------------------/

    ця частина обслуговує внутрішній RS485/RS422 інтерфейс

/-------------------------------------------------------------------------------------------------*/

char modbus_exchange_count;

void modbus_0()
{
    char i;
    int *i_ptr;

    VICSoftIntClear_bit.INT1 = 1;//зняти периривання
    //опитування
    if(uart0_sts_bit.end_rx | uart0_sts_bit.timeout | uart0_sts_bit.not_respond){
        if((CRCrx0 == 0)&&(!(buf[0].exception.function & 0x80))){
            switch(device){
              default:
                //AIU
                switch((aum.req >> device*2) & 0x03){
                  case 0:
                    //просто копіювати всі регістри
                    i = 0;
                    i_ptr = (int*)&aiu[device].sts;

                    *i_ptr++ = (unsigned short)litle_endian(buf[0].read_resp.reg[i]);
                    for(i=1;i<5;i=i+2)
                        *i_ptr++ = (unsigned int)litle_endian(buf[0].read_resp.reg[i])<<16 | (unsigned short)litle_endian(buf[0].read_resp.reg[i+1]);//швидкий код і повільний код

                    if(((aum.task >> device*2) & 0x01)==0)//щоб не затерти зміни
                        for(i=5;i<12;i++)
                            *i_ptr++ = (unsigned short)litle_endian(buf[0].read_resp.reg[i]);//решта нормальних регістрів

                    i_ptr = (int*)&aiu[device].offset;//поправити адресу бо попередній пункт може не відбутися
                    if(((aum.task >> device*2) & 0x02)==0)//щоб не затерти зміни
                        for(i=12;i<16;i=i+2)
                            *i_ptr++ = (unsigned int)litle_endian(buf[0].read_resp.reg[i])<<16 | (unsigned short)litle_endian(buf[0].read_resp.reg[i+1]);//зміщення і підсилення

                    i_ptr = (int*)&aiu[device].Kg;//поправити адресу бо попередній пункт може не відбутися
                    for(i=16;i<18;i++)
                        *i_ptr++ = (unsigned short)litle_endian(buf[0].read_resp.reg[i]);//решта нормальних регістрів

                    aiu[device].temperature = (signed short)aiu[device].temperature;

                    break;
                  default:
                    // писати в RAM, FLASH або в обидва одночасно
                    aum.task ^= aum.req & (0x3 <<device*2);//зняти із задач виконану
                    aum.req ^= aum.req & (0x3 <<device*2);//із запитів  виконану
                    break;

                }
                break;
              case 13:
                //AOU
                switch((aum.req >> device*2) & 0x03){
                  case 0:
                    aou.sts= (unsigned short)litle_endian(buf[0].read_resp.reg[0]);//решта нормальних регістрів
                        if((aum.task >> (device*2 & 0x02))==0)//щоб не затерти зміни
                            aou.current= (unsigned short)litle_endian(buf[0].read_resp.reg[1]);//решта нормальних регістрів
                    aou.out= (unsigned short)litle_endian(buf[0].read_resp.reg[2]);//решта нормальних регістрів

                    break;
                  default:
                    // писати в RAM, FLASH або в обидва одночасно
                    aum.task ^= aum.req & (0x3 <<device*2);//зняти із задач виконану
                    aum.req = aum.req & ~(0x3 <<device*2);//із запитів нагло зняти всі
                    break;
                }
            }
            aum.err &= ~(1<<device);//зняти ознаку помилки прийому
            aum.err_cntr = 0;
            device++;
        }else{
            //помилка прийому або девайс повідомив про помилку
            if(++aum.err_cntr > 2){
                aum.err_cntr = 0;
                aum.err |= 1<<device;
                aum.task = aum.task & ~(0x3 <<device*2);//зняти із задач
                aum.req = aum.req & ~(0x3 <<device*2);//із запитів нагло зняти всі
                device++;
            }
        }

        if(device >=14)device = 0;

        //------------------------ відправлення запитів ------------------------------
        switch(device){
          default:
            //AIU
            switch((aum.task >> device*2) & 0x03){
              case 0:
                //просто читати всі регістри
                buf[0].read_req.address = device+1;
                buf[0].read_req.function = 0x03;//регістрів
                buf[0].read_req.start_address = big_endian(0x0000);
                buf[0].read_req.quantity = big_endian(18);
                wr_uart0((char*)&buf[0].modbuf[0],6,&buf[0].modbuf[0],256);
                break;

              case 1://записати оперативні параметри
                aum.req |= aum.task & (3<<device*2);//поставити ознаку відправки запиту
                buf[0].write_mult_req.address = device+1;
                buf[0].write_mult_req.function = 0x10;//запис багатьох регістрів
                buf[0].write_mult_req.start_address = big_endian(7);
                buf[0].write_mult_req.quantity = big_endian(4);
                buf[0].write_mult_req.byte_count = 4*2;

                i_ptr = (int*)&aiu[device].d.meo.setup;
                for(i=0;i<4;i++){
                    buf[0].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }
                wr_uart0((char*)&buf[0].modbuf[0],15,&buf[0].modbuf[0],256);
                break;

              case 2://записати постійні параметри
                aum.req |= aum.task & (3<<device*2);//поставити ознаку відправки запиту
                buf[0].write_mult_req.address = device+1;
                buf[0].write_mult_req.function = 0x10;//запис багатьох регістрів
                buf[0].write_mult_req.start_address = big_endian(11);
                buf[0].write_mult_req.quantity = big_endian(7);
                buf[0].write_mult_req.byte_count = 7*2;

                i_ptr = (int*)&aiu[device].mult;
                for(i=0;i<1;i++){
                    buf[0].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }

                for(i=1;i<5;i=i+2){
                    buf[0].write_mult_req.reg[i] = big_endian((*i_ptr>>16));
                    buf[0].write_mult_req.reg[i+1] = big_endian((unsigned short)(*i_ptr));
                    *i_ptr++;
                }
                for(i=5;i<7;i++){
                    buf[0].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }

                wr_uart0((char*)&buf[0].modbuf[0],21,&buf[0].modbuf[0],256);
                break;

              case 3://записати всі одночасно
                aum.req |= aum.task & (3<<device*2);//поставити ознаку відправки запиту
                buf[0].write_mult_req.address = device+1;
                buf[0].write_mult_req.function = 0x10;//запис багатьох регістрів
                buf[0].write_mult_req.start_address = big_endian(7);
                buf[0].write_mult_req.quantity = big_endian(12);
                buf[0].write_mult_req.byte_count = 12*2;

                i_ptr = (int*)&aiu[device].d.meo.setup;
                for(i=0;i<4;i++){
                    buf[0].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }
                for(i=4;i<5;i++){
                    buf[0].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }
                for(i=5;i<9;i=i+2){
                    buf[0].write_mult_req.reg[i] = big_endian(*i_ptr>>16);
                    buf[0].write_mult_req.reg[i+1] = big_endian((unsigned short)(*i_ptr)>>16);
                    *i_ptr++;
                }
                for(i=9;i<11;i++){
                    buf[0].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }

                wr_uart0((char*)&buf[0].modbuf[0],29,&buf[0].modbuf[0],256);
                break;
            }
            break;
          case 13:
            //AOU
            switch((aum.task >> (device*2)) & 0x03){
              case 0: //запит на читання всіх регістрів
                wr_uart0((char*)&aou_tx,6,&buf[0].modbuf[0],256);
                break;
              default:
                //запит запису оперативних параметрів
                aum.req |= aum.task & (3<<device*2);//поставити ознаку відправки запиту
                buf[0].write_mult_req.address = device+1;
                buf[0].write_mult_req.function = 0x10;//запис багатьох регістрів
                buf[0].write_mult_req.start_address = big_endian(1);
                buf[0].write_mult_req.quantity = big_endian(2);
                buf[0].write_mult_req.byte_count = 2*2;

                i_ptr = (int*)&aou.current;
                for(i=0;i<2;i++){
                    buf[0].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }
                wr_uart0((char*)&buf[0].modbuf[0],11,&buf[0].modbuf[0],256);

                break;
            }
            break;
        }

    }//кінець
}

/*------------------------------------------------------------------------------------------------*/
void modbus_uart0_init(void)
{
    uart0_rd_manag[0].ptr = &buf[0].modbuf[0];  //не зберігати байти
    uart0_rd_manag[0].count = 256;              //1байт-команда, 2байти-адреса
    uart0_rd_manag[0].next = 0;                 //продовження

    uart0_rd_ptr = &uart0_rd_manag[0];          //ініціалізація поінтера читання
    CRCtx0_init();
    CRCrx0_init();
}

/*------------------------------------------------------------------------------------------------*/
void modbus_uart1_init(void)
{
    uart1_rd_manag[0].ptr = &buf[1].modbuf[0];     //не зберігати байти
    uart1_rd_manag[0].count = 256;       //1байт-команда, 2байти-адреса
    uart1_rd_manag[0].next = 0;                 //продовження

    uart1_rd_ptr = &uart1_rd_manag[0];          //ініціалізація поінтера читання
    CRCtx1_init();
    CRCrx1_init();
    uart1_status = 0x07;//зняти статус
}
/*------------------------------------------------------------------------------------------------*/
void modbus_receive_init()
{
    uart1_rd_manag[0].ptr = &buf[1].modbuf[0];     //зберігати байти
    uart1_rd_manag[0].count = 256;              //кількість
    uart1_rd_manag[0].next = 0;                 //продовження немає
    uart1_rd_ptr = &uart1_rd_manag[0];          //ініціалізація поінтера читання
    CRCrx1_init();
}
/*------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/












/*

    {SUB_NUM,0,"Витрата газу                %06d мЗ",  (int*)&Qga.Qg,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Витрата повтря              %06d мЗ",  (int*)&Qga.Qa,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Температура газів що відх.   %05d \260",  (int*)&aiu_add[2].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Температура в зоні горіння   %05d \260",  (int*)&aiu_add[3].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Температура суміші газ-пов.  %05d \260",  (int*)&aiu_add[4].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Температура фідера           %05d \260",  (int*)&aiu_add[5].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Температура води             %05d \260",  (int*)&aiu_add[6].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Температура замаслювача      %05d \260",  (int*)&aiu_add[7].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Температура фільєрної пласт. %05d \260",  (int*)&aiu_add[8].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
газ    {SUB_NUM,0,"Положення засувки вихід       %04d %%",  (int*)&aiu[9].d.meo.out,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Положення засувки вихід       %04d %%",  (int*)&aiu[10].d.meo.out,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Струм фільєрної пластини    %06d А ",  (int*)&aiu_add[11].r.g,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"Рівень базальту в печі      %06d мм",  (int*)&aiu_add[12].r.g,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},

газ    {SUB_NUM,0,"Положення засувки завдання    %04d %%",  (int*)&aiu[9].d.meo.setup,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Положення засувки завдання    %04d %%",  (int*)&aiu[10].d.meo.setup,0,0x7fffffff,1,0,0,0,preEnterNum,postEnter_aui11_ws_ram,UP,DOWN,LEFT,RIGHT},
    {SUB_HEX,1,"Завдання управління струмом   %04X",  (int*)&aou_curr,0,0xffff,0,0,0,0,preEnterNum,postEnter_aou_ws_ram,UP_hex,DOWN_hex,LEFT,RIGHT},

    {SUB_NUM,1,"Інтенсивність вібрації         %03d %%",  (int*)&aiu[12].d.vibr.duty,0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Період роботи вібратора      %05d С",  (int*)&aiu[12].d.vibr.T,0,0xffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"Час активності вібратора      %04d %%",  (int*)&aiu[12].d.vibr.t,0,1000,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},

    і 4 регулятори
*/
//адреси параметрів для обміну з компютером
const MMA mma[]={
    0,0,&Qga.Qg,                //0
    0,0,&Qga.Qa,
    0,0,&aiu_add[2].r.t,
    0,0,&aiu_add[3].r.t,
    0,0,&aiu_add[4].r.t,
    0,0,&aiu_add[5].r.t,
    0,0,&aiu_add[6].r.t,
    0,0,&aiu_add[7].r.t,
    0,0,&aiu_add[8].r.t,
    0,0,&aiu[9].d.meo.out,
    0,0,&aiu[10].d.meo.out,     //10
    0,0,&aiu_add[11].r.g,
    0,0,&aiu_add[12].r.g,
    0,1000,&aiu[9].d.meo.setup, //параметри які можна міняти---------------------------------------
    0,1000,&aiu[10].d.meo.setup,
    0,65535,(int*)&aou.current,
    0,100,&N.vibr.duty,
    0,9999,&N.vibr.T,
    0,1000,&N.vibr.t,
    0,65535,&N.pid[0].Sp,//регулятор струму------------------------------------------------------
    0,9999,&N.pid[0].Kp,// коеф. пропорційності     20
    0,9999,&N.pid[0].Ti,
    0,9999,&N.pid[0].Td,
    0,9999,&N.pid[0].Ts,//час квантування
    0,65535,&N.pid[0].Range,//діапазон зміни регульованого параметру
    0,65535,&N.pid[0].Out_Range,//повний діапазон зміни вихідного сигналу
    0,65535,&N.pid[0].Speed,//швидкість зміни завдання
    0,10000,&N.pid[0].Out_min,//мінімальне значення вихідного сигналу
    0,10000,&N.pid[0].Out_max,//максимальне значення вихідного сигналу
    0,10000,&N.pid[0].Alarm_h,// аварія вехня межа
    0,10000,&N.pid[0].Alarm_l,// аварія нижня межа   30
    0,1,&N.pid[0].on_off,//регулятор включений \ виключений
    0,65535,&N.pid[0].res,//ознака того що відбувається виведення на режим фільєри
    0,65535,&N.pid[0].reserv[0],//резерв щоб наступні парараметри не зміщувались
    0,65535,&N.pid[0].reserv[1],//резерв щоб наступні парараметри не зміщувались
    0,65535,&N.pid[1].Sp, //регулятор витрати газу--------------------------------------------------------------
    0,9999,&N.pid[1].Kp,// коеф. пропорційності
    0,9999,&N.pid[1].Ti,
    0,9999,&N.pid[1].Td,
    0,9999,&N.pid[1].Ts,//час квантування
    0,65535,&N.pid[1].Range,//діапазон зміни регульованого параметру    40
    0,65535,&N.pid[1].Out_Range,//повний діапазон зміни вихідного сигналу
    0,65535,&N.pid[1].Speed,//швидкість зміни завдання
    0,10000,&N.pid[1].Out_min,//мінімальне значення вихідного сигналу
    0,10000,&N.pid[1].Out_max,//максимальне значення вихідного сигналу
    0,10000,&N.pid[1].Alarm_h,// аварія вехня межа
    0,10000,&N.pid[1].Alarm_l,// аварія нижня межа
    0,1,&N.pid[1].on_off,//регулятор включений \ виключений
    0,1,&N.pid[1].TC,//вхідна термопара для регулятора газу зона горіння або зона фідера  - вибір
    0,65535,&N.pid[1].reserv[0],//резерв щоб наступні парараметри не зміщувались
    0,65535,&N.pid[1].reserv[1],//резерв щоб наступні парараметри не зміщувались      50
    0,65535,&N.pid[2].Sp,//регулятор витрати повітря -----------------------------------------------------------------------
    0,9999,&N.pid[2].Kp,// коеф. пропорційності
    0,9999,&N.pid[2].Ti,
    0,9999,&N.pid[2].Td,
    0,9999,&N.pid[2].Ts,//час квантування
    0,65535,&N.pid[2].Range,//діапазон зміни регульованого параметру
    0,65535,&N.pid[2].Out_Range,//повний діапазон зміни вихідного сигналу
    0,65535,&N.pid[2].Speed,//швидкість зміни завдання
    0,10000,&N.pid[2].Out_min,//мінімальне значення вихідного сигналу
    0,10000,&N.pid[2].Out_max,//максимальне значення вихідного сигналу                  60
    0,10000,&N.pid[2].Alarm_h,// аварія вехня межа
    0,10000,&N.pid[2].Alarm_l,// аварія нижня межа
    0,1,&N.pid[2].on_off,//регулятор включений \ виключений
    0,1,&N.pid[2].cascad,//для регулятора повітря режим проботи за витратою газу
    0,65535,&N.pid[2].reserv[0],//резерв щоб наступні парараметри не зміщувались
    0,65535,&N.pid[2].reserv[1],//резерв щоб наступні парараметри не зміщувались
    0,65535,&N.pid[3].Sp, //регулятор рівня-----------------------------------------------------------------------------------------
    0,9999,&N.pid[3].Kp,// коеф. пропорційності
    0,9999,&N.pid[3].Ti,
    0,9999,&N.pid[3].Td,                                                                 // 70
    0,9999,&N.pid[3].Ts,//час квантування
    0,65535,&N.pid[3].Range,//діапазон зміни регульованого параметру
    0,65535,&N.pid[3].Out_Range,//повний діапазон зміни вихідного сигналу
    0,65535,&N.pid[3].Speed,//швидкість зміни завдання
    0,10000,&N.pid[3].Out_min,//мінімальне значення вихідного сигналу
    0,10000,&N.pid[3].Out_max,//максимальне значення вихідного сигналу
    0,10000,&N.pid[3].Alarm_h,// аварія вехня межа
    0,10000,&N.pid[3].Alarm_l,// аварія нижня межа
    0,1,&N.pid[3].on_off,//регулятор включений \ виключений
    0,65535,&N.pid[3].res,//ознака того що відбувається виведення на режим фільєри         //80
    0,65535,&N.pid[3].reserv[0],//резерв щоб наступні парараметри не зміщувались
    0,65535,&N.pid[3].reserv[1],//резерв щоб наступні парараметри не зміщувались
    0,65535,&N.G_A.reserv_pg,//щільність газу  --------------------------------------------------------------------------------
    0,65535,&N.G_A.reserv_pa,//щільність повітря
    0,65535,&N.G_A.kg,//коеф. перерахунку витрати газу
    0,65535,&N.G_A.ka,//коеф. перерахунку витрати повітря
    0,65535,&N.G_A.K,//коефіцієнт співідношення газу і повітря
    0,65535,&N.G_A.auto_g,//автоматичний реж. газ
    0,65535,&N.G_A.auto_a,//автоматичний реж. повітря
    0,1,&N.R.auto_on,//включення виключення автоматичної виводки //для виведення фільєри на режим-------------------------------- 90
    0,65535,&N.R.t1,//тривалість етапу //виводка з холодного
    0,65535,&N.R.t2,
    0,65535,&N.R.t3,
    0,65535,&N.R.c1,// струми
    0,65535,&N.R.c2,
    0,65535,&N.R.t5_1,//виводка до 5 хв
    0,65535,&N.R.t5_2,
    0,65535,&N.R.c5,//виводка до 1 хв.
    0,65535,&N.R.t_1,//зворотня виводка
    0,65535,&N.R.t_b1,                                                                                                             // 100
    0,65535,&N.R.t_b2,
    0,65535,&N.R.t_b3,
    0,65535,&N.R.c_b1,
    0,65535,&N.R.c_b2,
};
MS ms;//для збереження у фоновій програмі даних отриманих по інтерфейсі
/*
typedef struct{
    int Sp;//робоча точка
    int Kp;// коеф. пропорційності
    int Ti;
    int Td;
    int Ts;//час квантування
    int Range;//діапазон зміни регульованого параметру
    int Out_Range;//повний діапазон зміни вихідного сигналу
    int Speed;//швидкість зміни завдання
    int Out_min;//мінімальне значення вихідного сигналу
    int Out_max;//максимальне значення вихідного сигналу
    int Alarm_h;// аварія вехня межа
    int Alarm_l;// аварія нижня межа
    int on_off;//регулятор включений \ виключений
    union{
        int ramp;//ознака того що відбувається виведення на режим фільєри
        int cascad;//для регулятора повітря режим проботи за витратою газу
        int TC;//вхідна термопара для регулятора газу зона горіння або зона фідера  - вибір
    };
    int reserv[2];//резерв щоб наступні парараметри не зміщувались
}PID;
*/
/*
//декларація регістрів для обміну з компютером
union{
    unsigned short r[83];
    struct{
        unsigned short Qg;//витрата газу
        unsigned short Qa;//витрата повітря
        unsigned short Tgv;//Температура газів що відходять
        unsigned short Tzg;//Температура в зоні горіння
        unsigned short Ts;//Температура суміші газ-пов.
        unsigned short Tf;//Температура фідера
        unsigned short Tw;//Температура води
        unsigned short Tz;//Температура замаслювача
        unsigned short Tfil;//Температура фільєрної пласт.
        unsigned short meog;//Положення засувки вихід газ
        unsigned short meoa;//Положення засувки вихід повітря
        unsigned short CURR;//Струм фільєрної пластини
        unsigned short basalt;//Рівень базальту в печі

        unsigned short meoG; //положення засувки газу  завдання
        unsigned short meoA; //положення засувки повітря  завдання
        unsigned short curr; //Завдання управління струмом фільєри
        PID pid[4];//регулятори

    }R;
}out;
*/


//--------------------------------
void slave()
{
    unsigned char i;
    int address;
    int quantity;
    int off;

    // перевірка контрольної суми
    //перевірка адреси
	if(CRCrx1 == 0 && (buf[1].modbuf[0] == 0 || buf[1].modbuf[0] ==N.UART1.simplex)){
		switch(buf[1].modbuf[1]){			//function code
            case 3:                     //читання одного або більне регістрів (всі регістри розміщені у зовнішній RAM)
                if(litle_endian(buf[1].read_req.quantity) > 125){
                    buf[1].exception.exeption = 3; //забагато регістрів
                    error();
                    break;
                }
                // обмежитись областю 16-бітвих регістрів модбаса
                quantity = litle_endian(buf[1].read_req.start_address);
                quantity += litle_endian(buf[1].read_req.quantity);
                if(quantity > sizeof(mma)/sizeof(MMA)){
                    buf[1].exception.exeption = 2; //помилка адреси або діапазону
                    error();
                    break;
                }

                // підготовка відповіді

                //i_ptr = (int*)m_adr[litle_endian(buf[1].read_req.start_address)];
                //s_ptr = (short*)&buf[1].read_resp.reg[0];
                address = litle_endian(buf[1].read_req.start_address);//адреса
                quantity = litle_endian(buf[1].read_req.quantity);
                buf[1].read_resp.byte_count = litle_endian(buf[1].read_req.quantity)*2;

                //упаковуємо дані на відправку
                for(i=0;i<quantity;i++){
                    buf[1].read_resp.reg[i] = big_endian(*mma[address].adr);
                    if(mma[address].adr == &aiu_add[5].r.t)buf[1].read_resp.reg[i] = big_endian(aiu_add[5].r.t/10);
                    address++;

                }

                transmit_prepare(3+buf[1].read_resp.byte_count);
                start_transmit();
                break;

          case 6:
                // обмежитись областю 16-бітвих регістрів модбаса
                address = litle_endian(buf[1].write_req.register_address);
                if(address >= sizeof(mma)/sizeof(MMA)){
                    buf[1].exception.exeption = 2; //помилка адреси або діапазону
                    error();
                    break;
                }
                //ПЕРЕВІРКА діапазону значень
                if(mma[address].min ==0 && mma[address].max == 0){
                    buf[1].exception.exeption = 2; //помилка адреси або діапазону
                    error();
                    return;
                }
                if((litle_endian(buf[1].write_req.register_value)) < mma[address].min || (litle_endian(buf[1].write_req.register_value)) > mma[address].max){
                    buf[1].exception.exeption = 3; //помилка діапазону
                    error();
                    return;
                }
                //збереження початкової адреси і кількості для того щоб у фоні зберегти
                ms.addr = mma[address].adr;
                ms.count = 1;
                *mma[address].adr = litle_endian(buf[1].write_req.register_value);

                //якщо передано параметри вібратора то виставити ознаки запису у плату вібратора
                if((address > 15)&&(address < 19)){
                    aiu[12].d.vibr.duty = N.vibr.duty;
                    aiu[12].d.vibr.T = N.vibr.T;
                    aiu[12].d.vibr.t = N.vibr.t;
                    aum.task = (0x01 << 12*2);
                }

            transmit_prepare(4+2);
            start_transmit();
            break;

          case 16:                     //запис одного або більне регістрів
                if(litle_endian(buf[1].write_mult_req.quantity) > sizeof(mma)/sizeof(MMA)){
                    buf[1].exception.exeption = 3; //забагато регістрів
                    error();
                    break;
                }
                // обмежитись областю 16-бітвих регістрів модбаса
                quantity = litle_endian(buf[1].write_mult_req.start_address);
                quantity += litle_endian(buf[1].write_mult_req.quantity);
                if(quantity > sizeof(mma)/sizeof(MMA)){
                    buf[1].exception.exeption = 2; //помилка адреси або діапазону
                    error();
                    break;
                }

				//ПЕРЕВІРКА діапазону значень
                off = 0;//для руху по буферу прийому
                address = litle_endian(buf[1].write_mult_req.start_address);//адреса
                quantity = litle_endian(buf[1].write_mult_req.quantity);

                for(i=address;i<address + quantity;i++){
                    if(mma[i].min ==0 && mma[i].max == 0){
                        buf[1].exception.exeption = 2; //помилка адреси або діапазону
                        error();
                        return;
                    }
                    if((litle_endian(buf[1].write_mult_req.reg[off])) < mma[i].min || (litle_endian(buf[1].write_mult_req.reg[off])) > mma[i].max){
                        buf[1].exception.exeption = 3; //помилка діапазону
                        error();
                        return;
                    }
                    off++;
                }
                //збереження початкової адреси і кількості для того щоб у фоні зберегти
                ms.addr = mma[address].adr;
                ms.count = quantity;
                //копіювання
                off = 0;
                for(i=address;i<address + quantity;i++){
                    *mma[i].adr = litle_endian(buf[1].write_mult_req.reg[off]);
                    off++;
                }

                //якщо передано параметри вібратора то виставити ознаки запису у плату вібратора
                if((address > 15)&&(address < 19)){
                    aiu[12].d.vibr.duty = N.vibr.duty;
                    aiu[12].d.vibr.T = N.vibr.T;
                    aiu[12].d.vibr.t = N.vibr.t;

                    aum.task = (0x01 << 12*2);
                }

                transmit_prepare(4+2);
                start_transmit();
                break;

          default:
                /* функція не підтримується */
                buf[1].exception.exeption = 1;
                error();
                break;
        }

        //зарядити таймер на 3,5символа
	}
	else{
    //ініціалізація готовності прийому після помилки
	    U1FCR_bit.TFR = 1;//TX FIFO RESET
        U1FCR_bit.RFR = 1;//RX FIFO RESET
        //uart1_status = 0x07;//зняти статус
        modbus_uart1_init();
        /*
        uart1_rd_manag[0].ptr = &buf[1].modbuf[0];     //зберігати байти
        uart1_rd_manag[0].count = 256;              //кількість
        uart1_rd_manag[0].next = 0;                 //продовження немає
        uart1_rd_ptr = &uart1_rd_manag[0];          //ініціалізація поінтера читання
        CRCtx1_init();
        CRCrx1_init();
        */
    }

}


void error()
{
	buf[1].exception.function |= 0x80;

    transmit_prepare(3);
    start_transmit();
}

void transmit_prepare(char count)
{
    uart1_wr_crc.ptr = &crc_buf1[0];            //ця частина для відправлення контрольної суми
    uart1_wr_crc.count = 2;                     //підставляється  в перериванні після досягнення ДНА
    uart1_wr_crc.next = 0;

    uart1_wr_manag[0].ptr = &buf[1].modbuf[0];         //джерело
    uart1_wr_manag[0].count = count;            //кількість байтів
    uart1_wr_manag[0].next = 0;                 //більше нема
    uart1_wr_ptr = &uart1_wr_manag[0];          //ініціалізація поінтера запису

    uart1_rd_manag[0].ptr = 0;     //зберігати байти
    uart1_rd_manag[0].count = 0;              //кількість
    uart1_rd_manag[0].next = 0;                 //продовження немає
    uart1_rd_ptr = &uart1_rd_manag[0];          //ініціалізація поінтера читання
    CRCtx1_init();
    CRCrx1_init();
}

void start_transmit()
{
    uart1_status = 0x00;        //стан початок прийому і передачі start_tx = 1 start_rx = 1;
    uart1_sts_bit.pre_time = 1; //ознака початку паузи перерд передачою
    //uart1_sts_bit.start_tx = 1; //ознака початку передачі
    //U1MCR_bit.DTR = 1;          //стати в передачу
    /*
    T0MR1 = T0TC + N.UART1.wait_respond*1000;        //пауза між прийомом і передачою
    T0IR_bit.MR1INT = 1;        //int flag reset
    T0MCR_bit.MR1INT = 1;       //enable interrupt for match channal 1;
    */
    timer_uart[1] = N.UART1.wait_respond*3;//пауза між прийомом і передачою

}







