#include "sys.h"   
#include "led.h"
//��ʼ��PA8Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void ledInit(void)
{
	RCC->APB2ENR |= 1<<2;    //ʹ��PORTAʱ��	   	 
	RCC->APB2ENR |= 1<<4;    //ʹ��PORTCʱ��	   
	
	GPIOA->CRH &= 0XFFFFFFF0; 
	GPIOA->CRH |= 0X00000003;//PA8 �������   	 
	
	GPIOC->CRH &= 0XFF0FFFFF;
	GPIOC->CRH |= 0X00300000;
}















