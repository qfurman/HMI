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

/*
**===========================================================================
**  2.2     Application include files
**===========================================================================
*/

#include "usercode.h"   /* Usercode macros (see <template.h>) */
#include "ma_tgt.h"     /* Target specific header file */
#include "ma_sfr.h"     /* Special function register bitfield macros */
#include "iolpc2138.h"  /* Defines Special function registers */

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



/*
**===========================================================================
**  3.2     Internal macros
**===========================================================================
*/

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

/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (declared as 'extern' in some header file)
**===========================================================================
*/

#define WREN 0x06
#define WRDI 0x04
#define RDID 0X9F
#define RDSR 0x05
#define READ 0x03
#define FAST_READ 0X0B
#define PW  0X0A
#define PP  0X02
#define PE  0xDB
#define SE  0XD8
#define DP  0XB9
#define RDP 0XAB
/*
**===========================================================================
**  5.      INTERNAL FUNCTIONS (declared in Section 3.5)
**===========================================================================
*/




/*
**===========================================================================
** HEADES OF SPI RAM COMMANDS
**===========================================================================
*/
/*
struct list{
    char *ptr;
    unsigned short count;
    struct list *next;
};
*/

extern const char UnProtect[3];//={WREN,WRSR,0x00};    //none
extern const char Write_arr[1];//={WRITE};             //WRITE COMMAND, BODY MUST FALOWING AFTER
extern const char Read_SR[1];//={RDSR};                //read statatus register
extern const char Read_arr[1];//={READ};               //READ MEMORY ADDRESS MUST FOLOWING AFTER

LIST *spi0_wr_ptr; //global pointer for outgoing thread
LIST *spi0_rd_ptr;  //global pointer for ingoing thread

union{
    char spi0_status;
    RXTX spi0_sts_bit;
};

/*
**===========================================================================
**      Function for write SPI Ramtron memory
**  addr - adress 0..32767
**  *src - sorce pointer for data in internal ram
**  count - count of byte for wtite
**===========================================================================
*/
/*
struct list{
    char *ptr;              //Operands pointer
    unsigned int count;     //count of bytes
    struct list *next;      //pointer for next element of list
};
*/
LIST spi0_wr_manag[3];
LIST spi0_rd_manag[2];

static unsigned int spi0_addr;

void wr_st(unsigned int addr, void *src, unsigned int count)
{
    spi0_addr = 0;
    spi0_addr = (addr&0xff)<<16 | addr&0x00ff00 | (char)(addr>>16);
    spi0_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi0_wr_manag[0].ptr = (char*)&Write_arr;   //������� ������ � ������ � ������
    spi0_wr_manag[0].count = sizeof(Write_arr); //����� �������
    spi0_wr_manag[0].next = &spi0_wr_manag[1];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi0_wr_manag[1].ptr = (char*)&spi0_addr;   //����� ���� 3 ����� � ������
    spi0_wr_manag[1].count = 3;                 //��� �����
    spi0_wr_manag[1].next = &spi0_wr_manag[2];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi0_wr_manag[2].ptr = (char*)src;          //�������
    spi0_wr_manag[2].count = count;             //������� �����
    spi0_wr_manag[2].next = 0;                  //����� ����

    spi0_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi0_rd_manag[0].count = sizeof(Write_arr)+3+count;//1����-�������, 2�����-������
    spi0_rd_manag[0].next = 0;                  //�����������

    spi0_wr_ptr =  &spi0_wr_manag[0];           //����������� ������� ������
    spi0_rd_ptr =  &spi0_rd_manag[0];           //����������� ������� �������

    IO0CLR_bit.P0_7 = 1;                       //ssel0 = 0
    spi0_sts_bit.start_tx = 1;
    spi0_sts_bit.start_rx = 1;
        //��������
        if(spi0_wr_ptr->count){                         //���� �������� �� �� �� ��� �� ��� ���������
            S0SPDR = *spi0_wr_ptr->ptr;
            if(spi0_wr_ptr->ptr)*spi0_wr_ptr->ptr++;    //� ���� � ��� �� �������� �� ��������� �������
            spi0_wr_ptr->count--;                       //��������� ������� ���� �� ��������
            //�������� ��������� ���� ��� ���������� ����������
        }
    S0SPCR_bit.SPIE = 1;                       //enable INTERRUPT
}

