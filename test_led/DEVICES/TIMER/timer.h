#ifndef __TIMER_H
#define __TIMER_H
#include <sys.h>	 

void timerInit(u16 arr,u16 psc);

#define SEG_ON	{TIM1->CR1 |= 0x01;}    //使能定时器
#define SEG_OFF	{TIM1->CR1 &= ~(0x01);}    //使能定时器

#endif
