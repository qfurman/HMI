/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2003
 *
 *    File name   : LPC_Vic.h
 *    Description :
 *
 *    History :
 *    1. Date: July 8th, 2004
 *       Author: Wilson Liu
 *       Description: Create the basic function
 *
 *    2. Date: August 4th, 2004
 *       Author: Shawn Zhang
 *       Description: Clean up the functions. Support nonvector interrupt at first.
 *
 *    3. Date        : Oct 11, 2004
 *       Author      : Stanimir Bonev
 *       Description : Modify some function and interface
 *
 *    $Revision: 20579 $
 **************************************************************************/

#include "includes.h"

#ifndef __LPC_VIC_H
#define __LPC_VIC_H

#define IRQ_FLAG        0x80
#define FIQ_FLAG        0x40

// Interrupt protection type
typedef enum {
  UserandPrivilegedMode=0,
  PrivilegedMode
}LPC_Vic_ProtectionMode_t;

typedef enum {
  VIC_Slot0 = 0,  // high priority
  VIC_Slot1,VIC_Slot2,VIC_Slot3,VIC_Slot4,VIC_Slot5,VIC_Slot6,VIC_Slot7,VIC_Slot8,
  VIC_Slot9,VIC_Slot10,VIC_Slot11,VIC_Slot12,VIC_Slot13,VIC_Slot14,VIC_Slot15
}LPC_VicIrqSlots_t;

/* Declare API functions */
void VIC_SetProtectionMode(LPC_Vic_ProtectionMode_t ProtectionType);
LPC_Vic_ProtectionMode_t VIC_GetProtectionMode(void);

void VIC_Init(void);

void VIC_EnableInt(unsigned int IntType);
void VIC_DisableInt(unsigned int IntType);

unsigned int VIC_GetIRQStatus(void);
unsigned int VIC_GetFIQStatus(void);

void VIC_EnableNonVectoredIRQ(void(*pIRQSub)());
void VIC_DisableNonVectoredIRQ(void);

void VIC_SetVectoredIRQ(void(*pIRQSub)(), LPC_VicIrqSlots_t VicIrqSlot, unsigned int VicIntSouce);

void VIC_EnaFastInt(unsigned int FastIntMask);
void VIC_DisFastInt(unsigned int FastIntMask);

#endif // __LPC_VIC_H
