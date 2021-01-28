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
**  1.3     Re-definition guard
**===========================================================================
*/

/*--- Avoid including this file more than once ---*/
#ifndef _IS_INCLUDED_UART_0_H
#define _IS_INCLUDED_UART_0_H

/* usercode/template redefinition guard */
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

/*
**===========================================================================
**  3.      DECLARATIONS
**  3.1     Global constants
**===========================================================================
*/
extern unsigned short const CRC_table[];
/*
**===========================================================================
**  3.2     Global macros
**===========================================================================
*/






/*
**===========================================================================
**  3.3     Global type definitions
**===========================================================================
*/

/*
**===========================================================================
**  3.4     Global variables (defined in some implementation file)
**===========================================================================
*/
extern unsigned short CRCtx0,CRCrx0;

extern LIST *uart0_wr_ptr; //global pointer for outgoing thread
extern LIST *uart0_rd_ptr;  //global pointer for ingoing thread

extern union{
    char uart0_status;
    RXTX uart0_sts_bit;
};

extern LIST uart0_wr_manag[3];
extern LIST uart0_rd_manag[2];

extern char crc_buf0[2];
extern LIST uart0_wr_crc;  //для відправки контрольної суми

/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (defined in some implementation file)
**===========================================================================
*/

extern void MA_IntHandler_UART0( void );
extern void send_command(char number,char instruction,unsigned int data);
extern void uart0_setup(int bitrate,char word_length,char stop_bit,char parity_enable, char parity_select);

extern void uart0_bitrate(int bitrate);
extern void uart0_word_length(char temp);
extern void uart0_stop_bit(char temp);
extern void uart0_parity_enable(char temp);
extern void uart0_parity_select(char temp);

void wr_uart0(char *src, unsigned int src_count, char *dest, unsigned int dest_count);

#endif /* _IS_INCLUDED_UART_0_H Match the re-definition guard */

/*
**===========================================================================
** END OF FILE
**===========================================================================
*/


