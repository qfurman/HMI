
/*
*****************************************************************************
**
**      Project     : My project
**
**      Component   : MakeApp (LPC2138)
**
**      Modulename  : System
**
**      Filename    : template.c
**
**      Abstract    : This file is the template file for an application
**                    Rename the file from <template.c> to e.g. <usercode.c>
**                    The file contains an example of main function
**
**                    Modify the <usercode.c> to suit your application.
**                    The <usercode.c> file will not be overwritten by MakeApp.
**
**      Date        : 2006-02-08 10:26:02
**
**      License no. : 9532-355-016-8098     Furman's
**
**      Warning     : This file has been automatically generated.
**                    Do not edit this file if you intend to regenerate it.
**
**      This template file was created by IAR MakeApp version
**      4.10C (Philips LPC2138-48: 4.01B) for the Philips LPC2138 series of
**      microcontrollers.
**
**      (c)Copyright 2004-2005 MakeApp Consulting.
**
*****************************************************************************
*/
/*
**===========================================================================
**  1       GENERAL
**  1.1     Revisions
**
**  Please read the IAR MakeApp for Philips LPC2138-48 readme file
**
**
**===========================================================================
*/

/*
**===========================================================================
**  1.2     References
**
**  No   Identification          Name or Description
**  ==   ===================     ================================
**  1    November 22, 2004       Philips LPC2131/2132/2138
**                               User Manual, Preliminary Release
**
**===========================================================================
*/

/*
**===========================================================================
**  2.      INCLUDE FILES
**  2.1     Standard include files
**===========================================================================
*/
#include "type.h"
#include "timers.h"
#include "intrinsics.h"
#include "usercode.h"   /* Usercode macros (see <template.h>) */
#include <iolpc2138.h>
#include "macros.h"
#include "menu.hpp"
/*
**===========================================================================
**  2.2     Application include files
**===========================================================================
*/


/*
**---------------------------------------------------
**  IAR MakeApp peripheral modules header files
**  Include the header files used by the application
**---------------------------------------------------
*/


/*
**===========================================================================
**  3.      DECLARATIONS
**  3.1     Internal constants
**===========================================================================
*/

