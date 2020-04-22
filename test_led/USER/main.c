#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 

/*
������ʱ�ã�
�Զ�������ӳٺ��������ڷ���ʱϵͳ��ʱ�����á������ô���ʱ
*/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

int main(void)
{		 
	Stm32_Clock_Init(9); 	//ϵͳʱ������
	delay_init(72);	     	//��ʱ��ʼ��
	LED_Init();		  	 	//��ʼ����LED���ӵ�Ӳ���ӿ�    
	while(1)
	{
		PAout(8) = 1;
		PAout(9) = 1;
//		delay_ms(300);
		Delay(0x2ffff);
		PAout(8) = 0;
		PAout(9) = 0;
//		delay_ms(300);
		Delay(0x2ffff);
	}	 
}







