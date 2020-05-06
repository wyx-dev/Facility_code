#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "seg.h"
#include "ds18b20.h"
#include "key.h"
#include "beep.h"
#include "timer.h"

/* 全局变量定义区 */
u8 integer = 0;				//温度整数
u8 decimal = 0;				//温度小数
u8 button = 0;
int targetTemp = 300;
int currentTemp = 0;
int alarmTemp = 400;
u8 set = 0;
unsigned int time_count = 0;
int Kp = 72;

/* 函数定义区 */
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

	Stm32_Clock_Init(9); 	//系统时钟设置
	delay_init(72);	     	//延时初始化
	uart_init(72, 9600);
	ledInit();		  	 		//初始化与LED连接的硬件接口    
	segInit();						//初始化seg所用到的三个IO口
	ds18b20Init();			  //初始化ds18b20所用到的1个IO口
	keyInit();						//扫描按键IO初始化
	extiInit();						//中断按键初始化
	beepInit();						//初始化蜂鸣器
	tim3Init(50,7199);		//10khz 计数到50
	tim1PwmInit(7199,0);	//10khz 计数到50
	SEG_ON;

	while(1)
	{
//		if(PBout(7))
//			PAout(3) = 1;
//		//delay(1000);
//		else
//			PAout(8) = 1;
//		delay(1000);
/* 数码管测试 */
//		seg4Test();
//		seg4Display(102);
/* 读取温度测试 */
//		currentTemp = readTemp();
//		seg4Display(currentTemp);
/* 按键测试 */
//		button = keyScan(0);
//		if(button != 0)
//			PCout(13) = ~PCout(13);
/* 蜂鸣器测试 */
//		BEEP_ON;
//		delay(500);
//		BEEP_OFF;
//		delay(500);
/* 定时器和PWM测试(PWM_VAL越大，输出电压越小) */
//		PWM_VAL = 7000;
//		delay(1000);
//		PWM_VAL = 1000;
//		delay(1000);
/* 蜂鸣器 按键 温度 数码管联合测试 */
		if(set == 1)
		{
			BEEP_OFF;
			PWM_VAL = 7199;
			setTargetTemp();
		}

		//读取温度 并处理温度
		if((time_count % 3) == 0)
		{
//			BEEP = !BEEP;
//			LED0 = !LED0;
			currentTemp = readTemp();

			//当前温度处于目标温度正负0.5，停止加热。蜂鸣器响。
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

			//小于目标温度，加热
			if(currentTemp < (targetTemp-5))
			{
				PWM_VAL = 7200 - (targetTemp - currentTemp) * Kp;
			}

			//大于目标温度，停止加热
			else if(currentTemp < (targetTemp+5))
			{
				PWM_VAL = 7199;
			}
		}

		//超过报警温度就报警
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
		设置目标/报警温度
		双击set设置报警温度
		单机set设置目标温度
*/
void setTargetTemp(void)
{
	u8 key = 0;
	int* setTemp;
	u8 flagSetType = 1;	//1表示目标温度，0表示报警温度
	
	//如果在500ms内连按set两次，就会进入设置报警温度
	delay_diy(5000);
	
	if(set == 0)
	{
		setTemp = &alarmTemp;
		set = 1;
		flagSetType = 0;
	}
	//否则设置目标温度
	else
	{
		setTemp = &targetTemp;
		flagSetType = 1;
		LED1 = 0;
	}

	while(1)
	{
		key = 0;
		//等待按键按下
		while(key == 0)
		{
			//显示目标温度，便于设置
			currentTemp = *setTemp;
			
			//如果设置的是报警模式，指示灯为闪烁。
			if(flagSetType == 0)
				LED1 = !LED1;
			delay(200);
			//set中断按下改变set变量值，退出设置模式，清除相关初始化变量
			if(set == 0)
			{
				//SEG_ON;
				LED1 = 1;
				set = 0;//初始化set变量
//				alarmTemp = targetTemp + 100;//默认报警温度为目标温度+10°。
				return;
			}

			//查询式 扫描按键（参数1代表支持连按）
			key = keyScan(1);
		}

		//判断按的那个按键，加或减目标温度
		if(key == ADD)
		{
			if(*setTemp < 65535)//限幅
				*setTemp += 5;
		}
		else if(key == SUB)
		{
			if(*setTemp > 0)		//限幅
				*setTemp -= 5;
		}

		//防误触
		//delay(200);
	}

}

/*
汇编：队长：黄超 队员：王宇祥、刘定远、崔宇航、王晨
FPGA：队长：黄超 队员：孙逍遥、王宇祥、刘定远、江时盼
while(1)
{
	PAout(0) = 0;
	delay_ms(1000);
	PAout(0) = 1;
	delay_ms(1000);
}
*/
