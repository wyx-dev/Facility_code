#include "sys.h"   
#include "beep.h"
//��ʼ��PA8Ϊ�����.��ʹ���������ڵ�ʱ��
//BEEP IO��ʼ��
void beepInit(void)
{
	RCC->APB2ENR |= 1<<2;    //ʹ��PORTAʱ��	   	 
	
	GPIOA->CRH &= 0XFFFFFFF0;
	GPIOA->CRH |= 0X00000003;//PA8 ������� 

  BEEP_OFF;
}