unsigned short const CRC_table[] = {
0x0000, 0xC1C0, 0x81C1, 0x4001, 0x01C3, 0xC003, 0x8002, 0x41C2, 0x01C6, 0xC006,
0x8007, 0x41C7, 0x0005, 0xC1C5, 0x81C4, 0x4004, 0x01CC, 0xC00C, 0x800D, 0x41CD,
0x000F, 0xC1CF, 0x81CE, 0x400E, 0x000A, 0xC1CA, 0x81CB, 0x400B, 0x01C9, 0xC009,
0x8008, 0x41C8, 0x01D8, 0xC018, 0x8019, 0x41D9, 0x001B, 0xC1DB, 0x81DA, 0x401A,
0x001E, 0xC1DE, 0x81DF, 0x401F, 0x01DD, 0xC01D, 0x801C, 0x41DC, 0x0014, 0xC1D4,
0x81D5, 0x4015, 0x01D7, 0xC017, 0x8016, 0x41D6, 0x01D2, 0xC012, 0x8013, 0x41D3,
0x0011, 0xC1D1, 0x81D0, 0x4010, 0x01F0, 0xC030, 0x8031, 0x41F1, 0x0033, 0xC1F3,
0x81F2, 0x4032, 0x0036, 0xC1F6, 0x81F7, 0x4037, 0x01F5, 0xC035, 0x8034, 0x41F4,
0x003C, 0xC1FC, 0x81FD, 0x403D, 0x01FF, 0xC03F, 0x803E, 0x41FE, 0x01FA, 0xC03A,
0x803B, 0x41FB, 0x0039, 0xC1F9, 0x81F8, 0x4038, 0x0028, 0xC1E8, 0x81E9, 0x4029,
0x01EB, 0xC02B, 0x802A, 0x41EA, 0x01EE, 0xC02E, 0x802F, 0x41EF, 0x002D, 0xC1ED,
0x81EC, 0x402C, 0x01E4, 0xC024, 0x8025, 0x41E5, 0x0027, 0xC1E7, 0x81E6, 0x4026,
0x0022, 0xC1E2, 0x81E3, 0x4023, 0x01E1, 0xC021, 0x8020, 0x41E0, 0x01A0, 0xC060,
0x8061, 0x41A1, 0x0063, 0xC1A3, 0x81A2, 0x4062, 0x0066, 0xC1A6, 0x81A7, 0x4067,
0x01A5, 0xC065, 0x8064, 0x41A4, 0x006C, 0xC1AC, 0x81AD, 0x406D, 0x01AF, 0xC06F,
0x806E, 0x41AE, 0x01AA, 0xC06A, 0x806B, 0x41AB, 0x0069, 0xC1A9, 0x81A8, 0x4068,
0x0078, 0xC1B8, 0x81B9, 0x4079, 0x01BB, 0xC07B, 0x807A, 0x41BA, 0x01BE, 0xC07E,
0x807F, 0x41BF, 0x007D, 0xC1BD, 0x81BC, 0x407C, 0x01B4, 0xC074, 0x8075, 0x41B5,
0x0077, 0xC1B7, 0x81B6, 0x4076, 0x0072, 0xC1B2, 0x81B3, 0x4073, 0x01B1, 0xC071,
0x8070, 0x41B0, 0x0050, 0xC190, 0x8191, 0x4051, 0x0193, 0xC053, 0x8052, 0x4192,
0x0196, 0xC056, 0x8057, 0x4197, 0x0055, 0xC195, 0x8194, 0x4054, 0x019C, 0xC05C,
0x805D, 0x419D, 0x005F, 0xC19F, 0x819E, 0x405E, 0x005A, 0xC19A, 0x819B, 0x405B,
0x0199, 0xC059, 0x8058, 0x4198, 0x0188, 0xC048, 0x8049, 0x4189, 0x004B, 0xC18B,
0x818A, 0x404A, 0x004E, 0xC18E, 0x818F, 0x404F, 0x018D, 0xC04D, 0x804C, 0x418C,
0x0044, 0xC184, 0x8185, 0x4045, 0x0187, 0xC047, 0x8046, 0x4186, 0x0182, 0xC042,
0x8043, 0x4183, 0x0041, 0xC181, 0x8180, 0x4040
};

/*
**===========================================================================
**  3.2     Internal macros
**===========================================================================
*/

#define RLS_ 3
#define RDA_ 2
#define CTI_ 6
#define THRE_ 1

/*
**===========================================================================
**  3.3     Internal type definitions
**===========================================================================
*/

/*
**===========================================================================
**  3.4     Global variables (declared as 'extern' in some header file)
**===========================================================================
*/

/*
**===========================================================================
**  3.5     Internal function prototypes (defined in Section 5)
**===========================================================================
*/

/*
**===========================================================================
**  3.6     Internal variables
**===========================================================================
*/
unsigned short CRCtx0,CRCrx0;

LIST *uart0_wr_ptr; //global pointer for outgoing thread
LIST *uart0_rd_ptr;  //global pointer for ingoing thread

union{
    char uart0_status;
    RXTX uart0_sts_bit;
};

/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (declared as 'extern' in some header file)
**===========================================================================
*/

/*
struct list{
    char *ptr;              //Operands pointer
    unsigned int count;     //count of bytes
    struct list *next;      //pointer for next element of list
};
*/

//RESPONSE_HEAD rx_temp_head;

LIST uart0_wr_manag[3];
LIST uart0_rd_manag[2];

char crc_buf0[2];
LIST uart0_wr_crc;  //��� �������� ���������� ����

