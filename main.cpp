#include <stdio.h>
#include "iolpc2138.h"
#include "type.h"
#include "menu.hpp"
#include "uart_0.h"
#include "uart_1.h"
#include "ramtron.h"
#include "lcd.h"
#include "fonts.h"
#include "lcd_middle.h"
#include "processing.hpp"
#include "modbus.h"

#include "spi.h"
#include "touch.h"
#include "Calc.h"
#include "timers.h"
#include "crypt.h"


extern"C"__irq __arm void irq_handler( void );

extern void main_init( void );
extern void kays_man(void);
extern char kays_pop(void);
//extern "C" void wr_uart1(char *src, unsigned int src_count, char *dest, unsigned int dest_count);
extern void send_command(char number,char instruction,unsigned int data);
extern void MA_Trig_WDT( void );




char ERROR = 0;//КОД ПОМИЛКИ ЯКЩО НЕ НУЛЬВИЙ
volatile int ERROR_TIME = 0;//час до якого дійсна помилка
char ERROR_I;//ІНІЦІАТОР ПОМИЛКИ ЧАСТОТНИКИ І ВСІ ІНШІ

/*===послідовний перелік елементів управління архівом для запису у рамтрон у такомуж порядку*/
//ramtron

unsigned int time_to_arch;//час до наступного архівування в секунах

int arch_flash_addr;//для запису адреси наступного запису у флеш
int arch_page;//сторінка 0-16383 архіву у флеш розміром 256 байт - 16 записів
int next_arch_page;//сторінка 0-16383 архіву у флеш розміром 256 байт - 16 записів   для відобаження наступної

unsigned short alarm_record_addr = (a_mng_rmtrn_addr+sizeof(A_MNG));
__no_init int wd_count;
/*-----------------------------------------------------------------------------------------*/

/*------------------------------------------------alarm------------------------------------*/
int GLOBAL_ALARM;//aварії від регуляторів 0..7 і частотника 8
int LAST_GLOBAL_ALARM;//ПОПЕРЕДНІ aварії від регуляторів 0..7 і частотника 8

//віртуальні таймери
union{
    TIME_ARCH virt_time[10];
    int virt_time_[10];
};
//static int virt_timer_reload=20;



char error_message();
TIME_ARCH new_time(int sec);

void archive(void);

/*------------------------------------------------------------------------------------*/

/*
**===========================================================================
**      ГОЛОВНА ФУНКЦІЯ
**===========================================================================
*/


