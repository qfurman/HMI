#include <math.h>
#include "fonts.h"
#include "menu.hpp"
#include "main.hpp"
#include "spi.h"
#include "lcd.h"
#include "lcd_middle.h"
#include "timers.h"
#include "defines.h"
#include "touch.h"
#include "modbus.h"
#include "TCK.h"
#include "TCB.h"
#include "Cu100.h"
#include "Pt100.h"
#include "LPC_Rtc.h"

/*===============================================================================================*/
//для обчислення код ацп береться з буфера

//для обчислення код ацп береться з буфера
int Calc_mV( int ADC, char Kg)
{
    //мВ - 3 знаки після коми
    int mV = ((((long long)ADC * 2450000) >> Kg) >> 22);//сунемо два меньше щоб округлити
    if(mV & 0x3 >= 2) mV = mV + 4;
    mV = mV>>2;
    return mV;
}

/*===============================================================================================*/
//для обчислення код ацп береться з буфера
int Calc_mA( int ADC, char Kg)
{
    int mV,mA;

    //мВ і мА - 3 знаки після коми
    //мВ - 3 знаки після коми
    mV = ((((long long)ADC * 2450000) >> Kg) >> 22);//сунемо два меньше щоб округлити
    if(mV & 0x3 >= 2) mV = mV + 4;
    mV = mV>>2;

    mV = mV*100 / 252; //R = 25.2 Ом
    mA = mV/10 + ((mV%10 >= 5)? 1 : 0);

    return mA;
}
/*===============================================================================================*/
//обчислення значення  параметру, первинним для котрого є струм 4-20мА
int Line(int mA, int P_4, int P_20)
{
   return (mA - 4000) * (P_20 - P_4) / 16000 + P_4;
}

/*===============================================================================================*/
/*===============================================================================================*/
//для обчислення код ацп береться з буфера
int Calc_Ohm( int mV)
{
    int Ohm;

    Ohm = mV/2 + ((mV%2 >= 1)? 1 : 0);   //ділимо напругу на 2 мА

    return Ohm;
}
/*===============================================================================================*/
int Termocuple_K(int mV, int Tcool)
{
    int T0,T,uV;
    char index;

    //обмеження термокомпенсації
    if(Tcool > 1000) Tcool = 1000;
    if(Tcool < -500) Tcool = -500;

    if(Tcool >= 0){
        //при додатніх температурах
        index = Tcool/100;
        uV = (Tcool - index*100)*(uV_K[index+1] - uV_K[index])/100 + uV_K[index];
    }else{
        index = Tcool/-50;
        uV = (Tcool + index*50)*(uV_Kn[index+1] + uV_Kn[index])/(-50) + uV_Kn[index];
    }

    mV = mV + uV; // термокомпенсація  зврахуванням того що uV в мкВ а mV в тисячних мВ

    if(mV < 0) return 0;
    //if(mV > 54000) return 99999;//зашкал


    index = mV>>9;// /512 БО ТАБЛИЦЯ ЧЕРЕЗ 0.512 мВ
    if(index > sizeof(TCK)/sizeof(int)-2) index = sizeof(TCK)/sizeof(int)-2; //має вказувати на на передостанню точку максимум
    T0 = TCK[index];
    T = (mV - index*512)*(TCK[index+1] - T0)/512 + T0;
    T = T/10 + ((T%10>=5)? 1 : 0);
    return T;
}
/*===============================================================================================*/
/*===============================================================================================*/
int Termocuple_B(int mV, int Tcool)
{
    int T0,T,uV;
    int index;

    //обмеження термокомпенсації
    if(Tcool > 1000) Tcool = 1000;
    if(Tcool < 0) Tcool = 0;

    //при додатніх температурах
    index = Tcool/100;
    uV = (Tcool - index*100)*(uV_B[index+1] - uV_B[index])/100 + uV_B[index];

    mV = mV + uV; // термокомпенсація з врахуванням того що uV в мкВ а mV в тисячних мВ

    if(mV < 0) return 0;
    //if(mV > 54000) return 99999;//зашкал


    index = mV>>5;// /32 БО ТАБЛИЦЯ ЧЕРЕЗ 0.032 мВ
    if(index > sizeof(TCB)/sizeof(int)-2) index = sizeof(TCB)/sizeof(int)-2; //має вказувати на на передостанню точку максимум
    T0 = TCB[index];
    T = (mV - index*32)*(TCB[index+1] - T0)/32 + T0;
    T = T/10 + ((T%10>=5)? 1 : 0);

    return T;
}

