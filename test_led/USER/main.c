#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "seg.h"
#include "ds18b20.h"
#include "key.h"


/* 全局变量定义区 */
u8 integer = 0;				//温度整数
u8 decimal = 0;				//温度小数
u8 button = 0;
u8 targetTemp = 0;
u8 currrentTemp = 0;


/* 函数定义区 */
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
	
	Stm32_Clock_Init(9); 	//系统时钟设置
	delay_init(72);	     	//延时初始化
	uart_init(72, 9600);
	ledInit();		  	 		//初始化与LED连接的硬件接口    
	segInit();						//初始化seg所用到的三个IO口
//	ds18b20Init();				//初始化ds18b20所用到的1个IO口
	KEY_Init();
	setTemp();

	while(1)
	{
//		PAout(8) = 1;
//		delay(1000);
//		PAout(8) = 0;
//		delay(1000);
/* 数码管测试 */
//		seg4Test();
//		seg4Display(102);
/* 读取温度测试 */
//		currrentTemp = readTemp();
//		seg4Display(currrentTemp);
/* 按键测试 */
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

/* 设置目标温度 */
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
汇编：队长：黄超 队员：王宇祥、刘定远、崔宇航、王晨
FPGA：队长：黄超 队员：孙逍遥、王宇祥、刘定远、江时盼
*/






