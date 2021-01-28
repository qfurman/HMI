

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
/  ����Ͳ��ֲ� ��������� ���������� � ��������
/  � ��������� ����������� ������������ �������� ���������� �����
/------------------------------------------------------------------------------------------------*/

bool block_1s;      //���������� �� 1�.
int time_1s;

bool block_100ms;   //���������� �� 100 ��.
int time_100ms;


volatile unsigned int timer_uart[2];

void start_tx0()
{
    switch(uart0_status & 0x1f){
      case 0x01:
        uart0_sts_bit.start_tx = 1; //������ ������� ��������
        FIO0CLR_bit.P0_16 = 1;          //����� � ��������
        timer_uart[0] = 2;// 2/3mS
        break;
      case 0x03://�������� ���������� � �������� ��������� - ������ ��������
        //��������
        U0FCR_bit.TFR = 1;//TX FIFO RESET
        U0FCR_bit.RFR = 1;//RX FIFO RESET
        U0IER_bit.THREIE = 1;   //transmite enable

        if(uart0_wr_ptr->count){                       //�������� ������ ���� �� ��������
            uart0_wr_ptr->count--;
            char byte = *uart0_wr_ptr->ptr;
            CRC_tx0(byte);                                //���������� CRC
            U0THR = byte;
            if(uart0_wr_ptr->ptr)*uart0_wr_ptr->ptr++;   //� ���� � ��� �� �������� �� ��������� �������
        }
        break;

      case 0x07://������������� � ������
        FIO0SET_bit.P0_16 = 1;                      //����� � ������
        uart0_sts_bit.start_rx = 1;             //������ ������� �������
        timer_uart[0] = N.UART0.wait_respond * 3;
        break;
      case 0x0F:
        if(U0LSR_bit.DR){    //���� � ����� ������� � ��� �� ������� ��
            VICSoftInt_bit.INT1 = 1; //����������� ������
            break;
        }
        //��� ������� ��� ������� ���𳿿 ��������� ������
        uart0_sts_bit.not_respond = 1;
        VICSoftInt_bit.INT1 = 1; //����������� ������
        break;
      default:
        uart0_status = 0;
        uart0_sts_bit.timeout = 1;
        VICSoftInt_bit.INT1 = 1; //����������� ������
        break;

    }
    //VICSoftInt_bit.INT1 = 1; //����������� ������
}//end start_tx0


void start_tx1()
{
    char byte;

    switch(uart1_status & 0x1f){
      case 0x01:
        uart1_sts_bit.start_tx = 1; //������ ������� ��������
        U1MCR_bit.DTR = 1;          //����� � ��������
        /*
        T0MR1 = T0TC + 100;        //+100uS
        T0MR3 = T0TC + 300;        //+100uS
        T0IR_bit.MR3INT = 1;        //int flag reset
        T0MCR_bit.MR3INT = 1;       //enable interrupt for match channal;
//        T0MCR_bit.MR1INT = 1;       //enable interrupt for match channal 1;
        */
        timer_uart[1] = 2;
        break;
      case 0x03://�������� ���������� � �������� ��������� - ������ ��������
        /*
        T0MCR_bit.MR3INT = 0;//DISable interrupt for match channal 3;
        T0IR_bit.MR3INT = 1;        //int flag reset
        T0MCR_bit.MR1INT = 0;//DISABLE INTERRUPT ON MATCH CHANNAL1
        */
        //��������
        U1FCR_bit.TFR = 1;//TX FIFO RESET
        U1FCR_bit.RFR = 1;//RX FIFO RESET
        U1IER_bit.THREIE = 1;//transmit enable

        if(uart1_wr_ptr->count){                       //�������� ������ ���� �� ��������
            uart1_wr_ptr->count--;
            byte = *uart1_wr_ptr->ptr;
            CRC_tx1(byte);                                //���������� CRC
            U1THR = byte;
            if(uart1_wr_ptr->ptr)*uart1_wr_ptr->ptr++;   //� ���� � ��� �� �������� �� ��������� �������
        }
        break;

      case 0x07://������������� � ������
        /*
        T0MCR_bit.MR3INT = 0;//DISable interrupt for match channal 3;
        T0IR_bit.MR3INT = 1;        //int flag reset
        */
        U1MCR_bit.DTR = 0;                      //����� � ������

        uart1_sts_bit.start_rx = 1;             //������ ������� �������
        //T0MR1 = T0TC + N.UART1.wait_respond*100;//��� ���������� ������
        //T0MCR_bit.MR1INT = 0;       //disable interrupt for match channal 1;
        modbus_receive_init();
        break;

      case 0x0F:
        if(U1LSR_bit.DR){    //���� � ����� ������� � ��� �� ������� ��
            //T0MCR_bit.MR1INT = 0;//DISABLE INTERRUPT ON MATCH CHANNAL0
            break;
        }
        //��� ������� ��� ������� ���𳿿 ��������� ������
        uart1_sts_bit.not_respond = 1;
        break;
    }
}//end start_tx1

