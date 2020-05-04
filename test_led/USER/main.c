#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "seg.h"
#include "ds18b20.h"
#include "key.h"
#include "beep.h"
#include "timer.h"

/* ȫ�ֱ��������� */
u8 integer = 0;				//�¶�����
u8 decimal = 0;				//�¶�С��
u8 button = 0;
int targetTemp = 300;
int currrentTemp = 0;
u8 set = 0;
int count = 0;

/* ���������� */
void setTargetTemp(void);

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
	ds18b20Init();			  //��ʼ��ds18b20���õ���1��IO��
	keyInit();						//ɨ�谴��IO��ʼ��
	extiInit();						//�жϰ�����ʼ��
	beepInit();						//��ʼ��������
	timerInit(50,3599);		//10khz ������50
	SEG_ON;

	while(1)
	{
//		if(PBout(7))
//			PAout(8) = 0;
//		//delay(1000);
//		else
//			PAout(8) = 1;
//		delay(1000);
/* ����ܲ��� */
//		seg4Test();
//		seg4Display(102);
/* ��ȡ�¶Ȳ��� */
//		currrentTemp = readTemp();
//		seg4Display(currrentTemp);
/* �������� */
//		button = keyScan(0);
//		if(button != 0)
//			PCout(13) = ~PCout(13);
/* ���������� */
//		BEEP_ON;
//		delay(500);
//		BEEP_OFF;
//		delay(500);

/* ������ ���� �¶� ��������ϲ��� */
		if(set == 1)
			setTargetTemp();

		//��ȡ�¶�
		if((count % 3) == 0)
		{
			currrentTemp = readTemp();
			
		}

		//����Ŀ���¶�10��ͱ���
		if(currrentTemp > (targetTemp + 100))
		{
			if(count % 100000 == 0)
				BEEP = !BEEP;
		}
		
		
//		else
//		{
//			delay()
//		}
//		PAout(3) = 1;
//		delay(1000);
//		PAout(3) = 0;
//		delay(1000);
		//printf("running\r\n");
	}
}

/* ����Ŀ���¶� */
void setTargetTemp(void)
{
	u8 key = 0;
	SEG_OFF;//�رն�ʱ���ж���ʾ�������ڵ�����ʾ
	BEEP_OFF; 
	//ָʾ�� ָʾ��������
	LED1 = 0;
	while(1)
	{
		key = 0;
		//�ȴ���������
		while(key == 0)
		{
			//printf("target:%d\r\n",targetTemp);
			seg4Display(targetTemp);
			key = keyScan(1);
			if(set == 0)
			{
				SEG_ON;
				LED1 = 1;
				set = 0;//��ʼ��set����
				return;
			}
		}
		
		//�жϰ����Ǹ�����
		if(key == ADD)
		{
			if(targetTemp < 65535)
				targetTemp += 5;
		}
		else if(key == SUB)
		{
			if(targetTemp > 0)
				targetTemp -= 5;
		}
		//����
		delay(200);
	}

}

/*
��ࣺ�ӳ����Ƴ� ��Ա�������顢����Զ�����������
FPGA���ӳ����Ƴ� ��Ա������ң�������顢����Զ����ʱ��
while(1)
{
	PAout(0) = 0;
	delay_ms(1000);
	PAout(0) = 1;
	delay_ms(1000);
}
*/
