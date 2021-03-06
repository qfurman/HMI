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
//#include "fiolpc2138.h"  /* Defines Special function registers */

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
//FM25L16-G
#define WREN 0x06
#define WRDI 0x04
#define RDSR 0x05
#define WRSR 0x01
#define READ 0x03
#define WRITE 0x02

//M25PE40
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
const char UnProtect[]={WREN,WRSR,0x00};    //none
//const char JProtect[]={WREN,WRSR,0x04};     //0x6000 to 0x7fff
//const char SProtect[]={WREN,WRSR,0x08};     //0x4000 to 0x7fff
//const char AllProtect[]={WREN,WRSR,0x0c};   //0x0000 to 0x7fff
const char Write_arr[]={WRITE};             //WRITE COMMAND, BODY MUST FALOWING AFTER
const char Read_SR[]={RDSR};                //read statatus register
const char Read_arr[]={READ};               //READ MEMORY ADDRESS MUST FOLOWING AFTER

const char WR_enable = WREN;                //WRITE ENABLE LATCH
const char page_write = PW;                 //

const char WRStatusR[] = {WRSR,0};           //����� ������ �� ������

LIST *spi1_wr_ptr; //global pointer for outgoing thread
LIST *spi1_rd_ptr;  //global pointer for ingoing thread

union{
    char spi1_status;
    RXTX spi1_sts_bit;
};

LIST spi1_wr_manag[3];
LIST spi1_rd_manag[2];

/*
**===========================================================================
**      Function for write SPI Ramtron memory
**  addr - adress 0..32767
**  *src - sorce pointer for data in internal ram
**  count - count of byte for wtite
**===========================================================================
*/

static unsigned int spi1_addr;

void wr_rmtrn(unsigned int addr, void *src, unsigned int count)
{
    spi1_addr = 0;
    spi1_addr = addr<<8 | (char)(addr>>8);
    spi1_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi1_wr_manag[0].ptr = (char*)&Write_arr;   //������� ������ � ������ � ������
    spi1_wr_manag[0].count = sizeof(Write_arr); //����� �������
    spi1_wr_manag[0].next = &spi1_wr_manag[1];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_wr_manag[1].ptr = (char*)&spi1_addr;   //����� ���� 2 ����� � ������
    spi1_wr_manag[1].count = 2;                 //��� �����
    spi1_wr_manag[1].next = &spi1_wr_manag[2];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_wr_manag[2].ptr = (char*)src;          //�������
    spi1_wr_manag[2].count = count;             //������� �����
    spi1_wr_manag[2].next = 0;                  //����� ����

    spi1_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi1_rd_manag[0].count = sizeof(Write_arr)+2+count;//1����-�������, 2�����-������
    spi1_rd_manag[0].next = 0;                  //�����������

    spi1_wr_ptr =  &spi1_wr_manag[0];           //������������ ������� ������
    spi1_rd_ptr =  &spi1_rd_manag[0];           //������������ ������� �������

    FIO0CLR_bit.P0_20 = 1;                       //ssel1 = 0
    spi1_sts_bit.start_tx = 1;
    spi1_sts_bit.start_rx = 1;
    SSPIMSC =  0x0e;                            //enable TXIM RXIM RTIM
}
/*
**===========================================================================
**      Function for write SPI ST memory
**  addr - adress 0..524288
- ����� ��������� � 8 ���� - ����� 4 ��������� �� 1 �����
**  *src - sorce pointer for data in internal ram
**  count - count of byte for wtite
**===========================================================================
*/

void spi1_address(unsigned int addr)
{
    spi1_addr = 0;
    spi1_addr = (addr&0xff)<<16 | addr&0x00ff00 | (char)(addr>>16) | (addr & 0x03000000);//������ �� ����� ��� 3-� ����� ������� ��������
}


