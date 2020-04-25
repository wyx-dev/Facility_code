#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "seg.h"
#include "ds18b20.h"

/* ȫ�ֱ��������� */
u8 integer = 0;				//�¶�����
u8 decimal = 0;				//�¶�С��
int temp = 0;			//�¶�ֵ

/*diy_wyx*/
#define FLAG_PROTUES 		0
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
	uart_init(72, 9600);
	ledInit();		  	 		//��ʼ����LED���ӵ�Ӳ���ӿ�    
	segInit();						//��ʼ��seg���õ�������IO��
	ds18b20Init();				//��ʼ��ds18b20���õ���1��IO��
	
	while(1)
	{
//		PAout(8) = 1;
//		delay(1000);
//		PAout(8) = 0;
//		delay(1000);
//		seg4Test();
//		seg4Display(102);
		
		temp = readTemp();;
//		
		seg4Display(temp);
//		PCout(13) = 1;
//		delay(1000);
//		PCout(13) = 0;
//		delay(1000);
		
	}	 
}







