

#ifndef _IS_INCLUDED_TIMERS_H
#define _IS_INCLUDED_TIMERS_H



void start_tx1();
void start_tx0();


void timer0_isr(void);
void timer1_isr(void);
extern volatile unsigned int timer_uart[2];
extern unsigned int time_10mS;
extern unsigned int pid_0[4];// таймер для під регуляторів


#endif

