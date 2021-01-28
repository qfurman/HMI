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
#include "ma_spi.h"     /* Target specific header file */
#include "ma_sfr.h"     /* Special function register bitfield macros */
#include "iolpc2138.h"  /* Defines Special function registers */
//#include "fiolpc2138.h"  /* Defines Special function registers */
#include "touch.h"
#include "lcd.h"
#include "menu.hpp"
#include "main.hpp"

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

LIST *spi0_wr_ptr; //global pointer for outgoing thread
LIST *spi0_rd_ptr;  //global pointer for ingoing thread

union{
    char spi0_status;
    RXTX spi0_sts_bit;
};

LIST spi0_wr_manag;
LIST spi0_rd_manag;

char check_sum_tx, check_sum_rx;
bool check_sum;//контрольна сума
char spi_device = 3;//три плати на spi почати з кінця щоб задовільняло умові таймера для ненаступання собі на хвіст

/*
**===========================================================================
**      Function for  SPI
**  addr - adress 0..32767
**  *src - sorce pointer for data in internal ram
**  count - count of byte for wtite
**===========================================================================
*/

//static unsigned int spi0_addr;
/*
void spi0(char device, void *src, void *dest, unsigned int count)
{
    spi0_status = 0x00;                         //стан початок прийому і передачі start_tx = 1 start_rx = 1;

    spi_mng.src = (char*)src;
    spi_mng.dest = (char*)dest;
    spi_mng.device = device;

    spi0_sts_bit.start_tx = 1;
    spi0_sts_bit.start_rx = 1;

    spi_mng.count = count;
}
*/
/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
unsigned int T0,T1,X,Y,Z1,Z2;
signed short temperature[32];//буфер для температури - різниця між Т1 і Т0
char t_index;//індекс щоб рухатичь по буферуж

const SPI_TOUCH_TX spi_touch_tx={0xb3,0x00,0xc3,0x00,0x87,0x00,0xf7,0x00,0xd3,0x00,0x93,0x00,0x00};//Z1,Z2,T0,T1,X,Y
SPI_TOUCH_RX spi_touch_rx;

unsigned int skey,sbkey;//отримані від клавіатури кнопка і всі кнопки побітово 
#pragma pack(1)
const unsigned char spi_key_tx[]={0x01,0x02,0x03,0x04,0x0a};//останній сума всіх
unsigned char spi_key_rx[16]; 
#pragma pack()
int TKsel;//переключення клавіатурою і сенсором

/*
**===========================================================================
**      Function for  SPI
**  *src - sorce pointer for data in internal ram
**  count - count of byte for wtite
**===========================================================================
*/

void spi0(char device, void *src, void *dest, unsigned int count)
{
    switch(device){
      case 0: FIO0CLR_bit.P0_7 = 1;break;//ssel0 = 0
      case 1: FIO0CLR_bit.P0_21 = 1;break;//CS00 = 0
      case 2: FIO0CLR_bit.P0_22 = 1;break;//CS01 = 0
      case 3: FIO0CLR_bit.P0_28 = 1;break;//CS02 = 0
      case 4: FIO0CLR_bit.P0_29 = 1;break;//CS03 = 0
    }
    spi0_status = 0x00;                         //стан початок прийому і передачі start_tx = 1 start_rx = 1;

    spi0_wr_manag.ptr = (char*)src;          //команда запису в масиву в пам’ять
    spi0_wr_manag.count = count;
    spi0_wr_manag.next = 0;                  //вказує на наступний елемент який потрібно предати по spi

    spi0_rd_manag.ptr = (char*)dest;
    spi0_rd_manag.count = count;
    spi0_rd_manag.next = 0;

    spi0_wr_ptr = &spi0_wr_manag;           //ініціалізація поінтера запису
    spi0_rd_ptr = &spi0_rd_manag;           //ініціалізація поінтера читання

    S0SPINT = 1;                               //clear int flag
    spi0_sts_bit.start_tx = 1;
    spi0_sts_bit.start_rx = 1;

    S0SPDR =  *spi0_wr_ptr->ptr;                //write first byte
    if(spi0_wr_ptr->ptr)*spi0_wr_ptr->ptr++;    //і якщо є дані на передачу то інкремент поінтера
    spi0_wr_ptr->count--;                       //декремент кількості байт на передачу

}

/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/
#define err_lim 0

int last_temperature;
void spi_prtcl()
{
  switch(TKsel){
    case 0:
    //відправка для тачпанелі
    spi0(0, (char*)&spi_touch_tx, &spi_touch_rx, sizeof(SPI_TOUCH_TX));
    TKsel++;//перейти
    break;
    
    case 1:
    if(spi0_sts_bit.end_rx){    
      //опитування тачпанелі
      /*if(N.touch){
      Z1 = 0x0fff & ((unsigned int)spi_touch_rx.hi0<<8 | spi_touch_rx.lo0)>>3;
      Z2 = 0x0fff & ((unsigned int)spi_touch_rx.hi1<<8 | spi_touch_rx.lo1)>>3;
      X = c_xy.x = 0x0fff & ((unsigned int)spi_touch_rx.hi4<<8 | spi_touch_rx.lo4)>>3;
      Y = c_xy.y = 0x0fff & ((unsigned int)spi_touch_rx.hi5<<8 | spi_touch_rx.lo5)>>3;
    }*/
      T0 = 0x0fff & ((unsigned int)spi_touch_rx.hi2<<8 | spi_touch_rx.lo2)>>3;
      T1 = 0x0fff & ((unsigned int)spi_touch_rx.hi3<<8 | spi_touch_rx.lo3)>>3;
      
      t_index = t_index & 0x1f;//обмежити розміром буфера
      if((last_temperature & 0xfffe) ==  ((T1 - T0)&0xfffe))temperature[t_index++] = T1 - T0;//не більше ніж на 1 біта
      last_temperature = T1 - T0;    
      TKsel++;//перейти 
    }
    break;
    
    case 2:
    //відправка для клавіатури
    spi0(1, (char*)&spi_key_tx, (char*)&spi_key_rx, 5);    
    TKsel++;//перейти 
    break;
    
    case 3:
    if(spi0_sts_bit.end_rx){
      //розпаковка клавіатурм
      if(spi_key_rx[4] == spi_key_rx[1] + spi_key_rx[2] + spi_key_rx[3]){
        skey = spi_key_rx[1];
        sbkey = spi_key_rx[2]<<8 | spi_key_rx[3];
      }
      TKsel = 0;
    }
    break;
  }
  
}