/*
**===========================================================================
**      Function for read SPI ST memory
**  addr - adress 0..32767
**  *dest - destenation pointer for data in internal ram
**  count - count of byte for read
**===========================================================================
*/
void rd_st(unsigned int addr, void *dest, unsigned int count)
{
    spi0_addr = 0;
    spi0_addr = addr<<8 | (char)addr>>8;
    spi0_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi0_wr_manag[0].ptr = (char*)&Read_arr;    //������� ������ � ������ � ������
    spi0_wr_manag[0].count = sizeof(Read_arr);  //����� �������
    spi0_wr_manag[0].next = &spi0_wr_manag[1];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi0_wr_manag[1].ptr = (char*)&spi0_addr;   //����� ���� 2 ����� � ������
    spi0_wr_manag[1].count = 2;                 //��� �����
    spi0_wr_manag[1].next = &spi0_wr_manag[2];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi0_wr_manag[2].ptr = 0;                   //���� ������� � ������
    spi0_wr_manag[2].count = count;             //������� ����� � ��
    spi0_wr_manag[2].next = 0;                  //��� ����

    spi0_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi0_rd_manag[0].count = 3;                 //1����-�������, 2�����-������
    spi0_rd_manag[0].next = &spi0_rd_manag[1];  //�����������

    spi0_rd_manag[1].ptr = (char*)dest;         //���� �����������
    spi0_rd_manag[1].count = count;             //�������
    spi0_rd_manag[1].next = 0;                  //�����������

    spi0_wr_ptr =  &spi0_wr_manag[0];           //����������� ������� ������
    spi0_rd_ptr =  &spi0_rd_manag[0];           //����������� ������� �������

    IO0CLR_bit.P0_7 = 1;                       //ssel0 = 0
    spi0_sts_bit.start_tx = 1;
    spi0_sts_bit.start_rx = 1;
        //��������
        if(spi0_wr_ptr->count){                         //���� �������� �� �� �� ��� �� ��� ���������
            S0SPDR = *spi0_wr_ptr->ptr;
            if(spi0_wr_ptr->ptr)*spi0_wr_ptr->ptr++;    //� ���� � ��� �� �������� �� ��������� �������
            spi0_wr_ptr->count--;                       //��������� ������� ���� �� ��������
            //�������� ��������� ���� ��� ���������� ����������
        }
    S0SPCR_bit.SPIE = 1;                       //enable INTERRUPT

}

/*
**===========================================================================
**      Initialisation ST memory
**===========================================================================
*/

void spi0_init(void)
{
    spi0_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi0_wr_manag[0].ptr = (char*)&UnProtect;   //������� ������ � ������ ������� spi �����
    spi0_wr_manag[0].count = sizeof(UnProtect); //����� �������
    spi0_wr_manag[0].next = 0;                  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi0_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi0_rd_manag[0].count = sizeof(UnProtect); //����� �������
    spi0_rd_manag[0].next = 0;                  //���� �����������

    spi0_wr_ptr =  &spi0_wr_manag[0];           //����������� ������� ������
    spi0_rd_ptr =  &spi0_rd_manag[0];           //����������� ������� �������

    IO0CLR_bit.P0_7 = 1;                       //ssel0 = 0
    spi0_sts_bit.start_tx = 1;
    spi0_sts_bit.start_rx = 1;
        //��������
        if(spi0_wr_ptr->count){                         //���� �������� �� �� �� ��� �� ��� ���������
            S0SPDR = *spi0_wr_ptr->ptr;
            if(spi0_wr_ptr->ptr)*spi0_wr_ptr->ptr++;    //� ���� � ��� �� �������� �� ��������� �������
            spi0_wr_ptr->count--;                       //��������� ������� ���� �� ��������
            //�������� ��������� ���� ��� ���������� ����������
        }
    S0SPCR_bit.SPIE = 1;                       //enable INTERRUPT
}

/*
**===========================================================================
**      Read status register SPI memory
**===========================================================================
*/
char spi0_status_rg;

