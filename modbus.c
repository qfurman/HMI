#include <iolpc2138.h>
#include "type.h"
#include "macros.h"
#include "uart_0.h"
#include "uart_1.h"
#include "menu.hpp"
#include "timers.h"
#include "main.hpp"

#define modbufsize 256

//���������� �������� �������

#pragma pack(1)
union{
    char modbuf[modbufsize];

    struct{
        char address;
        char function;
        char exeption;
    }exception;//����������� ��� �������� ��������

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }read_req;//������� �����

    struct{
        char address;
        char function;
        char byte_count;
        unsigned short reg[125];
    }read_resp;//������� �������

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_req;//����� ������ ������� �����

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_resp;//����� ������ ������� �������

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
        unsigned char byte_count;
        unsigned short reg[123];
    }write_mult_req;//����� ������� �����

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }write_mult_resp;//����� ������� �������

}buf[2];

//������� �������

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




//��� ����������� ���������� --------------------------------

const char aou_tx[6]={0x0e,0x03,0x00,0x00,0x00,0x03};

bool all = 0;//������ �� � � � �������
bool all_r = 0;//����� ���� ����������
bool request_type;//��� ������ �� ������
bool w_s_error;//�������

char device;
//���������� ����� � ����������� ��������
/* �� ����� ���������� �� ��� ��� � ������� � �������, ��� AIU ������� ��� ������� ����� -
���������, ����� ����������� ��������� � ����� �������� ��������� � ����� ����������� � �������� ���������
��� AOU - ��������� � ����� ������ ��������� ���������
������ ������ 28 ���
*/
struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//������� �� �� ���������� �������� �����
    unsigned short err_cntr;//�������� �������

}aum;

//��� ����������� ���������� ����� --------------------------------



/*-------------------------------------------------------------------------------------------------/

    �� ������� ��������� �������� RS485/RS422 ���������

/-------------------------------------------------------------------------------------------------*/

char modbus_exchange_count;

