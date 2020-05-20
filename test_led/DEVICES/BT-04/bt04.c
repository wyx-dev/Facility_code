#include "delay.h" 			 
#include "usart.h" 			 
#include "usart2.h" 			 
#include "bt04.h" 
#include "led.h" 
#include "string.h"	 
#include "math.h"

//初始化ATK-BT04模块
//返回值:0,成功;1,失败.
u8 BT04_Init(void)
{
	USART2_Init(36,9600);	//初始化串口2为:9600,波特率
	return 0;
}
