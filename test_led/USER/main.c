#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "seg.h"
/*diy_wyx*/
#define FLAG_PROTUES 		1
void (*delay)(u16);
/*diy_wyx*/


int main(void)
{		 
	delay = delay_ms;
	#if FLAG_PROTUES
	delay = delay_diy;
	#endif
	
	Stm32_Clock_Init(9); 	//ϵͳʱ������
	delay_init(72);	     	//��ʱ��ʼ��
	LED_Init();		  	 	//��ʼ����LED���ӵ�Ӳ���ӿ�    
	segInit();					//��ʼ��seg���õ�������IO��
	
	while(1)
	{
//		PAout(8) = 1;
//		PAout(9) = 1;
//		delay(1000);
//		PAout(8) = 0;
//		PAout(9) = 0;
//		delay(1000);
		seg4Display(1);
	}	 
}