void modbus_0()
{
    char i;
    int *i_ptr;

    VICSoftIntClear_bit.INT1 = 1;//����� �����������
    //����������
    if(uart0_sts_bit.end_rx | uart0_sts_bit.timeout | uart0_sts_bit.not_respond){
        if((CRCrx0 == 0)&&(!(buf[0].exception.function & 0x80))){
            switch(device){
              default:
                //AIU
                switch((aum.req >> device*2) & 0x03){
                  case 0:
                    //������ �������� �� �������
                    i = 0;
                    i_ptr = (int*)&aiu[device].sts;

                    *i_ptr++ = (unsigned short)litle_endian(buf[0].read_resp.reg[i]);
                    for(i=1;i<5;i=i+2)
                        *i_ptr++ = (unsigned int)litle_endian(buf[0].read_resp.reg[i])<<16 | (unsigned short)litle_endian(buf[0].read_resp.reg[i+1]);//������� ��� � �������� ���

                    if(((aum.task >> device*2) & 0x01)==0)//��� �� ������� ����
                        for(i=5;i<12;i++)
                            *i_ptr++ = (unsigned short)litle_endian(buf[0].read_resp.reg[i]);//����� ���������� �������

                    i_ptr = (int*)&aiu[device].offset;//��������� ������ �� ��������� ����� ���� �� ��������
                    if(((aum.task >> device*2) & 0x02)==0)//��� �� ������� ����
                        for(i=12;i<16;i=i+2)
                            *i_ptr++ = (unsigned int)litle_endian(buf[0].read_resp.reg[i])<<16 | (unsigned short)litle_endian(buf[0].read_resp.reg[i+1]);//������� � ���������

                    i_ptr = (int*)&aiu[device].Kg;//��������� ������ �� ��������� ����� ���� �� ��������
                    for(i=16;i<18;i++)
                        *i_ptr++ = (unsigned short)litle_endian(buf[0].read_resp.reg[i]);//����� ���������� �������

                    aiu[device].temperature = (signed short)aiu[device].temperature;

                    break;
                  default:
                    // ������ � RAM, FLASH ��� � ������ ���������
                    aum.task ^= aum.req & (0x3 <<device*2);//����� �� ����� ��������
                    aum.req ^= aum.req & (0x3 <<device*2);//�� ������  ��������
                    break;

                }
                break;
              case 13:
                //AOU
                switch((aum.req >> device*2) & 0x03){
                  case 0:
                    aou.sts= (unsigned short)litle_endian(buf[0].read_resp.reg[0]);//����� ���������� �������
                        if((aum.task >> (device*2 & 0x02))==0)//��� �� ������� ����
                            aou.current= (unsigned short)litle_endian(buf[0].read_resp.reg[1]);//����� ���������� �������
                    aou.out= (unsigned short)litle_endian(buf[0].read_resp.reg[2]);//����� ���������� �������

                    break;
                  default:
                    // ������ � RAM, FLASH ��� � ������ ���������
                    aum.task ^= aum.req & (0x3 <<device*2);//����� �� ����� ��������
                    aum.req = aum.req & ~(0x3 <<device*2);//�� ������ ����� ����� ��
                    break;
                }
            }
            aum.err &= ~(1<<device);//����� ������ ������� �������
            aum.err_cntr = 0;
            device++;
        }else{
            //������� ������� ��� ������ �������� ��� �������
            if(++aum.err_cntr > 2){
                aum.err_cntr = 0;
                aum.err |= 1<<device;
                aum.task = aum.task & ~(0x3 <<device*2);//����� �� �����
                aum.req = aum.req & ~(0x3 <<device*2);//�� ������ ����� ����� ��
                device++;
            }
        }

        if(device >=14)device = 0;

        //------------------------ ����������� ������ ------------------------------
        switch(device){
          default:
            //AIU
            switch((aum.task >> device*2) & 0x03){
              case 0:
                //������ ������ �� �������
                buf[0].read_req.address = device+1;
                buf[0].read_req.function = 0x03;//�������
                buf[0].read_req.start_address = big_endian(0x0000);
                buf[0].read_req.quantity = big_endian(18);
                wr_uart0((char*)&buf[0].modbuf[0],6,&buf[0].modbuf[0],256);
                break;

              case 1://�������� ��������� ���������
                aum.req |= aum.task & (3<<device*2);//��������� ������ �������� ������
                buf[0].write_mult_req.address = device+1;
                buf[0].write_mult_req.function = 0x10;//����� �������� �������
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

              case 2://�������� ������ ���������
                aum.req |= aum.task & (3<<device*2);//��������� ������ �������� ������
                buf[0].write_mult_req.address = device+1;
                buf[0].write_mult_req.function = 0x10;//����� �������� �������
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

              case 3://�������� �� ���������
                aum.req |= aum.task & (3<<device*2);//��������� ������ �������� ������
                buf[0].write_mult_req.address = device+1;
                buf[0].write_mult_req.function = 0x10;//����� �������� �������
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
              case 0: //����� �� ������� ��� �������
                wr_uart0((char*)&aou_tx,6,&buf[0].modbuf[0],256);
                break;
              default:
                //����� ������ ����������� ���������
                aum.req |= aum.task & (3<<device*2);//��������� ������ �������� ������
                buf[0].write_mult_req.address = device+1;
                buf[0].write_mult_req.function = 0x10;//����� �������� �������
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

    }//�����
}

/*------------------------------------------------------------------------------------------------*/
void modbus_uart0_init(void)
{
    uart0_rd_manag[0].ptr = &buf[0].modbuf[0];  //�� �������� �����
    uart0_rd_manag[0].count = 256;              //1����-�������, 2�����-������
    uart0_rd_manag[0].next = 0;                 //�����������

    uart0_rd_ptr = &uart0_rd_manag[0];          //����������� ������� �������
    CRCtx0_init();
    CRCrx0_init();
}

/*------------------------------------------------------------------------------------------------*/
void modbus_uart1_init(void)
{
    uart1_rd_manag[0].ptr = &buf[1].modbuf[0];     //�� �������� �����
    uart1_rd_manag[0].count = 256;       //1����-�������, 2�����-������
    uart1_rd_manag[0].next = 0;                 //�����������

    uart1_rd_ptr = &uart1_rd_manag[0];          //����������� ������� �������
    CRCtx1_init();
    CRCrx1_init();
    uart1_status = 0x07;//����� ������
}
/*------------------------------------------------------------------------------------------------*/
void modbus_receive_init()
{
    uart1_rd_manag[0].ptr = &buf[1].modbuf[0];     //�������� �����
    uart1_rd_manag[0].count = 256;              //�������
    uart1_rd_manag[0].next = 0;                 //����������� ����
    uart1_rd_ptr = &uart1_rd_manag[0];          //����������� ������� �������
    CRCrx1_init();
}
/*------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/












/*

    {SUB_NUM,0,"������� ����                %06d ��",  (int*)&Qga.Qg,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"������� ������              %06d ��",  (int*)&Qga.Qa,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"����������� ���� �� ���.   %05d \260",  (int*)&aiu_add[2].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"����������� � ��� ������   %05d \260",  (int*)&aiu_add[3].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"����������� ����� ���-���.  %05d \260",  (int*)&aiu_add[4].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"����������� ������           %05d \260",  (int*)&aiu_add[5].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"����������� ����             %05d \260",  (int*)&aiu_add[6].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"����������� �����������      %05d \260",  (int*)&aiu_add[7].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"����������� �������� �����. %05d \260",  (int*)&aiu_add[8].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
���    {SUB_NUM,0,"��������� ������� �����       %04d %%",  (int*)&aiu[9].d.meo.out,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"��������� ������� �����       %04d %%",  (int*)&aiu[10].d.meo.out,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"����� �������� ��������    %06d � ",  (int*)&aiu_add[11].r.g,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,0,"г���� �������� � ����      %06d ��",  (int*)&aiu_add[12].r.g,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},

���    {SUB_NUM,0,"��������� ������� ��������    %04d %%",  (int*)&aiu[9].d.meo.setup,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"��������� ������� ��������    %04d %%",  (int*)&aiu[10].d.meo.setup,0,0x7fffffff,1,0,0,0,preEnterNum,postEnter_aui11_ws_ram,UP,DOWN,LEFT,RIGHT},
    {SUB_HEX,1,"�������� ��������� �������   %04X",  (int*)&aou_curr,0,0xffff,0,0,0,0,preEnterNum,postEnter_aou_ws_ram,UP_hex,DOWN_hex,LEFT,RIGHT},

    {SUB_NUM,1,"������������ �������         %03d %%",  (int*)&aiu[12].d.vibr.duty,0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"����� ������ ��������      %05d �",  (int*)&aiu[12].d.vibr.T,0,0xffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"��� ��������� ��������      %04d %%",  (int*)&aiu[12].d.vibr.t,0,1000,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},

    � 4 ����������
*/
//������ ��������� ��� ����� � ����������
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
    0,1000,&aiu[9].d.meo.setup, //��������� �� ����� �����---------------------------------------
    0,1000,&aiu[10].d.meo.setup,
    0,65535,(int*)&aou.current,
    0,100,&N.vibr.duty,
    0,9999,&N.vibr.T,
    0,1000,&N.vibr.t,
    0,65535,&N.pid[0].Sp,//��������� ������------------------------------------------------------
    0,9999,&N.pid[0].Kp,// ����. �������������     20
    0,9999,&N.pid[0].Ti,
    0,9999,&N.pid[0].Td,
    0,9999,&N.pid[0].Ts,//��� �����������
    0,65535,&N.pid[0].Range,//������� ���� ������������� ���������
    0,65535,&N.pid[0].Out_Range,//������ ������� ���� ��������� �������
    0,65535,&N.pid[0].Speed,//�������� ���� ��������
    0,10000,&N.pid[0].Out_min,//�������� �������� ��������� �������
    0,10000,&N.pid[0].Out_max,//����������� �������� ��������� �������
    0,10000,&N.pid[0].Alarm_h,// ����� ����� ����
    0,10000,&N.pid[0].Alarm_l,// ����� ����� ����   30
    0,1,&N.pid[0].on_off,//��������� ��������� \ ����������
    0,65535,&N.pid[0].res,//������ ���� �� ���������� ��������� �� ����� �������
    0,65535,&N.pid[0].reserv[0],//������ ��� ������� ����������� �� ����������
    0,65535,&N.pid[0].reserv[1],//������ ��� ������� ����������� �� ����������
    0,65535,&N.pid[1].Sp, //��������� ������� ����--------------------------------------------------------------
    0,9999,&N.pid[1].Kp,// ����. �������������
    0,9999,&N.pid[1].Ti,
    0,9999,&N.pid[1].Td,
    0,9999,&N.pid[1].Ts,//��� �����������
    0,65535,&N.pid[1].Range,//������� ���� ������������� ���������    40
    0,65535,&N.pid[1].Out_Range,//������ ������� ���� ��������� �������
    0,65535,&N.pid[1].Speed,//�������� ���� ��������
    0,10000,&N.pid[1].Out_min,//�������� �������� ��������� �������
    0,10000,&N.pid[1].Out_max,//����������� �������� ��������� �������
    0,10000,&N.pid[1].Alarm_h,// ����� ����� ����
    0,10000,&N.pid[1].Alarm_l,// ����� ����� ����
    0,1,&N.pid[1].on_off,//��������� ��������� \ ����������
    0,1,&N.pid[1].TC,//������ ��������� ��� ���������� ���� ���� ������ ��� ���� ������  - ����
    0,65535,&N.pid[1].reserv[0],//������ ��� ������� ����������� �� ����������
    0,65535,&N.pid[1].reserv[1],//������ ��� ������� ����������� �� ����������      50
    0,65535,&N.pid[2].Sp,//��������� ������� ������ -----------------------------------------------------------------------
    0,9999,&N.pid[2].Kp,// ����. �������������
    0,9999,&N.pid[2].Ti,
    0,9999,&N.pid[2].Td,
    0,9999,&N.pid[2].Ts,//��� �����������
    0,65535,&N.pid[2].Range,//������� ���� ������������� ���������
    0,65535,&N.pid[2].Out_Range,//������ ������� ���� ��������� �������
    0,65535,&N.pid[2].Speed,//�������� ���� ��������
    0,10000,&N.pid[2].Out_min,//�������� �������� ��������� �������
    0,10000,&N.pid[2].Out_max,//����������� �������� ��������� �������                  60
    0,10000,&N.pid[2].Alarm_h,// ����� ����� ����
    0,10000,&N.pid[2].Alarm_l,// ����� ����� ����
    0,1,&N.pid[2].on_off,//��������� ��������� \ ����������
    0,1,&N.pid[2].cascad,//��� ���������� ������ ����� ������� �� �������� ����
    0,65535,&N.pid[2].reserv[0],//������ ��� ������� ����������� �� ����������
    0,65535,&N.pid[2].reserv[1],//������ ��� ������� ����������� �� ����������
    0,65535,&N.pid[3].Sp, //��������� ����-----------------------------------------------------------------------------------------
    0,9999,&N.pid[3].Kp,// ����. �������������
    0,9999,&N.pid[3].Ti,
    0,9999,&N.pid[3].Td,                                                                 // 70
    0,9999,&N.pid[3].Ts,//��� �����������
    0,65535,&N.pid[3].Range,//������� ���� ������������� ���������
    0,65535,&N.pid[3].Out_Range,//������ ������� ���� ��������� �������
    0,65535,&N.pid[3].Speed,//�������� ���� ��������
    0,10000,&N.pid[3].Out_min,//�������� �������� ��������� �������
    0,10000,&N.pid[3].Out_max,//����������� �������� ��������� �������
    0,10000,&N.pid[3].Alarm_h,// ����� ����� ����
    0,10000,&N.pid[3].Alarm_l,// ����� ����� ����
    0,1,&N.pid[3].on_off,//��������� ��������� \ ����������
    0,65535,&N.pid[3].res,//������ ���� �� ���������� ��������� �� ����� �������         //80
    0,65535,&N.pid[3].reserv[0],//������ ��� ������� ����������� �� ����������
    0,65535,&N.pid[3].reserv[1],//������ ��� ������� ����������� �� ����������
    0,65535,&N.G_A.reserv_pg,//�������� ����  --------------------------------------------------------------------------------
    0,65535,&N.G_A.reserv_pa,//�������� ������
    0,65535,&N.G_A.kg,//����. ����������� ������� ����
    0,65535,&N.G_A.ka,//����. ����������� ������� ������
    0,65535,&N.G_A.K,//���������� ����������� ���� � ������
    0,65535,&N.G_A.auto_g,//������������ ���. ���
    0,65535,&N.G_A.auto_a,//������������ ���. ������
    0,1,&N.R.auto_on,//��������� ���������� ����������� ������� //��� ��������� ������� �� �����-------------------------------- 90
    0,65535,&N.R.t1,//��������� ����� //������� � ���������
    0,65535,&N.R.t2,
    0,65535,&N.R.t3,
    0,65535,&N.R.c1,// ������
    0,65535,&N.R.c2,
    0,65535,&N.R.t5_1,//������� �� 5 ��
    0,65535,&N.R.t5_2,
    0,65535,&N.R.c5,//������� �� 1 ��.
    0,65535,&N.R.t_1,//�������� �������
    0,65535,&N.R.t_b1,                                                                                                             // 100
    0,65535,&N.R.t_b2,
    0,65535,&N.R.t_b3,
    0,65535,&N.R.c_b1,
    0,65535,&N.R.c_b2,
};
MS ms;//��� ���������� � ������ ������� ����� ��������� �� ���������
/*
typedef struct{
    int Sp;//������ �����
    int Kp;// ����. �������������
    int Ti;
    int Td;
    int Ts;//��� �����������
    int Range;//������� ���� ������������� ���������
    int Out_Range;//������ ������� ���� ��������� �������
    int Speed;//�������� ���� ��������
    int Out_min;//�������� �������� ��������� �������
    int Out_max;//����������� �������� ��������� �������
    int Alarm_h;// ����� ����� ����
    int Alarm_l;// ����� ����� ����
    int on_off;//��������� ��������� \ ����������
    union{
        int ramp;//������ ���� �� ���������� ��������� �� ����� �������
        int cascad;//��� ���������� ������ ����� ������� �� �������� ����
        int TC;//������ ��������� ��� ���������� ���� ���� ������ ��� ���� ������  - ����
    };
    int reserv[2];//������ ��� ������� ����������� �� ����������
}PID;
*/
/*
//���������� ������� ��� ����� � ����������
union{
    unsigned short r[83];
    struct{
        unsigned short Qg;//������� ����
        unsigned short Qa;//������� ������
        unsigned short Tgv;//����������� ���� �� ��������
        unsigned short Tzg;//����������� � ��� ������
        unsigned short Ts;//����������� ����� ���-���.
        unsigned short Tf;//����������� ������
        unsigned short Tw;//����������� ����
        unsigned short Tz;//����������� �����������
        unsigned short Tfil;//����������� �������� �����.
        unsigned short meog;//��������� ������� ����� ���
        unsigned short meoa;//��������� ������� ����� ������
        unsigned short CURR;//����� �������� ��������
        unsigned short basalt;//г���� �������� � ����

        unsigned short meoG; //��������� ������� ����  ��������
        unsigned short meoA; //��������� ������� ������  ��������
        unsigned short curr; //�������� ��������� ������� �������
        PID pid[4];//����������

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

    // �������� ���������� ����
    //�������� ������
	if(CRCrx1 == 0 && (buf[1].modbuf[0] == 0 || buf[1].modbuf[0] ==N.UART1.simplex)){
		switch(buf[1].modbuf[1]){			//function code
            case 3:                     //������� ������ ��� ����� ������� (�� ������� ������� � ������� RAM)
                if(litle_endian(buf[1].read_req.quantity) > 125){
                    buf[1].exception.exeption = 3; //�������� �������
                    error();
                    break;
                }
                // ���������� ������� 16-����� ������� �������
                quantity = litle_endian(buf[1].read_req.start_address);
                quantity += litle_endian(buf[1].read_req.quantity);
                if(quantity > sizeof(mma)/sizeof(MMA)){
                    buf[1].exception.exeption = 2; //������� ������ ��� ��������
                    error();
                    break;
                }

                // ��������� ������

                //i_ptr = (int*)m_adr[litle_endian(buf[1].read_req.start_address)];
                //s_ptr = (short*)&buf[1].read_resp.reg[0];
                address = litle_endian(buf[1].read_req.start_address);//������
                quantity = litle_endian(buf[1].read_req.quantity);
                buf[1].read_resp.byte_count = litle_endian(buf[1].read_req.quantity)*2;

                //��������� ��� �� ��������
                for(i=0;i<quantity;i++){
                    buf[1].read_resp.reg[i] = big_endian(*mma[address].adr);
                    if(mma[address].adr == &aiu_add[5].r.t)buf[1].read_resp.reg[i] = big_endian(aiu_add[5].r.t/10);
                    address++;

                }

                transmit_prepare(3+buf[1].read_resp.byte_count);
                start_transmit();
                break;

          case 6:
                // ���������� ������� 16-����� ������� �������
                address = litle_endian(buf[1].write_req.register_address);
                if(address >= sizeof(mma)/sizeof(MMA)){
                    buf[1].exception.exeption = 2; //������� ������ ��� ��������
                    error();
                    break;
                }
                //����²��� �������� �������
                if(mma[address].min ==0 && mma[address].max == 0){
                    buf[1].exception.exeption = 2; //������� ������ ��� ��������
                    error();
                    return;
                }
                if((litle_endian(buf[1].write_req.register_value)) < mma[address].min || (litle_endian(buf[1].write_req.register_value)) > mma[address].max){
                    buf[1].exception.exeption = 3; //������� ��������
                    error();
                    return;
                }
                //���������� ��������� ������ � ������� ��� ���� ��� � ��� ��������
                ms.addr = mma[address].adr;
                ms.count = 1;
                *mma[address].adr = litle_endian(buf[1].write_req.register_value);

                //���� �������� ��������� �������� �� ��������� ������ ������ � ����� ��������
                if((address > 15)&&(address < 19)){
                    aiu[12].d.vibr.duty = N.vibr.duty;
                    aiu[12].d.vibr.T = N.vibr.T;
                    aiu[12].d.vibr.t = N.vibr.t;
                    aum.task = (0x01 << 12*2);
                }

            transmit_prepare(4+2);
            start_transmit();
            break;

          case 16:                     //����� ������ ��� ����� �������
                if(litle_endian(buf[1].write_mult_req.quantity) > sizeof(mma)/sizeof(MMA)){
                    buf[1].exception.exeption = 3; //�������� �������
                    error();
                    break;
                }
                // ���������� ������� 16-����� ������� �������
                quantity = litle_endian(buf[1].write_mult_req.start_address);
                quantity += litle_endian(buf[1].write_mult_req.quantity);
                if(quantity > sizeof(mma)/sizeof(MMA)){
                    buf[1].exception.exeption = 2; //������� ������ ��� ��������
                    error();
                    break;
                }

				//����²��� �������� �������
                off = 0;//��� ���� �� ������ �������
                address = litle_endian(buf[1].write_mult_req.start_address);//������
                quantity = litle_endian(buf[1].write_mult_req.quantity);

                for(i=address;i<address + quantity;i++){
                    if(mma[i].min ==0 && mma[i].max == 0){
                        buf[1].exception.exeption = 2; //������� ������ ��� ��������
                        error();
                        return;
                    }
                    if((litle_endian(buf[1].write_mult_req.reg[off])) < mma[i].min || (litle_endian(buf[1].write_mult_req.reg[off])) > mma[i].max){
                        buf[1].exception.exeption = 3; //������� ��������
                        error();
                        return;
                    }
                    off++;
                }
                //���������� ��������� ������ � ������� ��� ���� ��� � ��� ��������
                ms.addr = mma[address].adr;
                ms.count = quantity;
                //���������
                off = 0;
                for(i=address;i<address + quantity;i++){
                    *mma[i].adr = litle_endian(buf[1].write_mult_req.reg[off]);
                    off++;
                }

                //���� �������� ��������� �������� �� ��������� ������ ������ � ����� ��������
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
                /* ������� �� ����������� */
                buf[1].exception.exeption = 1;
                error();
                break;
        }

        //�������� ������ �� 3,5�������
	}
	else{
    //����������� ��������� ������� ���� �������
	    U1FCR_bit.TFR = 1;//TX FIFO RESET
        U1FCR_bit.RFR = 1;//RX FIFO RESET
        //uart1_status = 0x07;//����� ������
        modbus_uart1_init();
        /*
        uart1_rd_manag[0].ptr = &buf[1].modbuf[0];     //�������� �����
        uart1_rd_manag[0].count = 256;              //�������
        uart1_rd_manag[0].next = 0;                 //����������� ����
        uart1_rd_ptr = &uart1_rd_manag[0];          //����������� ������� �������
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
    uart1_wr_crc.ptr = &crc_buf1[0];            //�� ������� ��� ����������� ���������� ����
    uart1_wr_crc.count = 2;                     //�������������  � ���������� ���� ���������� ���
    uart1_wr_crc.next = 0;

    uart1_wr_manag[0].ptr = &buf[1].modbuf[0];         //�������
    uart1_wr_manag[0].count = count;            //������� �����
    uart1_wr_manag[0].next = 0;                 //����� ����
    uart1_wr_ptr = &uart1_wr_manag[0];          //����������� ������� ������

    uart1_rd_manag[0].ptr = 0;     //�������� �����
    uart1_rd_manag[0].count = 0;              //�������
    uart1_rd_manag[0].next = 0;                 //����������� ����
    uart1_rd_ptr = &uart1_rd_manag[0];          //����������� ������� �������
    CRCtx1_init();
    CRCrx1_init();
}

void start_transmit()
{
    uart1_status = 0x00;        //���� ������� ������� � �������� start_tx = 1 start_rx = 1;
    uart1_sts_bit.pre_time = 1; //������ ������� ����� ������ ���������
    //uart1_sts_bit.start_tx = 1; //������ ������� ��������
    //U1MCR_bit.DTR = 1;          //����� � ��������
    /*
    T0MR1 = T0TC + N.UART1.wait_respond*1000;        //����� �� �������� � ���������
    T0IR_bit.MR1INT = 1;        //int flag reset
    T0MCR_bit.MR1INT = 1;       //enable interrupt for match channal 1;
    */
    timer_uart[1] = N.UART1.wait_respond*3;//����� �� �������� � ���������

}







