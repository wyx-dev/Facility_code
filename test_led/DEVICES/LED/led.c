#include "sys.h"   
#include "led.h"
//��ʼ��PA8��PA9Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��	   	 
	//RCC->APB2ENR|=1<<5;    //ʹ��PORTDʱ��	   	 
	GPIOA->CRH&=0XFFFFFF00; 
	GPIOA->CRH|=0X00000033;//PA8,9 �������   	 
//  GPIOA->ODR|=1<<8;      //PA8 �����
//	GPIOA->ODR|=1<<9;      //PA9 �����
}