void wr_uart0(char *src, unsigned int src_count, char *dest, unsigned int dest_count)
{

    uart0_wr_crc.ptr = &crc_buf0[0];             //�� ������� ��� ����������� ���������� ����
    uart0_wr_crc.count = 2;                     //�������������  � ����������� ���� ���������� ���
    uart0_wr_crc.next = 0;

    uart0_wr_manag[0].ptr = src;                //�������
    uart0_wr_manag[0].count = src_count;        //������� �����
    uart0_wr_manag[0].next = 0;                 //����� ����

    uart0_rd_manag[0].ptr = dest;               //�� �������� �����
    uart0_rd_manag[0].count = dest_count;       //1����-�������, 2�����-������
    uart0_rd_manag[0].next = 0;                 //�����������

    uart0_wr_ptr = &uart0_wr_manag[0];          //������������ ������� ������
    uart0_rd_ptr = &uart0_rd_manag[0];          //������������ ������� �������

    CRCtx0_init();
    CRCrx0_init();
    //N.UART1.simplex
    if(uart0_sts_bit.not_respond){  //���� ������ ������ �� ���� ����� � ����������� �������
        uart0_status = 0x00;        //���� ������� ������� � �������� start_tx = 1 start_rx = 1;
        uart0_sts_bit.pre_time = 1; //������ ������� ����� ������ ���������
        uart0_sts_bit.start_tx = 1; //������ ������� ��������
        FIO0CLR_bit.P0_16 = 1;       //����� � ��������
        timer_uart[0] = 2;  // 2/3 ��
    }
    else{
        uart0_status = 0x00;        //���� ������� ������� � �������� start_tx = 1 start_rx = 1;
        uart0_sts_bit.pre_time = 1; //������ ������� ����� ������ ���������
        timer_uart[0] = N.UART0.timeout*3;//����� �����
    }
}

void MA_IntHandler_UART0( void )
/*
**---------------------------------------------------------------------------
**
**  Abstract:
**      Takes care of the UART 1 interrupt.
**
**  Parameters:
**      None
**
**  Returns:
**      None
**
**---------------------------------------------------------------------------
*/
{
    volatile char dummy;
    char un;

    //if(U0IIR_bit.IP)return;//if no pending interrupts

    switch(U0IIR_bit.IID){//interrupt identification
      case RLS_: //recive line status
            dummy = U0LSR;
        break;

      case CTI_: //caracter timeout indicator
            uart0_sts_bit.timeout = 1;  //������ ������� �������� �������
            VICSoftInt_bit.INT1 = 1; //����������� ������
      case RDA_: //recive data available
        if(uart0_sts_bit.timeout)un = 16;//������� ���
        else un = 13;//�������� ����������� ���� ��� ��������� �������� ��������
        do{
            if(U0LSR_bit.DR){                      //���� � ����� ������� � ���� �� ������� ��
                if(uart0_rd_ptr->count){                 //���� � ���� �������
                    if(uart0_rd_ptr->ptr){               //���� ����� �������� � �� ������ ����������
                        char byte = U0RBR;
                        *uart0_rd_ptr->ptr++ = byte;    //���� ������ �� �������� �� ����������� ��� ��������� ����
                        CRC_rx0(byte);
                    }
                    else dummy = U0RBR;                 //������� ���������� ����

                    if(--uart0_rd_ptr->count == 0){      //��������� ������� ���� ��������� �� ��� �������� ���� ������� 0 �� ���������� ������� ��� �� �������
                        if(uart0_rd_ptr->next) uart0_rd_ptr = uart0_rd_ptr->next;//�������� �� � �������� ���� ��� ������� � ����� ���������� �������� ������ � ������� ������� ��������
                        else uart0_sts_bit.end_rx = 1;    //������ ���� �������
                    }
                }
                else dummy = U0RBR;//U0FCR_bit.RFR = 1;//RECIEV FIFO RESET//dummy = U0RBR; //������� ���������� ���� ��� ����������� ����
                //T0MCR_bit.MR0INT = 0;//��� �� �������� �� ������ ���� ������� ��� ����� ������� ����
            }
            else{
                dummy = U0RBR;
                break;
            }
        }while(--un);
        break;

      case THRE_:
        do{
            //if(uart0_sts_bit.end_tx)break;
            //��������
            if(uart0_wr_ptr->count){                             //���� �������� �� �� �� ���� �� ��� ���������
                if(U0LSR_bit.THRE){                              //���� FIFO �� �����������
                    uart0_wr_ptr->count--;                       //��������� ������� ���� �� ��������
                    char byte = *uart0_wr_ptr->ptr;
                        CRC_tx0(byte);
                        U0THR = byte;
                    if(uart0_wr_ptr->ptr)*uart0_wr_ptr->ptr++;   //� ���� � ���� �� �������� �� ��������� �������
                }else break; //��� ������������� ���� ��������� ������ ��� ���������� �����������
            }
            else{
                if(uart0_wr_ptr->next)                   //�������� �� � ��������� ���� ����� �� ��������
                    uart0_wr_ptr = uart0_wr_ptr->next;   //����� ���������� �������� ������ � ������� ������� ��������
                else{
                    if(!uart0_sts_bit.cs){
                        uart0_sts_bit.cs = 1;//CRC ���� ����������
                        uart0_wr_ptr = &uart0_wr_crc;
                        crc_buf0[0] = CRCtx0>>8;
                        crc_buf0[1] = CRCtx0;
                        //U0THR = CRCtx>>8;
                        //U0THR = CRCtx;
                        //return;
                    }else{
                        U0IER_bit.THREIE = 0; //������� ��������
                        uart0_sts_bit.end_tx = 1;//������ ���� ��������
                        timer_uart[0] = 60000/N.UART0.bitrate + 2;//��� �� �������� ����� + 2/3 ��
                        break;
                    }
                }
            }
        }while(1);
        break;
    }

} /* MA_IntHandler_uart0 */


