#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//ͨ���ı�TIM1->CCR1��ֵ���ı�ռ�ձȣ��Ӷ�����LED0������
#define PWM_VAL TIM1->CCR1
#define SEG_ON {TIM3->CR1 |= 0x01;}
#define SEG_OFF {TIM3->CR1 &= ~(0x01);}
void tim3Init(u16 arr,u16 psc);
void tim1PwmInit(u16 arr,u16 psc);
#endif























