

#include <stdbool.h>
#include "iolpc2138.h"
//#include "fiolpc2138.h"
#include "type.h"
#include "uart_0.h"
#include "uart_1.h"
#include "macros.h"
#include "menu.hpp"
#include "touch.h"
#include "modbus.h"
#include "main.hpp"
#include "spi.h"

#include "Calc.h"
/*------------------------------------------------------------------------------------------------/
/  ОРГАНІЗАЦІЯ ІНТЕРВАЛУ ПОСТАНОВКИ В ПЕРЕДАЧУ
/  і інтервалу утримування передачіпісля передачі останнього байту
/------------------------------------------------------------------------------------------------*/

bool block_1s;      //блокування на 1с.
int time_1s;

bool block_100ms;   //блокування на 100 мс.
int time_100ms;


volatile unsigned int timer_uart[2];

void start_tx0()
{
    switch(uart0_status & 0x1f){
      case 0x01:
        uart0_sts_bit.start_tx = 1; //ознака початку передачі
        FIO0CLR_bit.P0_16 = 1;          //стати в передачу
        timer_uart[0] = 2;// 2/3mS
        break;
      case 0x03://інтервал постановки в передачу закінчився - почати передачу
        //Передача
        U0FCR_bit.TFR = 1;//TX FIFO RESET
        U0FCR_bit.RFR = 1;//RX FIFO RESET
        U0IER_bit.THREIE = 1;   //transmite enable

        if(uart0_wr_ptr->count){                       //закинути перший байт на передачу
            uart0_wr_ptr->count--;
            char byte = *uart0_wr_ptr->ptr;
            CRC_tx0(byte);                                //обчислення CRC
            U0THR = byte;
            if(uart0_wr_ptr->ptr)*uart0_wr_ptr->ptr++;   //і якщо є дані на передачу то інкремент поінтера
        }
        break;

      case 0x07://переключитись в прийом
        FIO0SET_bit.P0_16 = 1;                      //стати в прийом
        uart0_sts_bit.start_rx = 1;             //ознака початку прийому
        timer_uart[0] = N.UART0.wait_respond * 3;
        break;
      case 0x0F:
        if(U0LSR_bit.DR){    //якщо у буфері прийому є дані то виймати їх
            VICSoftInt_bit.INT1 = 1; //активізувати модбас
            break;
        }
        //тут вписати код обробки аваріїї відсутності відповіді
        uart0_sts_bit.not_respond = 1;
        VICSoftInt_bit.INT1 = 1; //активізувати модбас
        break;
      default:
        uart0_status = 0;
        uart0_sts_bit.timeout = 1;
        VICSoftInt_bit.INT1 = 1; //активізувати модбас
        break;

    }
    //VICSoftInt_bit.INT1 = 1; //активізувати модбас
}//end start_tx0


void start_tx1()
{
    char byte;

    switch(uart1_status & 0x1f){
      case 0x01:
        uart1_sts_bit.start_tx = 1; //ознака початку передачі
        U1MCR_bit.DTR = 1;          //стати в передачу
        /*
        T0MR1 = T0TC + 100;        //+100uS
        T0MR3 = T0TC + 300;        //+100uS
        T0IR_bit.MR3INT = 1;        //int flag reset
        T0MCR_bit.MR3INT = 1;       //enable interrupt for match channal;
//        T0MCR_bit.MR1INT = 1;       //enable interrupt for match channal 1;
        */
        timer_uart[1] = 2;
        break;
      case 0x03://інтервал постановки в передачу закінчився - почати передачу
        /*
        T0MCR_bit.MR3INT = 0;//DISable interrupt for match channal 3;
        T0IR_bit.MR3INT = 1;        //int flag reset
        T0MCR_bit.MR1INT = 0;//DISABLE INTERRUPT ON MATCH CHANNAL1
        */
        //Передача
        U1FCR_bit.TFR = 1;//TX FIFO RESET
        U1FCR_bit.RFR = 1;//RX FIFO RESET
        U1IER_bit.THREIE = 1;//transmit enable

        if(uart1_wr_ptr->count){                       //закинути перший байт на передачу
            uart1_wr_ptr->count--;
            byte = *uart1_wr_ptr->ptr;
            CRC_tx1(byte);                                //обчислення CRC
            U1THR = byte;
            if(uart1_wr_ptr->ptr)*uart1_wr_ptr->ptr++;   //і якщо є дані на передачу то інкремент поінтера
        }
        break;

      case 0x07://переключитись в прийом
        /*
        T0MCR_bit.MR3INT = 0;//DISable interrupt for match channal 3;
        T0IR_bit.MR3INT = 1;        //int flag reset
        */
        U1MCR_bit.DTR = 0;                      //стати в прийом

        uart1_sts_bit.start_rx = 1;             //ознака початку прийому
        //T0MR1 = T0TC + N.UART1.wait_respond*100;//час очікування відповіді
        //T0MCR_bit.MR1INT = 0;       //disable interrupt for match channal 1;
        modbus_receive_init();
        break;

      case 0x0F:
        if(U1LSR_bit.DR){    //якщо у буфері прийому є дані то виймати їх
            //T0MCR_bit.MR1INT = 0;//DISABLE INTERRUPT ON MATCH CHANNAL0
            break;
        }
        //тут вписати код обробки аваріїї відсутності відповіді
        uart1_sts_bit.not_respond = 1;
        break;
    }
}//end start_tx1

