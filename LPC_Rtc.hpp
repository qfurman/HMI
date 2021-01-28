#ifndef __LPC_RTC_HPP
#define __LPC_RTC_HPP

/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2003
 *
 *    File name   : LPC_Rtc.h
 *    Description : Define RTI structure and relative micro
 *
 *    History :
 *    1. Data        : August 10, 2004
 *       Author      : Shawn Zhang
 *       Description : Create
 *
 *    2. Data        : Oct 12, 2004
 *       Author      : Stanimir Bonev
 *       Description: Modify the interface of some functions
 *
 *    $Revision: 1.1 $
 **************************************************************************/


typedef struct {
	unsigned short year;  // year value
	unsigned char month;  // month value
	unsigned char day;    // day value

	// the below member is used in get-operation
	unsigned char DOW;    // Day of week
	unsigned char DOY;    // Day of year
} LPC_Rtc_Date_t;

typedef struct {
	unsigned char hour;   // hour value
	unsigned char minute; // minute value
	unsigned char second; // second value
} LPC_Rtc_Time_t;

typedef struct {
	unsigned short year;  // year value
	unsigned char month;  // month value
	unsigned char day;    // day value

	unsigned char hour;   // hour value
	unsigned char minute; // minute value
	unsigned char second; // second value

	// the below member is used in get-operation
	unsigned char DOW;    // Day of week
	unsigned char DOY;    // Day of year
} LPC_Rtc_DateTime_t;




/* Declare functions */
extern "C" void RTC_Enable(void);
extern "C" void RTC_Disable(void);

extern "C" int RTC_Init(void);

extern "C" int RTC_SetDate (LPC_Rtc_Date_t *pDate);
extern "C" int RTC_SetTime (LPC_Rtc_Time_t *pTime);
extern "C" int RTC_SetDateTime (LPC_Rtc_DateTime_t *pDateTime);

extern "C" int RTC_GetDate (LPC_Rtc_Date_t *pDate);
extern "C" int RTC_GetTime (LPC_Rtc_Time_t *pTime);
extern "C" int RTC_GetDateTime (LPC_Rtc_DateTime_t *pDateTime);

extern "C" void RTC_SetIncInt (unsigned char IncIntType);
extern "C" void RTC_DisableIncInt(void);

extern "C" void RTC_SetAlarmInt (unsigned char AlarmIntType);
extern "C" void RTC_DisableAlarmInt(void);
extern "C" int RTC_SetAlarmDateTime (LPC_Rtc_DateTime_t *pDateTime);

extern "C" int RTC_ClearInt(unsigned long IntType);
extern "C" unsigned long RTC_CheckIntType(void);

extern "C" void RTC_ISR (void);

extern "C" void FormatDate (int Type, LPC_Rtc_Date_t *pDate, char *s);
extern "C" void FormatTime (int Type, LPC_Rtc_Time_t *pTime, char *s);
extern "C" void FormatDateTime (int Type, LPC_Rtc_DateTime_t *pDateTime, char *s);

extern "C" void SysTimeUpdate (void);
extern "C" void Alarm (void);


#endif  // __LPC_RTC_H