/*****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************/


//ці функції використовувались з апаратним SPI
/*
**===========================================================================
**      Interrupt Handler Function for write SPI memory
**===========================================================================
*/
void MA_IntHandler_SPI(void)
{
    volatile char dummy;
    volatile short cntr;

    do{
        if(S0SPSR_bit.SPIF){                      //якщо у буфері прийому є дані то виймати їх
            if(spi0_rd_ptr->count){               //якщо є куди виймати
                *spi0_rd_ptr->ptr++ = S0SPDR;    //розміститиза ним отриманий байт
                if(--spi0_rd_ptr->count == 0){      //декремент кількості байт прийнятих за цим поінтером якщо дорівнює 0 то спробувати перейти далі за списком
                    FIO0SET = 1<<7 | 1<<21 | 1<<22 | 1<<28 | 1<<29; //ssel0 = 1 CS00 = 1 CS01 = 1CS02 = 1 CS03 = 1
                    spi0_sts_bit.end_rx = 1;    //ознака кінця прийому
                    //T1TCR_bit.CE = 1; //опитано вже всі девайси запуск таймера
                }
            }
            else dummy = S0SPDR; //виймати непотрібний байт
        }

        //Передача
        if(spi0_wr_ptr->count){                             //якщо передано ще не всі дані за цим поінтрером
            if(MA_PutChar_SPI0(*spi0_wr_ptr->ptr) == MA_OK){ //кинути байт на передачу
                if(spi0_wr_ptr->ptr)*spi0_wr_ptr->ptr++;    //і якщо є дані на передачу то інкремент поінтера
                spi0_wr_ptr->count--;                       //декремент кількості байт на передачу
            }
            break; //предали байт і досить бо фіфо немає
        }
        else{
            spi0_sts_bit.end_tx = 1;            //ознака кінця передачі
            break;
        }
    }while(1);

    S0SPINT = 1;                               //clear int flag
}
/*
void MA_IntHandler_SPI(void)
{
    volatile char dummy;

    do{
        if(S0SPSR_bit.SPIF){                      //якщо у буфері прийому є дані то виймати їх
            if(spi0_rd_ptr->count){                 //якщо є куди виймати
                if(spi0_rd_ptr->ptr){               //якщо треба зберегти а не просто спорожнити
                    *spi0_rd_ptr->ptr++ = S0SPDR;    //якщо поінтер не нульовий то розміститиза ним отриманий байт
                }
                else dummy = S0SPDR;                 //виймати непотрібний байт

                if(--spi0_rd_ptr->count == 0){      //декремент кількості байт прийнятих за цим поінтером якщо дорівнює 0 то спробувати перейти далі за списком
                    if(spi0_rd_ptr->next) spi0_rd_ptr = spi0_rd_ptr->next;//перевірка чи є наступне місце для прийому і запис наступного елементу списку в активну позицію передачі
                    else{
                        spi0_sts_bit.end_rx = 1;    //ознака кінця прийому
                        FIO0SET_bit.P0_7 = 1;       //ssel0 = 1
                        FIO0SET_bit.P0_21 = 1;      //CS00 = 1
                        FIO0SET_bit.P0_22 = 1;      //CS01 = 1
                        FIO0SET_bit.P0_28 = 1;      //CS02 = 1
                        FIO0SET_bit.P0_29 = 1;      //CS03 = 1
                    }
                }
            }
            else dummy = S0SPDR; //виймати непотрібний байт
        }

        //Передача
        if(spi0_wr_ptr->count){                             //якщо передано ще не всі дані за цим поінтрером
            if(MA_PutChar_SPI0(*spi0_wr_ptr->ptr) == MA_OK){ //кинути байт на передачу
                if(spi0_wr_ptr->ptr)*spi0_wr_ptr->ptr++;    //і якщо є дані на передачу то інкремент поінтера
                spi0_wr_ptr->count--;                       //декремент кількості байт на передачу
            }
            break; //предали байт і досить бо фіфо немає
        }
        else{
            if(spi0_wr_ptr->next)                   //перевірка чи є наступний блок даних на передачу
                spi0_wr_ptr = spi0_wr_ptr->next;    //запис наступного елементу списку в активну позицію передачі
            else{
                spi0_sts_bit.end_tx = 1;            //ознака кінця передачі
                break;
            }
        }
    }while(1);

    S0SPINT = 1;                               //clear int flag
}
*/

/*
**===========================================================================
** END OF FILE
**===========================================================================
*/


