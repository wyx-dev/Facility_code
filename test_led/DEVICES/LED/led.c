#include "sys.h"   
#include "led.h"
//��ʼ��PC13 PA3Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void ledInit(void)
{ 	 
	RCC->APB2ENR |= 1<<4;    //ʹ��PORTCʱ��	   
	RCC->APB2ENR |= 1<<2;    //ʹ��PORTAʱ��	   	 
	
	GPIOA->CRL &= 0XFFFF0FFF;
	GPIOA->CRL |= 0X00003000;//PA3������� 
	
	
	GPIOC->CRH &= 0XFF0FFFFF;
	GPIOC->CRH |= 0X00300000;//PC13
	
	LED0 = 1;
	LED1 = 1;
}















