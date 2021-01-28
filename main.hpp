
#ifndef _IS_INCLUDED_MAIN_H
#define _IS_INCLUDED_MAIN_H

extern unsigned int time_to_arch;//час до наступного архівування в секунах
//extern A_MNG a_mng;//управління процесом архівації

extern int arch_flash_addr;
extern int arch_page;//сторінка 0-16383 архіву у флеш розміром 256 байт - 16 записі
extern int next_arch_page;//сторінка 0-16383 архіву у флеш розміром 256 байт - 16 записів   для відобаження наступної

extern char ERROR;//КОД ПОМИЛКИ ЯКЩО НЕ НУЛЬВИЙ
extern volatile int ERROR_TIME;//час до якого дійсна помилка
extern char ERROR_I;//ІНІЦІАТОР ПОМИЛКИ ЧАСТОТНИКИ І ВСІ ІНШІ

#endif