void wr_flash(unsigned int addr, void *src, unsigned int count)
{
    spi1_address(addr);
    spi1_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi1_wr_manag[0].ptr = (char*)&page_write;   //������� ������ � ������ � ������
    spi1_wr_manag[0].count = sizeof(page_write); //����� �������
    spi1_wr_manag[0].next = &spi1_wr_manag[1];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_wr_manag[1].ptr = (char*)&spi1_addr;   //����� ���� 3 ����� � ������
    spi1_wr_manag[1].count = 3;                 //3 �����
    spi1_wr_manag[1].next = &spi1_wr_manag[2];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_wr_manag[2].ptr = (char*)src;          //�������
    spi1_wr_manag[2].count = count;             //������� �����
    spi1_wr_manag[2].next = 0;                  //����� ����

    spi1_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi1_rd_manag[0].count = sizeof(Write_arr)+3+count;//1����-�������, 3�����-������
    spi1_rd_manag[0].next = 0;                  //�����������

    spi1_wr_ptr =  &spi1_wr_manag[0];           //������������ ������� ������
    spi1_rd_ptr =  &spi1_rd_manag[0];           //������������ ������� �������

    //�������� ��������� �� ������ ���������
    switch((spi1_addr >> 4)&3){
      case 0: FIO0CLR_bit.P0_23 = 1;break;                       //spi_flash = 0
      case 1: FIO0CLR_bit.P0_12 = 1;break;                       //spi_flash = 0
      case 2: FIO0CLR_bit.P0_11 = 1;break;                       //spi_flash = 0
      case 3: FIO0CLR_bit.P0_15 = 1;break;                       //spi_flash = 0
    }

    spi1_sts_bit.start_tx = 1;
    spi1_sts_bit.start_rx = 1;
    SSPIMSC =  0x0e;                            //enable TXIM RXIM RTIM
}


/*
**===========================================================================
**      Function for read SPI Ramtron memory
**  addr - adress 0..32767
**  *dest - destenation pointer for data in internal ram
**  count - count of byte for read
**===========================================================================
*/
void rd_rmtrn(unsigned int addr, void *dest, unsigned int count)
{
    spi1_addr = 0;
    spi1_addr = addr<<8 | (char)(addr>>8);
    spi1_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi1_wr_manag[0].ptr = (char*)&Read_arr;    //������� ������ � ������ � ������
    spi1_wr_manag[0].count = sizeof(Read_arr);  //����� �������
    spi1_wr_manag[0].next = &spi1_wr_manag[1];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_wr_manag[1].ptr = (char*)&spi1_addr;   //����� ���� 2 ����� � ������
    spi1_wr_manag[1].count = 2;                 //��� �����
    spi1_wr_manag[1].next = &spi1_wr_manag[2];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_wr_manag[2].ptr = 0;                   //���� ������� � ������
    spi1_wr_manag[2].count = count;             //������� ����� � ��
    spi1_wr_manag[2].next = 0;                  //��� ����

    spi1_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi1_rd_manag[0].count = 3;                 //1����-�������, 2�����-������
    spi1_rd_manag[0].next = &spi1_rd_manag[1];  //�����������

    spi1_rd_manag[1].ptr = (char*)dest;         //���� �����������
    spi1_rd_manag[1].count = count;             //�������
    spi1_rd_manag[1].next = 0;                  //�����������

    spi1_wr_ptr =  &spi1_wr_manag[0];           //������������ ������� ������
    spi1_rd_ptr =  &spi1_rd_manag[0];           //������������ ������� �������

    FIO0CLR_bit.P0_20 = 1;                       //ssel1 = 0
    spi1_sts_bit.start_tx = 1;
    spi1_sts_bit.start_rx = 1;
    SSPIMSC =  0x0e;                            //enable TXIM RXIM RTIM
}