int main()
{
    int i;

    main_init();
    menu_init();
    kays_init();

    i = 0x1ffff; while(--i);//wait for ramtron ready
    spi1_rd_status();
    while(spi1_sts_bit.end_rx == 0);

    spi1_init();
    while(spi1_sts_bit.end_rx == 0);

    //читаємо адресу запису архіву у флеш з рамтрона
    rd_rmtrn(a_mng_rmtrn_addr, &arch_flash_addr, sizeof(arch_flash_addr));
    while(spi1_sts_bit.end_rx == 0);

    rd_rmtrn(0,&N,sizeof(N));
    while(spi1_sts_bit.end_rx == 0);
/*
    N.UART0.bitrate = 115200;
    N.UART0.word_length = 8;
    N.UART0.stop_bit = 1;
    N.UART0.wait_respond = 16;
    N.UART0.timeout = 8;

    N.UART1.bitrate = 115200;
    N.UART1.word_length = 8;
    N.UART1.stop_bit = 1;
    N.UART1.wait_respond = 100;
    N.UART1.timeout = 100;
    N.UART1.simplex = 1;

    wr_rmtrn(0, &N, sizeof(N));
    while(spi1_sts_bit.end_rx == 0);
*/
    //перевірка параметрів внутрішнього інтерфейсу
    if(N.UART0.bitrate == 0)N.UART0.bitrate = 9600;
    if(N.UART0.word_length == 0)N.UART0.word_length = 8;
    if(N.UART0.stop_bit == 0)N.UART0.stop_bit = 1;
    if(N.UART0.wait_respond == 0)N.UART0.wait_respond = 1;
    if(N.UART0.timeout == 0)N.UART0.timeout = 1;

    uart0_setup(N.UART0.bitrate,N.UART0.word_length,N.UART0.stop_bit,N.UART0.parity_enable,N.UART0.parity_select);
    uart1_setup(N.UART1.bitrate,N.UART1.word_length,N.UART1.stop_bit,N.UART1.parity_enable,N.UART1.parity_select);

    modbus_uart1_init();

    uart0_sts_bit.timeout = 1;

    VICSoftInt_bit.INT1 = 1; //активізувати модбас

    //cypt_init();//ініціалізація активації

    time_to_arch = N.snap_time;

    char *tmp_ptr;

    //spi_prtcl();//запустити опитування тачпанелі

    initPID();

    Flr.time_down = time_delta();// після ввімкнення вважати що фільєра холодна
    Flr.ramp = 0; //зняти ознаку виводки фільєри
    Flr.down_ramp = 0;//зняти ознаку зворотньої виводки фільєри

    //ініціалізація початкової адреси і кількості для того щоб у фоні зберегти
    ms.addr = 0;
    ms.count = 0;

    //чекаємо доки не пройде два повних цикли обміну з вимірювачами і притримуємо таймер періода квантування регулятора струму
    while(device != 13)pid_0[0] = 0;
    while(device != 0)pid_0[0] = 0;

    //r.last_power = Flr.power = aiu[12].sts & 1;//наявність мережі
    //r.last_on = Flr.on = aiu[11].sts & (1<<15);//фільєра включена

    Flr.power = aiu[12].sts & 1;//наявність мережі
    Flr.on = aiu[11].sts & (1<<15);//фільєра включена

    if(Flr.on && Flr.power){
        //якщо фільєра включена
        if(aou.current > 0){
            //якщо завдання управління струмом не нульове
            //обраховуємо стум фільєри
            aiu_add[11].r.volt = Calc_mV(aiu[11].ADC,aiu[11].Kg);
            aiu_add[11].r.curr = Calc_mA(aiu[11].ADC,aiu[11].Kg);
            aiu_add[11].r.g = Line(aiu_add[11].r.curr, N.aiu_r[11].g_4, N.aiu_r[11].g_20);
            //aiu_add[11].r.g = 3800;
            if((aiu_add[11].r.g >= 0.95 * pid_r[0].Sp) && (aiu_add[11].r.g <= 1.05 * pid_r[0].Sp)){
                //якщо виміряне значення струму не відхиляється від заданого більше +-5%
                pid_r[0].Sp = N.pid[0].Sp;
            }else{
                if(aiu_add[11].r.g > 100){
                    //якщо стум не відповідає заданому
                    //якщо більше 1А
                    pid_r[0].Sp = aiu_add[11].r.g; //прийняти виміряне як задане
                    pid_r[0].Sp = N.pid[0].Sp;
                }
            }
            //раз сигнал управління струмом не нульовий отже напруга з вимірювачів не зникала тоу підхопити
            Flr.last_power = Flr.power;//наявність мережі
            Flr.last_on = Flr.on;//фільєра включена
        }
    }
    //ініціалізація параметрів вібрації
    aiu[12].d.vibr.duty = N.vibr.duty;
    aiu[12].d.vibr.T = N.vibr.T;
    aiu[12].d.vibr.t = N.vibr.t;
    aum.task = (0x01 << 12*2);

    while(1){
        /*
        __disable_interrupt();
        WDFEED = 0xAA;
        WDFEED = 0x55;
        __enable_interrupt();
        */
        if(general_purpose_flags.uart1_change){//відслідковування зміни параметрів UART1
            general_purpose_flags.uart1_change = 0;
            uart1_setup(N.UART1.bitrate,N.UART1.word_length,N.UART1.stop_bit,N.UART1.parity_enable,N.UART1.parity_select);
        }
        if(general_purpose_flags.uart0_change){//відслідковування зміни параметрів UART0
            general_purpose_flags.uart0_change = 0;
            uart0_setup(N.UART0.bitrate,N.UART0.word_length,N.UART0.stop_bit,N.UART0.parity_enable,N.UART0.parity_select);
        }

        tmp_ptr = &lcd_buf[0];
        i = 9600;
        do{
            *tmp_ptr = 0;
            tmp_ptr++;
        }while(--i);




        //if(spi0_sts_bit.end_rx)spi_prtcl();//якщо сенсорна панель включена і якщо кінець прийому то виймати всі дані і запустити знову
        //spi_prtcl();//обробка сенсора і клавіатури

        menu();
        //error_message();
        lcd_all_screen(0,&lcd_buf[0]);



        int new_t = 0;
        char count = 0;
        for(i=0;i<13;i++){
            aiu_add[i].r.volt = Calc_mV(aiu[i].ADC,aiu[i].Kg);

            //усереднення температур отриманих з AIU
            //перевіряємо наявність термодатчика
            if(aiu[i].temperature != 0x7fff){
                //перевіряємо коректність відповіді
                if((aum.err & (1<<i)) == 0){
                    new_t += aiu[i].temperature;
                    count++;
                }
            }
        }

        if(count){
            //якщо температури знайдено
            t_Cool = new_t/count + ((new_t%count >= 5)? 1 : 0);
        }else{
            t_Cool = 0;
        }

        //використати датчик тачпанелі
        new_t = 0;
        for(i=0;i<32;i++){
            new_t += temperature[i];
        }
        new_t = new_t >> 5;
        new_temperature = new_t * 2537 * 2500 / 4096 / 1000 - 273 + (((new_t * 2537 * 2500 / 4096 / 100 - 2730)&0xf >=8)? 1:0);//свіжа температура





        for(i=0;i<13;i++){
            aiu_add[i].r.volt = Calc_mV(aiu[i].ADC,aiu[i].Kg);
            switch(N.aiu_r[i].type){
              case 0://mA
                aiu_add[i].r.curr = Calc_mA(aiu[i].ADC,aiu[i].Kg);
                if((i == 11)&&(aiu_add[i].r.curr < 4000))aiu_add[i].r.g = Line(4000, N.aiu_r[i].g_4, N.aiu_r[i].g_20);
                else aiu_add[i].r.g = Line(aiu_add[i].r.curr, N.aiu_r[i].g_4, N.aiu_r[i].g_20);
                break;
              case 1: //ТХА (K)
                aiu_add[i].r.t = Termocuple_K(aiu_add[i].r.volt, N.aiu_r[i].comp? t_Cool : 0);
                break;
              case 2: //ТПР (B)
                aiu_add[i].r.t = Termocuple_B(aiu_add[i].r.volt, N.aiu_r[i].comp? t_Cool : 0);
                break;
              case 3: //TCM100
                aiu_add[i].r.Ohm = Calc_Ohm(aiu_add[i].r.volt);
                aiu_add[i].r.t = RTD_Cu100(aiu_add[i].r.Ohm);
                break;
              case 4: //Pt100
                aiu_add[i].r.Ohm = Calc_Ohm(aiu_add[i].r.volt);
                aiu_add[i].r.t = RTD_Pt100(aiu_add[i].r.Ohm);
                break;

            }
        }

        aou_curr = ((aou.current * 20000)>>16) + ( (((aou.current * 20000) & 0xffff)>0x7fff)? 1:0);//обчислення вихідного струму AOU

        Q_air_gas();//обрахунок

        //регулятор витрати повітря
        if(N.pid[2].cascad && N.pid[2].on_off){
            //якщо регулятор витрати повітря працює за витратою газу і він до того ж увімкнутий
            N.pid[2].Sp = N.G_A.K * Qga.Qg / 100;
        }

        //вибір каналу регулювання для регулятора витрати газу
        switch(N.pid[1].TC){
          case 0: Qga.T_RG = aiu_add[3].r.t; break;//температура в зоні горіння
          case 1: Qga.T_RG = aiu_add[5].r.t; break;//температура в зоні фідера
        }

        Flr.power = aiu[12].sts & 1;//наявність мережі
        Flr.on = aiu[11].sts & (1<<15);//фільєра включена


        current_ramp(); //організація виводки фільєри
        current_back_ramp(); //організація охолодження фільєри

        //якщо напруга є фільєра включена, авт.вихід на режим фільєри викл. і струм досягнув заданого
        if((Flr.on && Flr.power && !N.R.auto_on)&&(aiu_add[11].r.g >= N.pid[0].Sp)) Flr.ramp = 0;//ознака виведення фільєри зняти

        if(Flr.on && Flr.power && !Flr.ramp)setAlatmDateTime();//записати в будильник поточний час для того, щоб потім визначити скільки часу минуло

        if((aum.task & (0x03 << 12*2)) == 0){
            //якщо не відбувається запис у плату вібрації то переписати збережені параметри
            N.vibr.duty = aiu[12].d.vibr.duty;
            N.vibr.T = aiu[12].d.vibr.T;
            N.vibr.t = aiu[12].d.vibr.t;
        }


        //запис в рамтрон уставок отриманих через інтерфейс
        if(ms.addr && ms.count){
            spi1_init();
            while(spi1_sts_bit.end_rx == 0);

            int addr =(int)ms.addr - (int)&N;
            wr_rmtrn(addr, ms.addr, ms.count * sizeof(int));
            while(spi1_sts_bit.end_rx == 0);
            rd_rmtrn(addr, ms.addr, ms.count * sizeof(int));
            while(spi1_sts_bit.end_rx == 0);
            ms.addr = 0;
            ms.count = 0;
        }

        /*
        Qga.Qg = 512;
        Qga.Qa = 5234;
        aiu_add[2].r.t = 5678;//темератуга зазів що відходять
        aiu_add[3].r.t = 15678;//зона горіння
        aiu_add[4].r.t = 4567;//температура суміші
        aiu_add[5].r.t = 12345;//темература Фідера
        aiu_add[6].r.t = 987;//температура води
        aiu_add[7].r.t = 876;//замаслювач
        aiu_add[8].r.t = 13245;//температура фільєри
        aiu[9].d.meo.out = 512;
        aiu[10].d.meo.out = 678;
        aiu_add[11].r.g = 3456;//Струм
        aiu[12].d.vibr.T = 1234;//Період вібрації
        */
        archive();

        next_arch_page = arch_flash_addr / 256;
        
        N.seq.flag_ok = 1;
/*
        crypt_work();
        //вимикаємо регулятори якщо ліцензія кінчилась
        if(N.seq.flag_ok == 0){
            N.pid[0].on_off = 0;
            N.pid[1].on_off = 0;
            N.pid[2].on_off = 0;
            N.pid[3].on_off = 0;
        }
*/
    }
}

