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
int currrentTemp = 0;
u8 set = 0;
int count = 0;

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
	timerInit(50,3599);		//10khz 计数到50
	SEG_ON;

	while(1)
	{
//		if(PBout(7))
//			PAout(8) = 0;
//		//delay(1000);
//		else
//			PAout(8) = 1;
//		delay(1000);
/* 数码管测试 */
//		seg4Test();
//		seg4Display(102);
/* 读取温度测试 */
//		currrentTemp = readTemp();
//		seg4Display(currrentTemp);
/* 按键测试 */
//		button = keyScan(0);
//		if(button != 0)
//			PCout(13) = ~PCout(13);
/* 蜂鸣器测试 */
//		BEEP_ON;
//		delay(500);
//		BEEP_OFF;
//		delay(500);

/* 蜂鸣器 按键 温度 数码管联合测试 */
		if(set == 1)
			setTargetTemp();

		//读取温度
		if((count % 3) == 0)
		{
			currrentTemp = readTemp();
			
		}

		//超过目标温度10°就报警
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

/* 设置目标温度 */
void setTargetTemp(void)
{
	u8 key = 0;
	SEG_OFF;//关闭定时器中断显示，函数内调用显示
	BEEP_OFF; 
	//指示灯 指示在设置中
	LED1 = 0;
	while(1)
	{
		key = 0;
		//等待按键按下
		while(key == 0)
		{
			//printf("target:%d\r\n",targetTemp);
			seg4Display(targetTemp);
			key = keyScan(1);
			if(set == 0)
			{
				SEG_ON;
				LED1 = 1;
				set = 0;//初始化set变量
				return;
			}
		}
		
		//判断按的那个按键
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
		//防误触
		delay(200);
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
