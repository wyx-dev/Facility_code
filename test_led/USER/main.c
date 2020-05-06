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
int currentTemp = 0;
int alarmTemp = 400;
u8 set = 0;
unsigned int time_count = 0;
int Kp = 72;

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
	tim3Init(50,7199);		//10khz ������50
	tim1PwmInit(7199,0);	//10khz ������50
	SEG_ON;

	while(1)
	{
//		if(PBout(7))
//			PAout(3) = 1;
//		//delay(1000);
//		else
//			PAout(8) = 1;
//		delay(1000);
/* ����ܲ��� */
//		seg4Test();
//		seg4Display(102);
/* ��ȡ�¶Ȳ��� */
//		currentTemp = readTemp();
//		seg4Display(currentTemp);
/* �������� */
//		button = keyScan(0);
//		if(button != 0)
//			PCout(13) = ~PCout(13);
/* ���������� */
//		BEEP_ON;
//		delay(500);
//		BEEP_OFF;
//		delay(500);
/* ��ʱ����PWM����(PWM_VALԽ�������ѹԽС) */
//		PWM_VAL = 7000;
//		delay(1000);
//		PWM_VAL = 1000;
//		delay(1000);
/* ������ ���� �¶� ��������ϲ��� */
		if(set == 1)
		{
			BEEP_OFF;
			PWM_VAL = 7199;
			setTargetTemp();
		}

		//��ȡ�¶� �������¶�
		if((time_count % 3) == 0)
		{
//			BEEP = !BEEP;
//			LED0 = !LED0;
			currentTemp = readTemp();

			//��ǰ�¶ȴ���Ŀ���¶�����0.5��ֹͣ���ȡ��������졣
			if((currentTemp > (targetTemp-5)) && ((currentTemp < (targetTemp+5))))
			{
				if((time_count % 100) == 0)
				{
					BEEP_ON;
					delay(50);
					BEEP_OFF;
					delay(50);
				}
			}

			//С��Ŀ���¶ȣ�����
			if(currentTemp < (targetTemp-5))
			{
				PWM_VAL = 7200 - (targetTemp - currentTemp) * Kp;
			}

			//����Ŀ���¶ȣ�ֹͣ����
			else if(currentTemp < (targetTemp+5))
			{
				PWM_VAL = 7199;
			}
		}

		//���������¶Ⱦͱ���
		if(currentTemp > alarmTemp)
		{
			BEEP_ON;
		}
//		PAout(3) = 1;
//		delay(1000);
//		PAout(3) = 0;
//		delay(1000);
		//printf("running\r\n");
	}
}

/* 
		����Ŀ��/�����¶�
		˫��set���ñ����¶�
		����set����Ŀ���¶�
*/
void setTargetTemp(void)
{
	u8 key = 0;
	int* setTemp;
	u8 flagSetType = 1;	//1��ʾĿ���¶ȣ�0��ʾ�����¶�
	
	//�����500ms������set���Σ��ͻ�������ñ����¶�
	delay_diy(5000);
	
	if(set == 0)
	{
		setTemp = &alarmTemp;
		set = 1;
		flagSetType = 0;
	}
	//��������Ŀ���¶�
	else
	{
		setTemp = &targetTemp;
		flagSetType = 1;
		LED1 = 0;
	}

	while(1)
	{
		key = 0;
		//�ȴ���������
		while(key == 0)
		{
			//��ʾĿ���¶ȣ���������
			currentTemp = *setTemp;
			
			//������õ��Ǳ���ģʽ��ָʾ��Ϊ��˸��
			if(flagSetType == 0)
				LED1 = !LED1;
			delay(200);
			//set�жϰ��¸ı�set����ֵ���˳�����ģʽ�������س�ʼ������
			if(set == 0)
			{
				//SEG_ON;
				LED1 = 1;
				set = 0;//��ʼ��set����
//				alarmTemp = targetTemp + 100;//Ĭ�ϱ����¶�ΪĿ���¶�+10�㡣
				return;
			}

			//��ѯʽ ɨ�谴��������1����֧��������
			key = keyScan(1);
		}

		//�жϰ����Ǹ��������ӻ��Ŀ���¶�
		if(key == ADD)
		{
			if(*setTemp < 65535)//�޷�
				*setTemp += 5;
		}
		else if(key == SUB)
		{
			if(*setTemp > 0)		//�޷�
				*setTemp -= 5;
		}

		//����
		//delay(200);
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