/*===============================================================================================*/
/*===============================================================================================*/
int RTD_Cu100(int Ohm)
{
    int T0,T;
    int index;

    index = Ohm/2000;// /2 БО ТАБЛИЦЯ ЧЕРЕЗ 2 Ohm
    if(index > sizeof(Cu100)/sizeof(int)-2) index = sizeof(Cu100)/sizeof(int)-2; //має вказувати на на передостанню точку максимум
    T0 = Cu100[index];
    T = (Ohm - index*2000)*(Cu100[index+1] - T0)/2000 + T0;
    T = T/10 + ((T%10>=5)? 1 : 0);

    return T;
}

/*===============================================================================================*/
int RTD_Pt100(int Ohm)
{
    int T0,T;
    int index;

    index = Ohm/3000;// /3 БО ТАБЛИЦЯ ЧЕРЕЗ 3 Ohm
    if(index > sizeof(Pt100)/sizeof(int)-2) index = sizeof(Pt100)/sizeof(int)-2; //має вказувати на на передостанню точку максимум
    T0 = Pt100[index];
    T = (Ohm - index*3000)*(Pt100[index+1] - T0)/3000 + T0;
    T = T/10 + ((T%10>=5)? 1 : 0);

    return T;
}

/*===============================================================================================*/
/*
int Current_norm(int In)
{
    int I0,I;
    int index;

    index = In>>11;// /2048 БО ТАБЛИЦЯ на 32 значення
    if(index > sizeof(Curr_N)/sizeof(short)-2) index = sizeof(Curr_N)/sizeof(short)-2; //має вказувати на на передостанню точку максимум
    I0 = Curr_N[index];
    I = (((In - index<<11)*(Curr_N[index+1] - I0))>>11) + I0;
    I = I/10 + ((I%10>=5)? 1 : 0);

    return I;
}

int Current_back(int In)
{
    int I0,I;
    int index;

    index = In>>11;// /2048 БО ТАБЛИЦЯ на 32 значення
    if(index > sizeof(Curr_Back)/sizeof(short)-2) index = sizeof(Curr_Back)/sizeof(short)-2; //має вказувати на на передостанню точку максимум
    I0 = Curr_Back[index];
    I = (((In - index<<11)*(Curr_Back[index+1] - I0))>>11) + I0;
    I = I/10 + ((I%10>=5)? 1 : 0);

    return I;
}
*/
/*===============================================================================================*/
PID_run pid_r[4];

int UpdatePID(PID * pid, PID_run * pid_run, int error)
{
    int P, I, D, Out;
    int i;
    unsigned int k;

    P = pid->Kp * error/10;// томущо один знак після коми

    //заганяємо нову помилку
    pid_run->buf[pid_run->n] = error;

    //інтегрування
    k = pid_run->n;
    I = 0;
    for(i=0;i<pid->Ti/pid->Ts;i++){
       I += pid_run->buf[k];
       //рухаємось буфером в зворотньому напрямку
       k--;
       if(k > sizeof(pid_run->buf)/sizeof(short)-1) k = sizeof(pid_run->buf)/sizeof(short)-1;//щоб не булопереходуза межі буфера
    }

    I = I / pid->Ti * pid->Ts;

    //знаодимо місцезнаходження попередньої помилки
    i = pid_run->n - pid->Td/pid->Ts;
    if(i > sizeof(pid_run->buf)/sizeof(short)-1)i = sizeof(pid_run->buf)/sizeof(short)-1;
    if(i < 0)i = sizeof(pid_run->buf)/sizeof(short)-1 + i;

    //обчислення диф.складової
    D = pid->Td * (error - pid_run->buf[i]) / pid->Ts;

    //інкрементуємо місце наступного запису
    pid_run->n++;
    if(pid_run->n > sizeof(pid_run->buf)/sizeof(short)-1) pid_run->n = 0;

    Out = P + I + D;
    Out = ((long long)Out * pid->Out_Range) / pid->Range;//нормуємо вихід відносно фізичного вихідного сигналу і фізичного вхідного

    return Out;
}