void spi0_rd_status(void)
{
    spi0_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi0_wr_manag[0].ptr = (char*)&Read_SR;     //������� ������ � ������ � ������
    spi0_wr_manag[0].count = sizeof(Read_SR);   //����� �������
    spi0_wr_manag[0].next = &spi0_wr_manag[1];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi0_wr_manag[1].ptr = 0;                   //����� ���� ��� ���� ��� ���� ���� ���������
    spi0_wr_manag[1].count = 1;                 //1
    spi0_wr_manag[1].next = 0;                  //�����

    spi0_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi0_rd_manag[0].count = sizeof(Read_SR);   //����� �������
    spi0_rd_manag[0].next = &spi0_rd_manag[1];  //�����������

    spi0_rd_manag[1].ptr = &spi0_status_rg;     //���� �����������
    spi0_rd_manag[1].count = 1;                 //�������
    spi0_rd_manag[1].next = 0;                  //�����������

    spi0_wr_ptr =  &spi0_wr_manag[0];           //����������� ������� ������
    spi0_rd_ptr =  &spi0_rd_manag[0];           //����������� ������� �������

    IO0CLR_bit.P0_7 = 1;                       //ssel0 = 0
    spi0_sts_bit.start_tx = 1;
    spi0_sts_bit.start_rx = 1;
        //��������
        if(spi0_wr_ptr->count){                         //���� �������� �� �� �� ��� �� ��� ���������
            S0SPDR = *spi0_wr_ptr->ptr;
            if(spi0_wr_ptr->ptr)*spi0_wr_ptr->ptr++;    //� ���� � ��� �� �������� �� ��������� �������
            spi0_wr_ptr->count--;                       //��������� ������� ���� �� ��������
            //�������� ��������� ���� ��� ���������� ����������
        }
    S0SPCR_bit.SPIE = 1;                       //enable INTERRUPT
}

/*
**===========================================================================
**      Interrupt Handler Function for write SPI memory
**===========================================================================
*/

void MA_IntHandler_SPI(void)
{
    volatile char dummy;

    S0SPINT = 1;//CLEAR INTERRUPT

        if(S0SPSR_bit.SPIF){                        //���� � ����� ������� � ��� �� ������� ��
            if(spi0_rd_ptr->count){                 //���� � ���� �������
                if(spi0_rd_ptr->ptr){               //���� ����� �������� � �� ������ ����������
                    *spi0_rd_ptr->ptr++ = S0SPDR;   //���� ������ �� �������� �� ����������� ��� ��������� ����
                }
                else dummy = S0SPDR;                //������� ���������� ����

                if(--spi0_rd_ptr->count == 0){      //��������� ������� ���� ��������� �� ��� �������� ���� ������� 0 �� ���������� ������� ��� �� �������
                    if(spi0_rd_ptr->next) spi0_rd_ptr = spi0_rd_ptr->next;//�������� �� � �������� ���� ��� ������� � ����� ���������� �������� ������ � ������� ������� ��������
                    else{
                        spi0_sts_bit.end_rx = 1;    //������ ���� �������
                        S0SPCR_bit.SPIE = 0;        //DISABLE INTERRUPT
                        IO0SET_bit.P0_7 = 1;        //ssel0 = 1
                    }
                }
            }
            else dummy = S0SPDR; //������� ���������� ����
        }

        do{
            //��������
            if(spi0_wr_ptr->count){                         //���� �������� �� �� �� ��� �� ��� ���������
                S0SPDR = *spi0_wr_ptr->ptr;
                if(spi0_wr_ptr->ptr)*spi0_wr_ptr->ptr++;    //� ���� � ��� �� �������� �� ��������� �������
                spi0_wr_ptr->count--;                       //��������� ������� ���� �� ��������
                break;
                //�������� ��������� ���� ��� ���������� ����������
            }
            else{
                if(spi0_wr_ptr->next)                   //�������� �� � ��������� ���� ����� �� ��������
                    spi0_wr_ptr = spi0_wr_ptr->next;    //����� ���������� �������� ������ � ������� ������� ��������
                 else{
                    //������� ��������
                    spi0_sts_bit.end_tx = 1;            //������ ���� ��������
                    break;
                }
            }
        }while(1);
}


/*
**===========================================================================
** END OF FILE
**===========================================================================
*/