//==============================================================================
//��� ��������
int last_enc;//�������� �������� ���� ��� ��������
int enc_cntr;//�������� ����������
int buttPT;//��� ���������� ������
int Bst;//��� ����� ������
char encdr[3];//����� ������� ��������

 void Encoder()
{
  int enc;
  
  enc = ((FIO0PIN >> 2)&3); 
  enc |= ((int)(FIO0PIN_bit.P0_10))<<2;//button
  
  
  if(++enc_cntr > 0){     
    switch(encB){
    case 0:
      if(!(enc & 4)){
        //���� ������ ��������� 
        switch(Bst){
        case 0:
          //������� � �������� � ���������
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
          //���� ������ ���� ��������� � ��������
          if(buttPT < 3000) kays_push(7);//enc_enter
          Bst = 0;
          buttPT = 0;
        }else{
          //���� ������ ��������
          if( (last_enc & 1) && !(enc & 1) ){            
            if(enc&2)kays_push(3); //���� key  
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
              if(encB != 0x11) encB |= 0x2;//������ ��������� �������� ���� �� BOOL
              kays_push(1); //left key 
            }else{
              if(encB != 0x11) encB |= 0x2;//������ ��������� �������� ���� �� BOOL
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
//����� ������ �����
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
          L = k;//�������� ����� ����� ��������
          k=encdr[i];//����� �������� �������� ��� ���������
        }
        else return; //���� ���� ������� 3 ������� ��������� �� �����
      }
      break;
    case 1:
      if(k==encdr[i]) cn[1]++;
      else{ 
        if(cn[1]>=1) break;
        else{cn[1] =0; k=encdr[i];}//�������� �������� � ������ ������� 
        break;
      }
    }
  }
  
  if(cn[1]>=1){
    //���� �������� �� 2 ��������
    last_enc = k;
    enc = L;
  }else return;
  */
//==============================================================================  

//������� ��� �������� �������
//VIRTUAL_TIMER volatile vtimer_arr[10];
void Timer_end_funct(char i);


unsigned int time_1sa;
unsigned int time_10mS;

unsigned int time_time;//����������� ��� ������ � 30 ���� �� ����� 10 �� � � 1/3��

unsigned int pid_100ms;//100�� ������ ��� �� ����������
unsigned int pid_0[4];// ������ ��� �� ����������

//��������� ����� 1/3 ��
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

    //if(encoder)encoder--;//������ ���������� ��������
    //Encoder();
    
    if(++time_time >= 33){
        time_time = 0;
        //�� ������� �� ������������� ��� �� 10 ��
        //������ ��� �������
        if(++time_10mS >=99)time_10mS = 0;
        spi_prtcl();//������� ������� � ���������
/*
        //�������� �������
        for(i=0;i<sizeof(vtimer_arr)/sizeof(struct VIRTUAL_TIMER);i++){
            if(vtimer_arr[i].tick != 0){
                vtimer_arr[i].tick -= 1;
                if(vtimer_arr[i].tick == 0)
                    Timer_end_funct(i);
            }
        }
*/


        //================================================================================
        //������������� ������
        if(++time_1sa >= 99){
            time_1sa = 0;
            if(time_to_arch)time_to_arch--;//��� �� ���������� ����������� � ��������

            //���������� ��������� ���� ������ �����
            for(i=0;i<4;i++){
                if((i==0) && (Flr.ramp || (!Flr.last_on || !Flr.last_power))) continue;//���� ��� ������� ������� ��� ���� �������� �� �� �������� ��������
                if(N.pid[i].on_off){//���� ��������� ��������
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

            //��������� ���������� �������
            if(!Flr.on || !Flr.power){if(Flr.time_down < 3600)Flr.time_down++;}
            //���������� ��� �����
            if(Flr.time) Flr.time--;

        }
        //================================================================================


        //100mS ������
        //================================================================================
        if(++pid_100ms >= 10){
            pid_100ms = 0;

            int out;
            int error;

            //��������� ������
            for(i=0;i<4;i++){
                if(N.pid[i].on_off){//���� ��������� ��������
                    if(++pid_0[i] >= N.pid[i].Ts){
                        pid_0[i] = 0;
                        if(N.pid[i].Ts < 1)N.pid[i].Ts = 1;

                        switch(i){
                          case 0:
                            if(aiu_add[11].r.g >= 0){
                                if(!Flr.last_on || !Flr.last_power)error = 0 - aiu_add[11].r.g;//���� ������� ��������� �� �������� 0
                                else error = pid_r[i].Sp - aiu_add[11].r.g;
                            }else error = pid_r[i].Sp - 0;//���� �������� ����� ����� �� �� ������ ��������� � ����
                            break; //����� �������
                          case 1: error = pid_r[i].Sp - Qga.T_RG; break;//����������� � ��� ������ ��� ������
                          case 2: error = pid_r[i].Sp - Qga.Qa; break;//������� ������
                        }
                        //���������
                        out = UpdatePID(&N.pid[i], &pid_r[i], error);
                        //out = pid_r[i].last_out + out;
                        switch(i){
                          case 0:out = aou.current + out;break;//��������� ������
                          case 1:out = aiu[9].d.meo.setup + out;break;//��������� ������� ����
                          case 2:out = aiu[10].d.meo.setup + out;break;//��������� ������� ������
                        }
                        // ��������� ��������� �������
                        if(out < 0) out = 0;
                        if(out > N.pid[i].Out_Range) out = N.pid[i].Out_Range;

                        if((N.pid[i].Out_max)&&(out*10000/ N.pid[i].Out_Range > N.pid[i].Out_max)) out = N.pid[i].Out_max * N.pid[i].Out_Range / 10000;// 100.00% ����� ��������� ��������� �������
                        if((N.pid[i].Out_min)&&(out*10000/ N.pid[i].Out_Range < N.pid[i].Out_min)) out = N.pid[i].Out_min * N.pid[i].Out_Range / 10000;// 100.00% ���� ��������� ��������� �������

                        switch(i){
                          case 0://��������� ������
                            if(!Flr.last_on || !Flr.last_power)out=0;//���� ������� ��������� �� �������� ���������
                            if(out != aou.current){
                                aou.current = out;
                                aum.task = (0x01 << 13*2);
                            }
                            break;
                          case 1://��������� ������� ����
                            //out /= 10; //�� ������� ������������� � ������� �� 0.1%
                            if(out != aiu[9].d.meo.setup){
                                aiu[9].d.meo.setup = out;
                                aum.task = (0x01 << 9*2);
                            }
                            break;
                          case 2://��������� ������� ������
                            //out /= 10; //�� ������� ������������� � ������� �� 0.1%
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