/*===============================================================================================*/
/*===============================================================================================*/

void initPID()
{
    int i,k;

    for(k=0;k<sizeof(pid_r)/sizeof(PID_run)-1;k++){
        pid_r[k].n = 0;
        pid_r[k].last_out = 0;
        pid_r[k].Sp = 0;
        for(i=0;i<sizeof(pid_r[k].buf)/sizeof(short)-1;i++){
            pid_r[k].buf[i] = 0;
        }
    }

    for(i=0;i<4;i++){
        if(N.pid[i].on_off){//якщо регулятор увімкнуто
            pid_r[i].Sp = N.pid[i].Sp;//то переписати значення заданої точки прямо в поточне завдання регулятора без швидкості наростання
        }
    }

}

/*===============================================================================================*/
void Q_air_gas()
{
    /*
    if(aiu_add[0].r.g >= 0)Qga.Qg = N.G_A.kg * sqrt((double)(20 * aiu_add[0].r.g)/N.G_A.pg)/100;// коеф. - 2 знаки після коми, перепад два знаки а щільність в грамах на кілограм(тобто три знаки)
    else Qga.Qg = 0;
    if(aiu_add[1].r.g >= 0)Qga.Qa = N.G_A.ka * sqrt((double)(20 * aiu_add[1].r.g)/N.G_A.pa)/100;// коеф. - 2 знаки після коми щільність в грамах на кілограм
    else Qga.Qa = 0;
    */
    if(aiu_add[0].r.g >= 0)Qga.Qg = (int) (N.G_A.kg * sqrt((double)aiu_add[0].r.g/100)/100);// коеф. - 2 знаки після коми, перепад два знаки
    else Qga.Qg = 0;
    if(aiu_add[1].r.g >= 0)Qga.Qa = (int) (N.G_A.ka * sqrt((double)aiu_add[1].r.g/100)/100);// коеф. - 2 знаки після коми щільність
    else Qga.Qa = 0;

}
/*===============================================================================================*/
FLR Flr;