/*
**===========================================================================
**      Function for read SPI ST memory
**  addr - adress 0..524288
**  *dest - destenation pointer for data in internal ram
**  count - count of byte for read
**===========================================================================
*/
void rd_flash(unsigned int addr, void *dest, unsigned int count)
{
    spi1_address(addr);
    spi1_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi1_wr_manag[0].ptr = (char*)&Read_arr;    //������� ������ � ������ � ������
    spi1_wr_manag[0].count = sizeof(Read_arr);  //����� �������
    spi1_wr_manag[0].next = &spi1_wr_manag[1];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_wr_manag[1].ptr = (char*)&spi1_addr;   //����� ���� 3 ����� � ������
    spi1_wr_manag[1].count = 3;                 //��� �����
    spi1_wr_manag[1].next = &spi1_wr_manag[2];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_wr_manag[2].ptr = 0;                   //���� ������� � ������
    spi1_wr_manag[2].count = count;             //������� ����� � ��
    spi1_wr_manag[2].next = 0;                  //��� ����

    spi1_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi1_rd_manag[0].count = 1+3;               //1����-�������, 3�����-������
    spi1_rd_manag[0].next = &spi1_rd_manag[1];  //�����������

    spi1_rd_manag[1].ptr = (char*)dest;         //���� �����������
    spi1_rd_manag[1].count = count;             //�������
    spi1_rd_manag[1].next = 0;                  //�����������

    spi1_wr_ptr =  &spi1_wr_manag[0];           //������������ ������� ������
    spi1_rd_ptr =  &spi1_rd_manag[0];           //������������ ������� �������

    //�������� ��������� �� ������ ���������
    switch((spi1_addr >> 4)&3){
      case 0: FIO0CLR_bit.P0_23 = 1;break;                       //spi_flash = 0
      case 1: FIO0CLR_bit.P0_12 = 1;break;                       //spi_flash = 0
      case 2: FIO0CLR_bit.P0_11 = 1;break;                       //spi_flash = 0
      case 3: FIO0CLR_bit.P0_15 = 1;break;                       //spi_flash = 0
    }

    spi1_sts_bit.start_tx = 1;
    spi1_sts_bit.start_rx = 1;
    SSPIMSC =  0x0e;                            //enable TXIM RXIM RTIM
}


/*
**===========================================================================
**      Initialisation Ramtron memory
**===========================================================================
*/

void spi1_init(void)
{
    spi1_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi1_wr_manag[0].ptr = (char*)&UnProtect;   //������� ������ � ������ ������� spi �����
    spi1_wr_manag[0].count = sizeof(UnProtect); //����� �������
    spi1_wr_manag[0].next = 0;                  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi1_rd_manag[0].count = sizeof(UnProtect); //����� �������
    spi1_rd_manag[0].next = 0;                  //���� �����������

    spi1_wr_ptr =  &spi1_wr_manag[0];           //������������ ������� ������
    spi1_rd_ptr =  &spi1_rd_manag[0];           //������������ ������� �������

    FIO0CLR_bit.P0_20 = 1;                       //ssel1 = 0
    spi1_sts_bit.start_tx = 1;
    spi1_sts_bit.start_rx = 1;
    SSPIMSC =  0x0e;                            //enable TXIM RXIM RTIM
}
/*
**===========================================================================
**      Initialisation ST memory
**===========================================================================
*/

void spi1_WEL(unsigned int addr)
{
    spi1_address(addr);
    spi1_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi1_wr_manag[0].ptr = (char*)&WR_enable;   //������� ������
    spi1_wr_manag[0].count = sizeof(WR_enable); //����� �������
    spi1_wr_manag[0].next = 0;                  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi1_rd_manag[0].count = sizeof(WR_enable); //����� �������
    spi1_rd_manag[0].next = 0;                  //���� �����������

    spi1_wr_ptr =  &spi1_wr_manag[0];           //������������ ������� ������
    spi1_rd_ptr =  &spi1_rd_manag[0];           //������������ ������� �������

    //�������� ��������� �� ������ ���������
    switch((spi1_addr >> 4)&3){
      case 0: FIO0CLR_bit.P0_23 = 1;break;                       //spi_flash = 0
      case 1: FIO0CLR_bit.P0_12 = 1;break;                       //spi_flash = 0
      case 2: FIO0CLR_bit.P0_11 = 1;break;                       //spi_flash = 0
      case 3: FIO0CLR_bit.P0_15 = 1;break;                       //spi_flash = 0
    }
    spi1_sts_bit.start_tx = 1;
    spi1_sts_bit.start_rx = 1;
    SSPIMSC =  0x0e;                            //enable TXIM RXIM RTIM
}
/*
**===========================================================================
**      Initialisation ST memory        ����� ���������� �� ������
**===========================================================================
*/