void uart0_bitrate(int bitrate)
{
    char u0ier_back = U0IER;

    bitrate = 14745600*4/16/bitrate;

    U0IER = 0; //disable all interrupt from that uart

    U0LCR_bit.DLAB = 1;
    U0DLL = bitrate & 0xff;
    U0DLM = bitrate >> 8 & 0xff;
    U0LCR_bit.DLAB = 0;

    U0IER = u0ier_back;//enable interrupt from that uart
}

void uart0_word_length(char temp)
{
    //�������� 5,6,7,8
    temp = temp - 5;
    U0LCR_bit.WLS = temp;//������� �����
}

void uart0_stop_bit(char temp)
{
    //�������� 1,2 ����� 0,1
    temp -= 1;// 1 or 2(1.5for 5bit word length) stop bits
    U0LCR_bit.SBS = temp;
}

void uart0_parity_enable(char temp)
{
    //0 - disable 1 - enable
    U0LCR_bit.PE = temp;
}

void uart0_parity_select(char temp)
{
    // �������� odd, even, 1, 0  = 0 1 2 3
    U0LCR_bit.PS = temp;
}

void uart0_setup(int bitrate,char word_length,char stop_bit,char parity_enable, char parity_select)
{
    U0IER = 0; //disable all interrupt from that uart

    bitrate = 14745600*4/16/bitrate;

    U0LCR_bit.DLAB = 1;
    U0DLL = bitrate & 0xff;
    U0DLM = bitrate >> 8 & 0xff;
    U0LCR_bit.DLAB = 0;

    //�������� 5,6,7,8
    word_length = word_length - 5;
    U0LCR_bit.WLS = word_length;//������� �����

    //�������� 1,2 ����� 0,1
    stop_bit -= 1;// 1 or 2(1.5for 5bit word length) stop bits
    U0LCR_bit.SBS = stop_bit;

    //0 - disable 1 - enable
    U0LCR_bit.PE = parity_enable;

    // �������� odd, even, 1, 0  = 0 1 2 3
    U0LCR_bit.PS = parity_select;

    U0FCR_bit.RFR = 1;//reset rx fifo
    U0FCR_bit.TFR = 1;//reset tx fifo
    U0FCR_bit.FCRFE = 1;//fifo enable
    U0FCR_bit.RTLS = 3;//trigger level 14 bytes

    U0IER_bit.RDAIE = 1;//enable RDA interrupt
    U0IER_bit.THREIE = 1;//enable THRE interrupt
}
/*
**===========================================================================
**  5.      INTERNAL FUNCTIONS (declared in Section 3.5)
**===========================================================================
*/


/*
**===========================================================================
** END OF FILE
**===========================================================================
*/