void current_ramp()
{

    if((!Flr.on && Flr.last_on) || (Flr.last_power && !Flr.power)){
        //якщо фільєра виключена або пропала електрика то обнулити завдання регулятору
        pid_r[0].Sp = 0;
        aou.current = 0;
        aum.task = (0x01 << 13*2);

    }

    if(!Flr.on || !Flr.power){
        //якщо фільєра відключена то вийти з виводки
        Flr.regime = 0;
        Flr.phase = 0;
        Flr.b_phase = 0; //це для охолодження фільєри
    }

    if(N.R.auto_on && Flr.on && Flr.power){ //дозволено автоматичну виводку  і включений регулятор
        switch(Flr.regime){
          case 0: //аналізуємо наявність напруги в мережі і чи включений нагрів фільєри і дозволений
            //якщо мережі не було і мережа зявилась, якщо фільєра була виключена і включилась при наявності мережі включена і
            if(((!Flr.last_power && Flr.power) || (!Flr.last_on && Flr.on)) && Flr.power && Flr.on){
                Flr.ramp = 1; // ознака того що струм фільєри виводиться
                N.pid[0].on_off = 1; //нагло включити регулятор
                if(Flr.time_down >= 300){
                    Flr.regime = 1;//холодна
                    Flr.time = N.R.t1;//час етапу;
                    Flr.phase = 0;
                    break;
                }else{
                    if(Flr.time_down >= 60){
                        Flr.regime = 2;// до 5 хв
                        Flr.time = N.R.t5_1;//час етапу;
                        Flr.phase = 0;
                        break;
                    }else{
                        if(Flr.time_down <= 60){
                            Flr.regime = 3;// до 1 хв
                            Flr.time = N.R.t_1;//час етапу;
                            Flr.phase = 0;
                            break;
                        }
                    }
                }
            }
            break;

          case 1:
            //холодна
            switch(Flr.phase){
              case 0:
                //перший відрізок - нарощування до приблизно, 15А
                //записати робочу точку прямо в поточне завдання
                pid_r[0].Sp = (N.R.t1 - Flr.time) * N.R.c1 / N.R.t1;
                if(Flr.time == 0){
                    Flr.time = N.R.t2;//час наступного етапу
                    Flr.phase++;//перейти на наступну фазу розігріву
                }
                break;
              case 1:
                //записати робочу точку прямо в поточне завдання
                pid_r[0].Sp = (N.R.t2 - Flr.time)*(N.R.c2 - N.R.c1)/ N.R.t2 + N.R.c1;
                if(Flr.time == 0){
                    Flr.time = N.R.t3;//час наступного етапу
                    Flr.phase++;//перейти на наступну фазу розігріву
                }
                break;
              case 2:
                //записати робочу точку прямо в поточне завдання
                pid_r[0].Sp = (N.R.t3 - Flr.time)*(N.pid[0].Sp - N.R.c2)/ N.R.t3 + N.R.c2;
                if(Flr.time == 0){
                    Flr.phase++;//перейти на наступну фазу розігріву
                    Flr.time_down = 0;// як фільєру розігріли то обнулити час відліку її вимкнення
                    Flr.ramp = 0; //зняти ознаку виводки фільєри
                }
                break;
              case 3: //  тут ми відсижджуємось аж до наступної виводки
                break;
            }
            break;

          case 2://до 5 хв.
            switch(Flr.phase){
              case 0:
                //записати робочу точку прямо в поточне завдання
                pid_r[0].Sp = (N.R.t5_1 - Flr.time) * N.R.c5 / N.R.t5_1;
                if(Flr.time == 0){
                    Flr.time = N.R.t5_2;//час наступного етапу
                    Flr.phase++;//перейти на наступну фазу розігріву
                }
                break;
              case 1:
                //записати робочу точку прямо в поточне завдання
                pid_r[0].Sp = (N.R.t5_2 - Flr.time)*(N.pid[0].Sp - N.R.c5)/ N.R.t5_2 + N.R.c5;
                if(Flr.time == 0){
                    Flr.phase++;//перейти на наступну фазу розігріву
                    Flr.time_down = 0;// як фільєру розігріли то обнулити час відліку її вимкнення
                    Flr.ramp = 0; //зняти ознаку виводки фільєри
                }
                break;
              case 2: //тут ми відсижджуємось аж до наступної виводки
                break;
            }
            break;

          case 3://до 1 хв.
            switch(Flr.phase){
              case 0:
                //записати робочу точку прямо в поточне завдання
                pid_r[0].Sp = (N.R.t_1 - Flr.time) * N.pid[0].Sp / N.R.t_1;
                if(Flr.time == 0){
                    Flr.phase++;//перейти на наступну фазу розігріву
                    Flr.time_down = 0;// як фільєру розігріли то обнулити час відліку її вимкнення
                    Flr.ramp = 0; //зняти ознаку виводки фільєри
                }
                break;
              case 1: //тут ми відсижджуємось аж до наступної виводки
                break;
            }
            break;
        }
    }else Flr.ramp = 0; //зняти ознаку виводки фільєри

    Flr.last_power = Flr.power;//оновити попередній стан мережі
    Flr.last_on = Flr.on;//оновити попередній стан  вимикача фільєри
}
/*===============================================================================================*/
//організація охолодження фільєри
void current_back_ramp()
{
    //якщо запущено процес охолодження і при цьому фільєра включена
    //і значення теперішнього завдання темератури вище ніж те до якого слід охолодити то запусити процес
    //а якщо ні то виключити охолодження
    if(Flr.down_ramp && Flr.power && Flr.on && (pid_r[0].Sp > N.R.c_b1)){
        switch(Flr.b_phase){
          case 0:
            N.pid[0].on_off = 1; //нагло включити регулятор
            Flr.time = N.R.t_b1;//час етапу;
            Flr.phase = 0;
            Flr.regime = 4;//охолодження
            Flr.b_phase = 1;
            Flr.ramp = 1; // ознака того що струм фільєри виводиться - заблокувати безударний режим
            Flr.time_down = 300;// як фільєру охолодили то вважати її холодною
            break;
          case 1:
            //записати робочу точку прямо в поточне завдання
            pid_r[0].Sp = N.pid[0].Sp - (N.R.t_b1 - Flr.time)*(N.pid[0].Sp - N.R.c_b1)/ N.R.t_b1 ;
            if(Flr.time == 0){
                Flr.b_phase++;//перейти на наступну фазу розігріву
                Flr.time = N.R.t_b2;// як фільєру розігріли то обнулити час відліку її вимкнення
            }
            break;
          case 2:
            //Тут звичайно маєбути витримка
            pid_r[0].Sp = N.R.c_b1 - (N.R.t_b2 - Flr.time)*(N.R.c_b1 - N.R.c_b2)/ N.R.t_b2 ;
            if(Flr.time == 0){
                Flr.b_phase++;//перейти на наступну фазу розігріву
                Flr.time = N.R.t_b3;// як фільєру розігріли то обнулити час відліку її вимкнення
            }
            break;
          case 3:
            //записати робочу точку прямо в поточне завдання
            pid_r[0].Sp = N.R.c_b2 - (N.R.t_b3 - Flr.time)*N.R.c_b2 / N.R.t_b3 ;
            if(Flr.time == 0){
                Flr.b_phase++;//перейти на наступну фазу розігріву
                Flr.ramp = 0; //зняти ознаку виводки фільєри
                Flr.down_ramp = 0;//ЗНЯТИ ОЗНАКУ ПРОЦЕСУ ОХОЛОДЖЕННЯ
            }
            break;
          case 4:
            //кінець процесу охолодження
            break;
        }
    }else{
        Flr.down_ramp = 0;
        Flr.b_phase = 0;
    }
}
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
//упаковка поточного часу для порівняння
unsigned int time_delta()
{
    unsigned int i,l,k,day;
    LPC_Rtc_Time_t Time;

    RTC_GetTime(&Time);
    day =0;
    for(i=2000;i<YEAR;i++){
        if(IsLeapYear(i)) day += 366;
        else day +=365;
    }
    l = (day + DOY)*86400 + Time.hour*3600 + Time.minute*60 + Time.second;

    day = 0;
    for(i=2000;i<ALYEAR;i++){
        if(IsLeapYear(i)) day += 366;
        else day +=365;
    }
    k = (day + ALDOY)*86400;
    k += ALHOUR*3600;
    k += ALMIN*60;
    k += ALSEC;
    return l-k;
}
/*===============================================================================================*/
//для запису в будилник поточного часу, щоб після включення розібратись скільки секунд минуло
void setAlatmDateTime()
{
    LPC_Rtc_DateTime_t D_t;

    RTC_GetDateTime(&D_t);
    RTC_SetAlarmDateTime(&D_t);

}
/*===============================================================================================*/
//упаковка поточного часу
unsigned int time_pack()
{
    unsigned int i,l,day;
    LPC_Rtc_Time_t Time;

    RTC_GetTime(&Time);
    day =0;
    for(i=2000;i<YEAR;i++){
        if(IsLeapYear(i)) day += 366;
        else day +=365;
    }
    l = (day + DOY)*86400 + Time.hour*3600 + Time.minute*60 + Time.second;
    return l;
}
/*===============================================================================================*/
const char dinL[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};
const char din[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

void time_UNpack(int time, LPC_Rtc_DateTime_t *Dt)
{
    unsigned int i;


    Dt->year = 2000;
    for(;;){
        if(IsLeapYear(Dt->year)){
            if(time > 31622400)time -= 31622400;//86400*366
            else break;
            Dt->year++;
        }
        else{
            if(time > 31536000)time -= 31536000;//86400*365
            else break;
            Dt->year++;
        }
    }

    Dt->DOY = time / 86400;
    time -= Dt->DOY * 86400;

    i = Dt->DOY;
    Dt->month = 1;
    for(;;){
        if(IsLeapYear(Dt->year)){
            if(i > dinL[Dt->month])i -= dinL[Dt->month];
            else break;
            Dt->month++;
        }else{
            if(i > din[Dt->month])i -= dinL[Dt->month];
            else break;
            Dt->month++;
        }
    }


    Dt->day = i;

    Dt->hour = time / 3600;
    time -= Dt->hour * 3600;
    Dt->minute = time / 60;
    time -= Dt->minute * 60;
    Dt->second = time;

}
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
