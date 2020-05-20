#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "seg.h"
#include "ds18b20.h"
#include "key.h"
#include "beep.h"
#include "timer.h"
#include "string.h"
#include "bt04.h"
#include "usart2.h"

/* ȫ�ֱ��������� */
int targetTemp = 280;
int currentTemp = 0;
int alarmTemp = 400;
u8 set = 0, control_bluetooth = 0;
unsigned int time_count = 0;
int Kp = 72;
struct PID {
	unsigned int diff; // �趨Ŀ���뵱ǰ�Ĳ�ֵ Desired Value
	unsigned int proportion; // �������� ProporTIonal Const
	unsigned int integral; // ���ֳ��� Integral Const
	unsigned int derivative; // ΢�ֳ��� DerivaTIve Const
	unsigned int lastError; // Error��-1��
	unsigned int prevError; // Error��-2��
	unsigned int sumError; // Sums of Errors
}spid;


/* ���������� */
void setTargetTemp(void);
void alarmCheck(int temp);
u16 PID_calc(struct PID *pp);
void PIDInit(struct PID *pp);

/*diy_wyx*/
#define FLAG_PROTUES 		0
void (*delay)(u16);
/*diy_wyx*/


int main(void)
{
	u16 heatOut = 0;
	u16 coldOut = 0;
	u16 coldDiff = 0;
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
	while(BT04_Init()) 		//��ʼ��BT04ģ��  
	{
		//����ָʾ����˸
		LED0 = !LED0;
		delay_ms(500);
	}
	tim3Init(50,7199);		//10khz ������50
	tim1PwmInit(7199,0);	//10khz
	PIDInit(&spid);
	SEG_ON;

	spid.proportion = 36;
	spid.integral = 0;
	spid.derivative = 0;

	while(1)
	{
//		PWM_CH4_VAL = 2;//30--7.26   10---7.19  5---7.14  3---7.05 1--0 2--0
//		setPwm(7200,1);
//		/* ����PID��ϵͳ */
		if(set == 1)
		{
			BEEP_OFF;//�ط�����
			PWM_OFF;
			setTargetTemp();
			/* ����������� */
			u2_printf("A%.1f;%.1f\n",(float)targetTemp/10, (float)alarmTemp/10);
			delay_ms(500);
			u2_printf("BOFF");
			/* ************ */
		}

		//��ȡ�¶� �������¶�
		if((time_count % 3) == 0)
		{
//			LED0 = !LED0;
			currentTemp = readTemp();
			alarmCheck(currentTemp);

			//����ѵ�Ŀ�귶Χ,��ֹͣ����
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

			//����
			else if(currentTemp > targetTemp)
			{
				setPwm(0,1);//�رռ�����
				//����
				coldOut = coldDiff * 12; //300 ---3600
				setPwm(coldOut ,0);
			}
			
			//����
			else if(currentTemp < targetTemp)
			{
				setPwm(0,0);//�رշ���
				spid.diff = targetTemp - currentTemp;
				if(spid.diff > 0)
					heatOut = spid.diff * spid.proportion;

				//PID���ݴ���
				else
				{
					heatOut = PID_calc(&spid);
				}

				setPwm(heatOut, 1);
			}
			
		}
		/* ����PID��ϵͳ */
	}	
}


/*
PID_calc�������ܣ�PID���Ƽ������
���������struct PID *  PID�ṹ�������
�����u16 �������PWMֵ��
*/
u16 PID_calc(struct PID *pp)
{ 
	u16 out = 0;
	u16 dError = 0;

	pp->sumError += pp->diff; //����

	dError = pp->lastError - pp->prevError; //����𽥼�С��������Ч���������ϴμ����ϴ�Ϊ��
	
	//�������ϴ������ϴ����
	pp->prevError = pp->lastError;
	pp->lastError = pp->diff;

	out = (pp->proportion * pp->diff) + (pp->integral * pp->sumError) + (pp->derivative * dError);
	return out;
}