void spi1_WSR(unsigned int addr)
{
    spi1_address(addr);
    spi1_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi1_wr_manag[0].ptr = (char*)&WRStatusR;   //������� ������
    spi1_wr_manag[0].count = sizeof(WRStatusR); //����� �������
    spi1_wr_manag[0].next = 0;                  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi1_rd_manag[0].count = sizeof(WRStatusR); //����� �������
    spi1_rd_manag[0].next = 0;                  //���� �����������

    spi1_wr_ptr =  &spi1_wr_manag[0];           //������������ ������� ������
    spi1_rd_ptr =  &spi1_rd_manag[0];           //������������ ������� �������

    //�������� ��������� �� ������ ���������
    switch((spi1_addr >> 4)&3){
      case 0: FIO0CLR_bit.P0_23 = 1;break;                       //spi_flash = 0
      case 1: FIO0CLR_bit.P0_12 = 1;break;                       //spi_flash = 0
      case 2: FIO0CLR_bit.P0_11 = 1;break;                       //spi_flash = 0
      case 3: FIO0CLR_bit.P0_15 = 1;break;                       //spi_flash = 0
    }
    spi1_sts_bit.start_tx = 1;
    spi1_sts_bit.start_rx = 1;
    SSPIMSC =  0x0e;                            //enable TXIM RXIM RTIM
}/*
**===========================================================================
**      Read status register SPI memory  RAMTRON
**===========================================================================
*/
char spi1_status_rg;

void spi1_rd_status(void)
{
    spi1_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi1_wr_manag[0].ptr = (char*)&Read_SR;     //������� ������ � ������ � ������
    spi1_wr_manag[0].count = sizeof(Read_SR);   //����� �������
    spi1_wr_manag[0].next = &spi1_wr_manag[1];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_wr_manag[1].ptr = 0;                   //����� ���� ��� ���� ��� ���� ���� ���������
    spi1_wr_manag[1].count = 1;                 //1
    spi1_wr_manag[1].next = 0;                  //�����

    spi1_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi1_rd_manag[0].count = sizeof(Read_SR);   //����� �������
    spi1_rd_manag[0].next = &spi1_rd_manag[1];  //�����������

    spi1_rd_manag[1].ptr = &spi1_status_rg;     //���� �����������
    spi1_rd_manag[1].count = 1;                 //�������
    spi1_rd_manag[1].next = 0;                  //�����������

    spi1_wr_ptr =  &spi1_wr_manag[0];           //������������ ������� ������
    spi1_rd_ptr =  &spi1_rd_manag[0];           //������������ ������� �������

    FIO0CLR_bit.P0_20 = 1;                       //ssel1 = 0
    spi1_sts_bit.start_tx = 1;
    spi1_sts_bit.start_rx = 1;
    SSPIMSC =  0x0e;                            //enable TXIM RXIM RTIM
}
/*
**===========================================================================
**      Read status register SPI memory  ST
**===========================================================================
*/