/*
**===========================================================================
**
**===========================================================================
*/

/*
**===========================================================================
** формування архіву
**===========================================================================
*/
void archive(void)
{
A_MNG a_mng;//управління процесом архівації
A_MNG a_mng2;//для перевірки правильності запису
    int i;

    if(time_to_arch == 0){//якщо час до моменту архівації вийшов то записати в архів
        //читання з рамтрону структури управління процесом архівації
        rd_rmtrn(a_mng_rmtrn_addr, &a_mng, sizeof(a_mng));
        while(spi1_sts_bit.end_rx == 0);

        if(a_mng.rec_count > 15) a_mng.rec_count = 0;//щоб без фокусів

        if(a_mng.rec_count == 0){//якщо перший запис
            for(i=1;i<64;i++) a_mng.rec_ibuf[i] = 0;//очистити
        }
        a_mng.rec[a_mng.rec_count].time = time_pack();//час від 2000 року в секундах
        //зафасовка даних
        a_mng.rec[a_mng.rec_count].Ggas = Qga.Qg/10;
        a_mng.rec[a_mng.rec_count].Gair = Qga.Qa/100;
        a_mng.rec[a_mng.rec_count].Tgasiv = aiu_add[2].r.t;//темератуга зазів що відходять
        a_mng.rec[a_mng.rec_count].Tsum = aiu_add[4].r.t;//температура суміші
        a_mng.rec[a_mng.rec_count].Tfider = aiu_add[5].r.t;//темература Фідера
        a_mng.rec[a_mng.rec_count].curr = aiu_add[11].r.g;//Струм
        a_mng.rec[a_mng.rec_count].Vbr = aiu[12].d.vibr.T;//Період вібрації
        a_mng.rec_count++;
        time_to_arch = N.snap_time;//новий час архівації

        char iter = 3;//спроби запису
        do{
            //запис у рамтрон оновлених даних
            spi1_init();
            while(spi1_sts_bit.end_rx == 0);
            wr_rmtrn(a_mng_rmtrn_addr, &a_mng, sizeof(a_mng));
            while(spi1_sts_bit.end_rx == 0);

            //читання з рамтрона в буфер для порівняння
            rd_rmtrn(a_mng_rmtrn_addr, &a_mng2, sizeof(a_mng2));
            while(spi1_sts_bit.end_rx == 0);

            char *ptr_o,*ptr_c;//оригінал і копія
            ptr_o = (char*)&a_mng;
            ptr_c = (char*)&a_mng2;
            //перевірка правильності запису
            for(i=0;i<sizeof(a_mng);i++){
                if(*ptr_o != *ptr_c){
                    //аварія неправильно записані/прочитані дані
                    break;
                }
                ptr_o++;
                ptr_c++;
            }
            if(i == sizeof(a_mng))break;
        }while(--iter);
        if(iter == 0)return;//не вдалося записати

        //при успішній перевірці
        if(a_mng.rec_count == 16){//укомлектовано 256 байтів - переписати у флеш

            //запис у флеш зформованих 16 записів
            spi1_WEL(a_mng.flash_addr);
            while(spi1_sts_bit.end_rx == 0);
            wr_flash(a_mng.flash_addr&0x3FFF00, &a_mng.rec, 256);
            while(spi1_sts_bit.end_rx == 0);

            a_mng.flash_addr = (a_mng.flash_addr + 256) & 0x3FFF00;

            //запис у рамтрон місця запису у флеш  flash_addr і rec_count
            spi1_init();
            while(spi1_sts_bit.end_rx == 0);
            wr_rmtrn(a_mng_rmtrn_addr, &a_mng, sizeof(int)*2);
            while(spi1_sts_bit.end_rx == 0);

            //читання з рамтрона місця запису у флеш flash_addr і rec_count
            rd_rmtrn(a_mng_rmtrn_addr, &a_mng, sizeof(int)*2);
            while(spi1_sts_bit.end_rx == 0);

        }
        arch_flash_addr = a_mng.flash_addr;
    }
}

/*
**===========================================================================
**обробка отриманих даних для графіків
**===========================================================================
*/


/*
**===========================================================================
**
**===========================================================================
*/





