#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//通过改变TIM1->CCR1的值来改变占空比，从而控制LED0的亮度
#define PWM_VAL TIM1->CCR1
#define SEG_ON {TIM3->CR1 |= 0x01;}
#define SEG_OFF {TIM3->CR1 &= ~(0x01);}
void tim3Init(u16 arr,u16 psc);
void tim1PwmInit(u16 arr,u16 psc);
#endif























