/***************************************************************************
 **
 **    This file defines the Special Function Registers for
 **    Philips LPC2138
 **
 **    Used with ARM IAR C/C++ Compiler and Assembler.
 **
 **    (c) Copyright IAR Systems 2004
 **
 **    $Revision: 1.8 $
 **
 **    Note: Only little endian addressing of 8 bit registers.
 ***************************************************************************/

#ifndef __FIOLPC2138_H
#define __FIOLPC2138_H



#include "io_macros.h"

/***************************************************************************
 ***************************************************************************
 **
 **    LPC2138 SPECIAL FUNCTION REGISTERS
 **
 ***************************************************************************
 ***************************************************************************
 ***************************************************************************/

/* C-compiler specific declarations  ***************************************/

#ifdef __IAR_SYSTEMS_ICC__

#ifndef _SYSTEM_BUILD
  #pragma system_include
#endif



/* GPIO registers */
typedef struct{
__REG32 P0_0   : 1;
__REG32 P0_1   : 1;
__REG32 P0_2   : 1;
__REG32 P0_3   : 1;
__REG32 P0_4   : 1;
__REG32 P0_5   : 1;
__REG32 P0_6   : 1;
__REG32 P0_7   : 1;
__REG32 P0_8   : 1;
__REG32 P0_9   : 1;
__REG32 P0_10  : 1;
__REG32 P0_11  : 1;
__REG32 P0_12  : 1;
__REG32 P0_13  : 1;
__REG32 P0_14  : 1;
__REG32 P0_15  : 1;
__REG32 P0_16  : 1;
__REG32 P0_17  : 1;
__REG32 P0_18  : 1;
__REG32 P0_19  : 1;
__REG32 P0_20  : 1;
__REG32 P0_21  : 1;
__REG32 P0_22  : 1;
__REG32 P0_23  : 1;
__REG32 P0_24  : 1;
__REG32 P0_25  : 1;
__REG32 P0_26  : 1;
__REG32 P0_27  : 1;
__REG32 P0_28  : 1;
__REG32 P0_29  : 1;
__REG32 P0_30  : 1;
__REG32 P0_31  : 1;
} __gpfio_bits;

/* GPFIO registers */
typedef struct{
__REG8 P0_0   : 1;
__REG8 P0_1   : 1;
__REG8 P0_2   : 1;
__REG8 P0_3   : 1;
__REG8 P0_4   : 1;
__REG8 P0_5   : 1;
__REG8 P0_6   : 1;
__REG8 P0_7   : 1;
} __gpfio_bits0;

/* GPFIO registers */
typedef struct{
__REG8 P0_8   : 1;
__REG8 P0_9   : 1;
__REG8 P0_10  : 1;
__REG8 P0_11  : 1;
__REG8 P0_12  : 1;
__REG8 P0_13  : 1;
__REG8 P0_14  : 1;
__REG8 P0_15  : 1;
} __gpfio_bits1;

/* GPFIO registers */
typedef struct{
__REG8 P0_16  : 1;
__REG8 P0_17  : 1;
__REG8 P0_18  : 1;
__REG8 P0_19  : 1;
__REG8 P0_20  : 1;
__REG8 P0_21  : 1;
__REG8 P0_22  : 1;
__REG8 P0_23  : 1;
__REG8 P0_24  : 1;
} __gpfio_bits2;

/* GPFIO registers */
typedef struct{
__REG8 P0_25  : 1;
__REG8 P0_26  : 1;
__REG8 P0_27  : 1;
__REG8 P0_28  : 1;
__REG8 P0_29  : 1;
__REG8 P0_30  : 1;
__REG8 P0_31  : 1;
} __gpfio_bits3;

/* GPFIO registers */
typedef struct{
__REG16 P0_0   : 1;
__REG16 P0_1   : 1;
__REG16 P0_2   : 1;
__REG16 P0_3   : 1;
__REG16 P0_4   : 1;
__REG16 P0_5   : 1;
__REG16 P0_6   : 1;
__REG16 P0_7   : 1;
__REG16 P0_8   : 1;
__REG16 P0_9   : 1;
__REG16 P0_10  : 1;
__REG16 P0_11  : 1;
__REG16 P0_12  : 1;
__REG16 P0_13  : 1;
__REG16 P0_14  : 1;
__REG16 P0_15  : 1;
} __gpfio_bits01;

/* GPFIO registers */
typedef struct{
__REG16 P0_16  : 1;
__REG16 P0_17  : 1;
__REG16 P0_18  : 1;
__REG16 P0_19  : 1;
__REG16 P0_20  : 1;
__REG16 P0_21  : 1;
__REG16 P0_22  : 1;
__REG16 P0_23  : 1;
__REG16 P0_24  : 1;
__REG16 P0_25  : 1;
__REG16 P0_26  : 1;
__REG16 P0_27  : 1;
__REG16 P0_28  : 1;
__REG16 P0_29  : 1;
__REG16 P0_30  : 1;
__REG16 P0_31  : 1;
} __gpfio_bits23;

