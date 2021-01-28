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
#ifndef _IS_INCLUDED_ST_H
#define _IS_INCLUDED_ST_H

/* usercode/template redefinition guard */
/*
**===========================================================================
**  2.      INCLUDE FILES
**  2.1     Standard include files
**===========================================================================
*/

#include "type.h"


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

/*
**===========================================================================
**  3.2     Global macros
**===========================================================================
*/


/*--- User code for the SCB module ---*/



/*
**===========================================================================
**  3.3     Global type definitions
**===========================================================================
*/

extern union{
    char spi0_status;
    RXTX spi0_sts_bit;
};

extern char spi0_status_rg;

/*
**===========================================================================
**  3.4     Global variables (defined in some implementation file)
**===========================================================================
*/
extern struct list *rmtrn_out_ptr; //global pointer for outgoing thread
extern struct list *rmtrn_in_ptr; //global pointer for ingoing thread
/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (defined in some implementation file)
**===========================================================================
*/
extern void spi0_init(void);
extern void spi0_rd_status(void);

extern void wr_st(unsigned int addr, void *src, unsigned int count);
extern void rd_st(unsigned int addr, void *dest, unsigned int count);


#endif /* Match the re-definition guard */

/*
**===========================================================================
** END OF FILE
**===========================================================================
*/


