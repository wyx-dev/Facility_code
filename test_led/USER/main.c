#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "seg.h"
#include "ds18b20.h"

/* 全局变量定义区 */
u8 integer = 0;				//温度整数
u8 decimal = 0;				//温度小数
int temp = 0;			//温度值

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
	ds18b20Init();				//初始化ds18b20所用到的1个IO口
	
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







