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
	
	Stm32_Clock_Init(9); 	//系统时钟设置
	delay_init(72);	     	//延时初始化
	LED_Init();		  	 	//初始化与LED连接的硬件接口    
	segInit();					//初始化seg所用到的三个IO口
	
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







