#include "sys.h"   
#include "beep.h"
//��ʼ��PA0Ϊ�����.��ʹ���������ڵ�ʱ��
//BEEP IO��ʼ��
void beepInit(void)
{
	RCC->APB2ENR |= 1<<2;    //ʹ��PORTAʱ��	   	 
	
	GPIOA->CRL &= 0XFFFFFFF0;
	GPIOA->CRL |= 0X00000003;//PA0 ������� 

  BEEP_OFF;
}