//==============================================================================
//для енкодера
int last_enc;//попереднє значення обох пінів енкодера
int enc_cntr;//лічильник спрацювань
int buttPT;//час натискання кнопки
int Bst;//для стану кнопки
char encdr[3];//масив значень енкодера

 void Encoder()
{
  int enc;
  
  enc = ((FIO0PIN >> 2)&3); 
  enc |= ((int)(FIO0PIN_bit.P0_10))<<2;//button
  
  
  if(++enc_cntr > 0){     
    switch(encB){
    case 0:
      if(!(enc & 4)){
        //якщо кнопка натиснута 
        switch(Bst){
        case 0:
          //перехід з відпущеної в натиснуту
          if( (last_enc & 4) && !(enc & 4) ){
            Bst=1;
            buttPT = 0;
          }
          break;
        case 1:
          buttPT++;
          if(buttPT > 3000){
            kays_push(6);//menu_key
            Bst = 2;
          }
          break;
        case 2:
          buttPT++;
          break;
        }
      }else{ 
        if(Bst && buttPT){
          //якщо кнопка була натиснута і відпущена
          if(buttPT < 3000) kays_push(7);//enc_enter
          Bst = 0;
          buttPT = 0;
        }else{
          //якщо кнопка відпущена
          if( (last_enc & 1) && !(enc & 1) ){            
            if(enc&2)kays_push(3); //вниз key  
            else kays_push(2); //up key         
          }
        }
      }
      break;
        
      default:
        if( (last_enc & 4) && !(enc & 4) ) kays_push(7);//enc_enter
        else{
          if( (last_enc & 1) && !(enc & 1) ){
            if(enc&2){
              if(encB != 0x11) encB |= 0x2;//ознака обертання енкодера якщо не BOOL
              kays_push(1); //left key 
            }else{
              if(encB != 0x11) encB |= 0x2;//ознака обертання енкодера якщо не BOOL
              kays_push(4); //right key         
            }
          }               
        }
        break;
    }
    
    enc_cntr = 0;
    last_enc = enc;  
  }
}
//==============================================================================
/* 
  for(i=0;i<sizeof(encdr)-1;i++){
    encdr[i]=encdr[i+1];//shift left all array
  }
  encdr[i] = enc;
  
  if(encdr[sizeof(encdr)-1] != encdr[sizeof(encdr)-2]) return;
  last_enc = encdr[sizeof(encdr)-3];
  enc = encdr[sizeof(encdr)-1]; 
//пошук стійких станів
  k = encdr[sizeof(encdr)-1];
  int cn[2],cS=0;
  cn[0]= 0;
  cn[1]= 0;
  for(i=sizeof(encdr)-2;i>=0;i--){
    switch (cS){
    case 0:
      if(k==encdr[i]) cn[0]++;
      else{
        if(cn[0]>=2 && i>1){
          cS=1;
          L = k;//зберегти перше стійке значення
          k=encdr[i];//взяти теперішне значення для порівняння
        }
        else return; //якщо стан останніх 3 виборок змінюється то вийти
      }
      break;
    case 1:
      if(k==encdr[i]) cn[1]++;
      else{ 
        if(cn[1]>=1) break;
        else{cn[1] =0; k=encdr[i];}//обнулити лічильник і шукати повтори 
        break;
      }
    }
  }
  
  if(cn[1]>=1){
    //отже знайшлоя ще 2 значення
    last_enc = k;
    enc = L;
  }else return;
  */
//==============================================================================  

//таймери для візуальної частини
//VIRTUAL_TIMER volatile vtimer_arr[10];
void Timer_end_funct(char i);


unsigned int time_1sa;
unsigned int time_10mS;

unsigned int time_time;//затормозити цей таймер в 30 разів бо треба 10 мс а є 1/3мс

unsigned int pid_100ms;//100мС таймер для під регуляторів
unsigned int pid_0[4];// таймер для під регуляторів