void spi1_rd_status_flash(unsigned int addr)
{
    spi1_address(addr);
    spi1_status = 0x00;                         //���� ������� ������� � �������� start_tx = 1 start_rx = 1;

    spi1_wr_manag[0].ptr = (char*)&Read_SR;     //������� ������ � ������ � ������
    spi1_wr_manag[0].count = sizeof(Read_SR);   //����� �������
    spi1_wr_manag[0].next = &spi1_wr_manag[1];  //����� �� ��������� ������� ���� ������� ������� �� spi

    spi1_wr_manag[1].ptr = 0;                   //����� ���� ��� ���� ��� ���� ���� ���������
    spi1_wr_manag[1].count = 1;                 //1
    spi1_wr_manag[1].next = 0;                  //�����

    spi1_rd_manag[0].ptr = 0;                   //�� �������� �����
    spi1_rd_manag[0].count = sizeof(Read_SR);   //����� �������
    spi1_rd_manag[0].next = &spi1_rd_manag[1];  //�����������

    spi1_rd_manag[1].ptr = &spi1_status_rg;     //���� �����������
    spi1_rd_manag[1].count = 1;                 //�������
    spi1_rd_manag[1].next = 0;                  //�����������

    spi1_wr_ptr =  &spi1_wr_manag[0];           //������������ ������� ������
    spi1_rd_ptr =  &spi1_rd_manag[0];           //������������ ������� �������

    //�������� ��������� �� ������ ���������
    switch((spi1_addr >> 4)&3){
      case 0: FIO0CLR_bit.P0_23 = 1;break;                       //spi_flash = 0
      case 1: FIO0CLR_bit.P0_12 = 1;break;                       //spi_flash = 0
      case 2: FIO0CLR_bit.P0_11 = 1;break;                       //spi_flash = 0
      case 3: FIO0CLR_bit.P0_15 = 1;break;                       //spi_flash = 0
    }
    spi1_sts_bit.start_tx = 1;
    spi1_sts_bit.start_rx = 1;
    SSPIMSC =  0x0e;                            //enable TXIM RXIM RTIM
}

/*
**===========================================================================
**      Interrupt Handler Function for write SPI memory
**===========================================================================
*/

void MA_IntHandler_SSP(void)
{
    volatile char dummy;

    do{
        do{
            if(SSPSR & SSPSR_RNE){                      //���� � ����� ������� � ���� �� ������� ��
                if(spi1_rd_ptr->count){                 //���� � ���� �������
                    if(spi1_rd_ptr->ptr){               //���� ����� �������� � �� ������ ����������
                        *spi1_rd_ptr->ptr++ = SSPDR;    //���� ������ �� �������� �� ����������� ��� ��������� ����
                    }
                    else dummy = SSPDR;                 //������� ���������� ����

                    if(--spi1_rd_ptr->count == 0){      //��������� ������� ���� ��������� �� ��� �������� ���� ������� 0 �� ���������� ������� ��� �� �������
                        if(spi1_rd_ptr->next) spi1_rd_ptr = spi1_rd_ptr->next;//�������� �� � �������� ���� ��� ������� � ����� ���������� �������� ������ � ������� ������� ��������
                        else{
                            spi1_sts_bit.end_rx = 1;    //������ ���� �������
                            FIO0SET_bit.P0_20 = 1;       //ssel1 = 1
                            FIO0SET_bit.P0_23 = 1;       //spi_flash = 1
                            FIO0SET_bit.P0_12 = 1;
                            FIO0SET_bit.P0_11 = 1;
                            FIO0SET_bit.P0_15 = 1;
                        }
                    }
                }
                else dummy = SSPDR; //������� ���������� ���� ��� ����������� ����
            }
            else break;
        }while(1);

        //��������
        if(spi1_wr_ptr->count){                             //���� �������� �� �� �� ���� �� ��� ���������
            if(MA_PutChar_SSP(*spi1_wr_ptr->ptr) == MA_OK){ //���� FIFO �� �����������
                if(spi1_wr_ptr->ptr)*spi1_wr_ptr->ptr++;    //� ���� � ���� �� �������� �� ��������� �������
                spi1_wr_ptr->count--;                       //��������� ������� ���� �� ��������
            }
            else break; //��� ������������� ���� ��������� ������ ��� ���������� �����������


        }
        else{
            if(spi1_wr_ptr->next)                   //�������� �� � ��������� ���� ����� �� ��������
                spi1_wr_ptr = spi1_wr_ptr->next;    //����� ���������� �������� ������ � ������� ������� ��������
            else{
                SSPIMSC =  0x06;                    //������� ��������
                spi1_sts_bit.end_tx = 1;            //������ ���� ��������
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


