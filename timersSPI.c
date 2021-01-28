

#include <stdbool.h>
#include "iolpc2138.h"
#include "fiolpc2138.h"
#include "type.h"
#include "uart_0.h"
#include "uart_1.h"
#include "macros.h"
#include "menu.hpp"
#include "touch.hpp"
#include "modbus.h"
#include "main.hpp"
#include "spi.h"
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



//������� ��� �������� �������
//VIRTUAL_TIMER volatile vtimer_arr[10];
void Timer_end_funct(char i);


unsigned int time_1sa;
unsigned int time_10mS;

unsigned int time_time;//����������� ��� ������ � 30 ���� �� ����� 10 �� � � 1/3��

//��������� ����� 1/3 ��
void timer0_isr(void)
{
    //char i;

    if(timer_uart[0] != 0){
        timer_uart[0] -= 1;
        if(timer_uart[0] == 0) start_tx0();
    }

    if(timer_uart[1] != 0){
        timer_uart[1] -= 1;
        if(timer_uart[1] == 0) start_tx1();
    }

    if(++time_time >= 30){
        time_time = 0;
        //�� ������� �� ������������� ��� �� 10 ��
        //������ ��� �������
        if(++time_10mS >=99)time_10mS = 0;
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
        if(++time_1sa >= 100){//������������� ������
            time_1sa = 0;
            if(time_to_arch)time_to_arch--;//��� �� ���������� ����������� � ��������
        }

    }

}

///////////////////////////////////////////////////////////////////////////////
/*
struct SPI_MNG{
    char device;//����� ��� ����������
    char *src;//�� ����������
    char *dest;//���� ��������
    char count;//������ �����
    int spi_front;//�������� ���� ������
};
typedef struct SPI_MNG;
*/
SPI_MNG spi_mng;

#define set_sck FIO0SET_bit.P0_4=1
#define clr_sck FIO0CLR_bit.P0_4=1
#define set_mosi FIO0SET_bit.P0_6=1
#define clr_mosi FIO0CLR_bit.P0_6=1

bool first_data;//��������� ������ �� �����

void timer1_isr(void)
{
    char pos;
    char one_byte;

    //T1MR0 = T1TC + 600;        //   /60 uS

    //������� ���� �� ������� ���� ���������� � ��������� ������� �����
    if(spi_mng.count){

        switch(first_data){
          case false:
            // CS � ������ ��
            switch(spi_mng.device){
              case 0: FIO0CLR_bit.P0_7 = 1;break;//ssel0 = 0
              case 1: FIO0CLR_bit.P0_21 = 1;break;//CS00 = 0
              case 2: FIO0CLR_bit.P0_22 = 1;break;//CS01 = 0
              case 3: FIO0CLR_bit.P0_28 = 1;break;//CS02 = 0
              case 4: FIO0CLR_bit.P0_29 = 1;break;//CS03 = 0
            }
            //������ �� ������� ������ ��������� ����� ������ �������
            if(*spi_mng.src & 0x80) set_mosi;
            else clr_mosi;
            first_data = true;
            break;

          default:
            //������ ����� ��� ������ ���� ��� ������
            switch(spi_mng.spi_front%2){
              case 0:
              //�������� MISO
              if(FIO0PIN & (1<<5))
                  *(spi_mng.dest + (spi_mng.spi_front >> 4)) |= 1<<(7-((spi_mng.spi_front>>1)&0x07));
              else
                  *(spi_mng.dest + (spi_mng.spi_front >> 4)) &= ~(1<<(7-((spi_mng.spi_front>>1)&0x07)));
                set_sck;//sck0 = 1
              break;

              case 1:
                pos = 1<<(7-(((spi_mng.spi_front+1)>>1)&0x07));
                one_byte =*(spi_mng.src + ((spi_mng.spi_front+1)>>4));
                if(one_byte & pos)set_mosi;
                else clr_mosi;
                clr_sck; //sck0 = 0
              break;
            }

            //�������� ��������� � �������� �� ����� ������
            if(++spi_mng.spi_front>>4 >= spi_mng.count){
                first_data = false;
                spi_mng.spi_front = 0;
                spi_mng.count = 0;
                FIO0SET_bit.P0_7 = 1;       //ssel0 = 1
                FIO0SET_bit.P0_21 = 1;      //CS00 = 1
                FIO0SET_bit.P0_22 = 1;      //CS01 = 1
                FIO0SET_bit.P0_28 = 1;      //CS02 = 1
                FIO0SET_bit.P0_29 = 1;      //CS03 = 1
                    spi0_sts_bit.end_tx = 1;
                    spi0_sts_bit.end_rx = 1;
            }
        }
    }
    //�����  ����������� SPI
}

void Timer_end_funct(char i)
{

}


