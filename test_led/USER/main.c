#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 

/*
（仿真时用）
自定义软件延迟函数，由于仿真时系统延时不可用。所以用此延时
*/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

int main(void)
{		 
	Stm32_Clock_Init(9); 	//系统时钟设置
	delay_init(72);	     	//延时初始化
	LED_Init();		  	 	//初始化与LED连接的硬件接口    
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







