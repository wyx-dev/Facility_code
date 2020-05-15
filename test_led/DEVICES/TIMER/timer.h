#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//ͨ���ı�TIM1->CCR1��ֵ���ı�ռ�ձȣ��Ӷ�����LED0������
#define PWM_CH1_VAL TIM1->CCR1
#define PWM_CH4_VAL TIM1->CCR4
#define SEG_ON {TIM3->CR1 |= 0x01;}
#define SEG_OFF {TIM3->CR1 &= ~(0x01);}
#define PWM_OFF {TIM1->CCR1 = 0;TIM1->CCR4 = 0;}
void tim3Init(u16 arr,u16 psc);
void tim1PwmInit(u16 arr,u16 psc);
void setPwm(int moto, u8 flag);
#endif