/* 
		����Ŀ��/�����¶�
		˫��set���ñ����¶�
		����set����Ŀ���¶�
*/
void setTargetTemp(void)
{
	
	int* setTemp;
	u8 flagSetType = 1;	//1��ʾĿ���¶ȣ�0��ʾ�����¶�
	u8 control_key = 0;
	
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
		control_key = 0;
		control_bluetooth = 0;
		/* ����������� */
		if(flagSetType == 0)//�� ����ֵ
			u2_printf("BalarmTemp:%.1f",((float)*setTemp/10));
		else				//���� Ŀ��ֵ
			u2_printf("BtargetTemp:%.1f",((float)*setTemp/10));
		/* ************ */
		//�ȴ���������
		while((control_key == 0) && (control_bluetooth == 0))
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
				set = 0;											//��ʼ��set����
				//alarmTemp = targetTemp + 100;	//Ĭ�ϱ����¶�ΪĿ���¶�+10�㡣���������ϻ����alarm�¶�������û���õ�Ч��,����!!!
				return;
			}

			//��ѯʽ ɨ�谴��������1����֧��������
			control_key = keyScan(1);
		}

		//�жϰ����Ǹ��������ӻ��Ŀ���¶�
		if((control_key == ADD) || (control_bluetooth == ADD))
		{
			if(*setTemp < 65535)//�޷�
				*setTemp += 5;
		}
		else if((control_key == SUB) || (control_bluetooth == SUB))
		{
			if(*setTemp > 0)		//�޷�
				*setTemp -= 5;
		}

		//����
		//delay(200);
	}
}

/*
�ӿ����ƣ�void alarmCheck(int temp);
���������temp(Ҫ�����¶�ֵ)��
�����������
*/
void alarmCheck(int temp)
{
	static u8 alarmCount = 0;

	//������Ŀ���¶�һ��ʱ��ű���,�൱������
	(temp > alarmTemp) ? alarmCount++ : (alarmCount = 0);

	if(alarmCount == 5)
	{
		BEEP_ON;
		alarmCount = 0;
	}
}
void PIDInit(struct PID *pp)
{
	memset(pp, 0, sizeof(struct PID));
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
//while(1)
//	{
// 		delay_ms(10);	 
//		if(dir)led0pwmval++;
//		else led0pwmval--;	 
// 		if(led0pwmval>7199)dir=0;
//		if(led0pwmval==0)dir=1;	   					 
//		PWM_VAL=led0pwmval;
//		PWM_VAL = 7100;
//		delay(1500);
//		PWM_VAL = 000;
//		delay(1500);

//		PAout(3) = 1;
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
//		PWM_VAL = 7100;
//		delay(1500);
//		PWM_VAL = 1000;
//		delay(1500);
/* ������ ���� �¶� ��������ϲ��� */
//		if(set == 1)
//		{
//			BEEP_OFF;
//			setPwm(7199, 1);
//			setTargetTemp();
//		}

//		//��ȡ�¶� �������¶�
//		if((time_count % 3) == 0)
//		{
////			BEEP = !BEEP;
////			LED0 = !LED0;
//			currentTemp = readTemp();

//			//��ǰ�¶ȴ���Ŀ���¶�����0.5��ֹͣ���ȡ��������졣
//			if((currentTemp > (targetTemp-5)) && ((currentTemp < (targetTemp+5))))
//			{
//				if((time_count % 100) == 0)
//				{
//					BEEP_ON;
//					delay(50);
//					BEEP_OFF;
//					delay(50);
//				}
//			}

//			//С��Ŀ���¶ȣ�����
//			if(currentTemp < (targetTemp-5))
//			{
//				moto = 7200 - (targetTemp - currentTemp) * Kp;
//			}

//			//����Ŀ���¶ȣ�ֹͣ����
//			else if(currentTemp < (targetTemp+5))
//			{
//				moto = 7199;
//			}
//			setPwm(moto, 1);
//		}

//		//����Ƿ񱨾�
//		alarmCheck(currentTemp);
//		PAout(3) = 1;
//		delay(1000);
//		PAout(3) = 0;
//		delay(1000);
		//printf("running\r\n");
//	}
	
	