/* SCS registers */
typedef struct{
__REG32 GPIO0M   : 1;
__REG32 GPIO1M   : 1;
__REG32          : 30;
} __scs_bits;

#endif    /* __IAR_SYSTEMS_ICC__ */

/***************************************************************************
 **
 ** GPFIO
 **
 ***************************************************************************/
__IO_REG32_BIT(SCS,              0xE01FC1A0,__READ_WRITE,__gpfio_bits);

__IO_REG32_BIT(FIO0DIR,          0x3FFFC000,__READ_WRITE,__gpfio_bits);
__IO_REG8_BIT(FIO0DIR0,          0x3FFFC000,__READ_WRITE,__gpfio_bits0);
__IO_REG8_BIT(FIO0DIR1,          0x3FFFC001,__READ_WRITE,__gpfio_bits1);
__IO_REG8_BIT(FIO0DIR2,          0x3FFFC002,__READ_WRITE,__gpfio_bits2);
__IO_REG8_BIT(FIO0DIR3,          0x3FFFC003,__READ_WRITE,__gpfio_bits3);
__IO_REG16_BIT(FIO0DIRL,         0x3FFFC000,__READ_WRITE,__gpfio_bits01);
__IO_REG16_BIT(FIO0DIRU,         0x3FFFC002,__READ_WRITE,__gpfio_bits23);
__IO_REG32_BIT(FIO0MASK,         0x3FFFC010,__READ_WRITE,__gpfio_bits);
__IO_REG8_BIT(FIO0MASK0,          0x3FFFC010,__READ_WRITE,__gpfio_bits0);
__IO_REG8_BIT(FIO0MASK1,          0x3FFFC011,__READ_WRITE,__gpfio_bits1);
__IO_REG8_BIT(FIO0MASK2,          0x3FFFC012,__READ_WRITE,__gpfio_bits2);
__IO_REG8_BIT(FIO0MASK3,          0x3FFFC013,__READ_WRITE,__gpfio_bits3);
__IO_REG16_BIT(FIO0MASKL,         0x3FFFC010,__READ_WRITE,__gpfio_bits01);
__IO_REG16_BIT(FIO0MASKU,         0x3FFFC012,__READ_WRITE,__gpfio_bits23);
__IO_REG32_BIT(FIO0PIN,          0x3FFFC014,__READ      ,__gpfio_bits);
__IO_REG8_BIT(FIO0PIN0,          0x3FFFC014,__READ_WRITE,__gpfio_bits0);
__IO_REG8_BIT(FIO0PIN1,          0x3FFFC015,__READ_WRITE,__gpfio_bits1);
__IO_REG8_BIT(FIO0PIN2,          0x3FFFC016,__READ_WRITE,__gpfio_bits2);
__IO_REG8_BIT(FIO0PIN3,          0x3FFFC017,__READ_WRITE,__gpfio_bits3);
__IO_REG16_BIT(FIO0PINL,         0x3FFFC014,__READ_WRITE,__gpfio_bits01);
__IO_REG16_BIT(FIO0PINU,         0x3FFFC016,__READ_WRITE,__gpfio_bits23);
__IO_REG32_BIT(FIO0SET,          0x3FFFC018,__READ_WRITE,__gpfio_bits);
__IO_REG8_BIT(FIO0SET0,          0x3FFFC018,__READ_WRITE,__gpfio_bits0);
__IO_REG8_BIT(FIO0SET1,          0x3FFFC019,__READ_WRITE,__gpfio_bits1);
__IO_REG8_BIT(FIO0SET2,          0x3FFFC01A,__READ_WRITE,__gpfio_bits2);
__IO_REG8_BIT(FIO0SET3,          0x3FFFC01B,__READ_WRITE,__gpfio_bits3);
__IO_REG16_BIT(FIO0SETL,         0x3FFFC018,__READ_WRITE,__gpfio_bits01);
__IO_REG16_BIT(FIO0SETU,         0x3FFFC01A,__READ_WRITE,__gpfio_bits23);
__IO_REG32_BIT(FIO0CLR,          0x3FFFC01C,__WRITE     ,__gpfio_bits);
__IO_REG8_BIT(FIO0CLR0,          0x3FFFC01C,__READ_WRITE,__gpfio_bits0);
__IO_REG8_BIT(FIO0CLR1,          0x3FFFC01D,__READ_WRITE,__gpfio_bits1);
__IO_REG8_BIT(FIO0CLR2,          0x3FFFC01E,__READ_WRITE,__gpfio_bits2);
__IO_REG8_BIT(FIO0CLR3,          0x3FFFC01F,__READ_WRITE,__gpfio_bits3);
__IO_REG16_BIT(FIO0CLRL,         0x3FFFC01C,__READ_WRITE,__gpfio_bits01);
__IO_REG16_BIT(FIO0CLRU,         0x3FFFC01E,__READ_WRITE,__gpfio_bits23);