//спрацьовує кожну 1/3 мС
void timer0_isr(void)
{
    char i;

    if(timer_uart[0] != 0){
        timer_uart[0] -= 1;
        if(timer_uart[0] == 0) start_tx0();
    }

    if(timer_uart[1] != 0){
        timer_uart[1] -= 1;
        if(timer_uart[1] == 0) start_tx1();
    }

    //if(encoder)encoder--;//таймер блокування енкодера
    //Encoder();
    
    if(++time_time >= 33){
        time_time = 0;
        //ця частина має спрацьовувати раз за 10 мС
        //таймер для дисплея
        if(++time_10mS >=99)time_10mS = 0;
        spi_prtcl();//обробка сенсора і клавіатури
/*
        //віртуальні таймери
        for(i=0;i<sizeof(vtimer_arr)/sizeof(struct VIRTUAL_TIMER);i++){
            if(vtimer_arr[i].tick != 0){
                vtimer_arr[i].tick -= 1;
                if(vtimer_arr[i].tick == 0)
                    Timer_end_funct(i);
            }
        }
*/


        //================================================================================
        //односекундний таймер
        if(++time_1sa >= 99){
            time_1sa = 0;
            if(time_to_arch)time_to_arch--;//час до наступного архівування в секундах

            //організація безударної зміни робочої точки
            for(i=0;i<4;i++){
                if((i==0) && (Flr.ramp || (!Flr.last_on || !Flr.last_power))) continue;//якщо йде виводка фільєри або вона вимкнута то не змінювати завдання
                if(N.pid[i].on_off){//якщо регулятор увімкнуто
                    if(pid_r[i].Sp != N.pid[i].Sp){
                        int deltaSp;
                        deltaSp = N.pid[i].Sp - pid_r[i].Sp;

                        if(deltaSp > 0){
                            if(pid_r[i].Sp + N.pid[i].Speed > N.pid[i].Sp) pid_r[i].Sp = N.pid[i].Sp;
                            else pid_r[i].Sp += N.pid[i].Speed;
                        }else{
                            if(pid_r[i].Sp - N.pid[i].Speed < N.pid[i].Sp) pid_r[i].Sp = N.pid[i].Sp;
                            else pid_r[i].Sp -= N.pid[i].Speed;
                        }
                    }
                }
            }

            //секундомір відключення фільєри
            if(!Flr.on || !Flr.power){if(Flr.time_down < 3600)Flr.time_down++;}
            //відраховуємо час етапів
            if(Flr.time) Flr.time--;

        }
        //================================================================================


        //100mS таймер
        //================================================================================
        if(++pid_100ms >= 10){
            pid_100ms = 0;

            int out;
            int error;

            //регулятор струму
            for(i=0;i<4;i++){
                if(N.pid[i].on_off){//якщо регулятор увімкнуто
                    if(++pid_0[i] >= N.pid[i].Ts){
                        pid_0[i] = 0;
                        if(N.pid[i].Ts < 1)N.pid[i].Ts = 1;

                        switch(i){
                          case 0:
                            if(aiu_add[11].r.g >= 0){
                                if(!Flr.last_on || !Flr.last_power)error = 0 - aiu_add[11].r.g;//якщо фільєра виключена то завдання 0
                                else error = pid_r[i].Sp - aiu_add[11].r.g;
                            }else error = pid_r[i].Sp - 0;//якщо значення входу відємне то не кидати регулятор в мінус
                            break; //струм фільєри
                          case 1: error = pid_r[i].Sp - Qga.T_RG; break;//температура в зоні горіння або фідера
                          case 2: error = pid_r[i].Sp - Qga.Qa; break;//витрата повітря
                        }
                        //регулятор
                        out = UpdatePID(&N.pid[i], &pid_r[i], error);
                        //out = pid_r[i].last_out + out;
                        switch(i){
                          case 0:out = aou.current + out;break;//регулятор струму
                          case 1:out = aiu[9].d.meo.setup + out;break;//регулятор витрати газу
                          case 2:out = aiu[10].d.meo.setup + out;break;//регулятор витрати повітря
                        }
                        // обмеження вихідного сигналу
                        if(out < 0) out = 0;
                        if(out > N.pid[i].Out_Range) out = N.pid[i].Out_Range;

                        if((N.pid[i].Out_max)&&(out*10000/ N.pid[i].Out_Range > N.pid[i].Out_max)) out = N.pid[i].Out_max * N.pid[i].Out_Range / 10000;// 100.00% верхнє оюмеження вихідного сигналу
                        if((N.pid[i].Out_min)&&(out*10000/ N.pid[i].Out_Range < N.pid[i].Out_min)) out = N.pid[i].Out_min * N.pid[i].Out_Range / 10000;// 100.00% нижнє оюмеження вихідного сигналу

                        switch(i){
                          case 0://регулятор струму
                            if(!Flr.last_on || !Flr.last_power)out=0;//якщо фільєра виключена то вирубати регулятор
                            if(out != aou.current){
                                aou.current = out;
                                aum.task = (0x01 << 13*2);
                            }
                            break;
                          case 1://регулятор витрати газу
                            //out /= 10; //бо заслінка позиціонується з точністю до 0.1%
                            if(out != aiu[9].d.meo.setup){
                                aiu[9].d.meo.setup = out;
                                aum.task = (0x01 << 9*2);
                            }
                            break;
                          case 2://регулятор витрати повітря
                            //out /= 10; //бо заслінка позиціонується з точністю до 0.1%
                            if(out != aiu[10].d.meo.setup){
                                aiu[10].d.meo.setup = out;
                                aum.task = (0x01 << 10*2);
                            }
                            break;
                        }
                        pid_r[i].last_out = out;
                    }
                }
            }

        }
        //================================================================================

    }

}

///////////////////////////////////////////////////////////////////////////////



