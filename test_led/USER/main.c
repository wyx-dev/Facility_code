#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "seg.h"
#include "ds18b20.h"
#include "key.h"


/* ȫ�ֱ��������� */
u8 integer = 0;				//�¶�����
u8 decimal = 0;				//�¶�С��
u8 button = 0;
u8 targetTemp = 0;
u8 currrentTemp = 0;


/* ���������� */
void setTemp(void);

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
	uart_init(72, 9600);
	ledInit();		  	 		//��ʼ����LED���ӵ�Ӳ���ӿ�    
	segInit();						//��ʼ��seg���õ�������IO��
//	ds18b20Init();				//��ʼ��ds18b20���õ���1��IO��
	KEY_Init();
	setTemp();

	while(1)
	{
//		PAout(8) = 1;
//		delay(1000);
//		PAout(8) = 0;
//		delay(1000);
/* ����ܲ��� */
//		seg4Test();
//		seg4Display(102);
/* ��ȡ�¶Ȳ��� */
//		currrentTemp = readTemp();
//		seg4Display(currrentTemp);
/* �������� */
//		button = KEY_Scan(0);
//		if(button != 0)
//			printf("button: %d \r\n",button);

		
		
//		PCout(13) = 1;
//		delay(1000);
//		PCout(13) = 0;
//		delay(1000);
		printf("running\r\n");
	}	 
}

/* ����Ŀ���¶� */
void setTemp(void)
{
	u8 key = 0;
	while(1)
	{
		key = 0;
		while(key == 0)
		{
			printf("target:%d\r\n",targetTemp);
			
			//seg4Display(targetTemp);
			
			key = KEY_Scan(0);
		}
		if(key == SET)
			return ;
		else if(key == ADD)
		{
			if(targetTemp < 255)
				targetTemp += 5;
		}
		else if(key == SUB)
		{
			if(targetTemp > 0)
				targetTemp -= 5;
		}
		delay(1000);
	}
}

/*
��ࣺ�ӳ����Ƴ� ��Ա�������顢����Զ�����������
FPGA���ӳ����Ƴ� ��Ա������ң�������顢����Զ����ʱ��
*/