__IO_REG32_BIT(FIO1DIR,          0x3FFFC020,__READ_WRITE,__gpfio_bits);
__IO_REG8_BIT(FIO1DIR0,          0x3FFFC020,__READ_WRITE,__gpfio_bits0);
__IO_REG8_BIT(FIO1DIR1,          0x3FFFC021,__READ_WRITE,__gpfio_bits1);
__IO_REG8_BIT(FIO1DIR2,          0x3FFFC022,__READ_WRITE,__gpfio_bits2);
__IO_REG8_BIT(FIO1DIR3,          0x3FFFC023,__READ_WRITE,__gpfio_bits3);
__IO_REG16_BIT(FIO1DIRL,         0x3FFFC020,__READ_WRITE,__gpfio_bits01);
__IO_REG16_BIT(FIO1DIRU,         0x3FFFC022,__READ_WRITE,__gpfio_bits23);
__IO_REG32_BIT(FIO1MASK,         0x3FFFC030,__READ_WRITE,__gpfio_bits);
__IO_REG8_BIT(FIO1MASK0,          0x3FFFC030,__READ_WRITE,__gpfio_bits0);
__IO_REG8_BIT(FIO1MASK1,          0x3FFFC031,__READ_WRITE,__gpfio_bits1);
__IO_REG8_BIT(FIO1MASK2,          0x3FFFC032,__READ_WRITE,__gpfio_bits2);
__IO_REG8_BIT(FIO1MASK3,          0x3FFFC033,__READ_WRITE,__gpfio_bits3);
__IO_REG16_BIT(FIO1MASKL,         0x3FFFC030,__READ_WRITE,__gpfio_bits01);
__IO_REG16_BIT(FIO1MASKU,         0x3FFFC032,__READ_WRITE,__gpfio_bits23);
__IO_REG32_BIT(FIO1PIN,          0x3FFFC034,__READ      ,__gpfio_bits);
__IO_REG8_BIT(FIO1PIN0,          0x3FFFC034,__READ_WRITE,__gpfio_bits0);
__IO_REG8_BIT(FIO1PIN1,          0x3FFFC035,__READ_WRITE,__gpfio_bits1);
__IO_REG8_BIT(FIO1PIN2,          0x3FFFC036,__READ_WRITE,__gpfio_bits2);
__IO_REG8_BIT(FIO1PIN3,          0x3FFFC037,__READ_WRITE,__gpfio_bits3);
__IO_REG16_BIT(FIO1PINL,         0x3FFFC034,__READ_WRITE,__gpfio_bits01);
__IO_REG16_BIT(FIO1PINU,         0x3FFFC036,__READ_WRITE,__gpfio_bits23);
__IO_REG32_BIT(FIO1SET,          0x3FFFC038,__READ_WRITE,__gpfio_bits);
__IO_REG8_BIT(FIO1SET0,          0x3FFFC038,__READ_WRITE,__gpfio_bits0);
__IO_REG8_BIT(FIO1SET1,          0x3FFFC039,__READ_WRITE,__gpfio_bits1);
__IO_REG8_BIT(FIO1SET2,          0x3FFFC03A,__READ_WRITE,__gpfio_bits2);
__IO_REG8_BIT(FIO1SET3,          0x3FFFC03B,__READ_WRITE,__gpfio_bits3);
__IO_REG16_BIT(FIO1SETL,         0x3FFFC038,__READ_WRITE,__gpfio_bits01);
__IO_REG16_BIT(FIO1SETU,         0x3FFFC03A,__READ_WRITE,__gpfio_bits23);
__IO_REG32_BIT(FIO1CLR,          0x3FFFC03C,__WRITE     ,__gpfio_bits);
__IO_REG8_BIT(FIO1CLR0,          0x3FFFC03C,__READ_WRITE,__gpfio_bits0);
__IO_REG8_BIT(FIO1CLR1,          0x3FFFC03D,__READ_WRITE,__gpfio_bits1);
__IO_REG8_BIT(FIO1CLR2,          0x3FFFC03E,__READ_WRITE,__gpfio_bits2);
__IO_REG8_BIT(FIO1CLR3,          0x3FFFC03F,__READ_WRITE,__gpfio_bits3);
__IO_REG16_BIT(FIO1CLRL,         0x3FFFC03C,__READ_WRITE,__gpfio_bits01);
__IO_REG16_BIT(FIO1CLRU,         0x3FFFC03E,__READ_WRITE,__gpfio_bits23);

#endif    /* __FIOLPC2138_H */
