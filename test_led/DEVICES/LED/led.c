#include "sys.h"   
#include "led.h"
//初始化PA8为输出口.并使能这两个口的时钟		    
//LED IO初始化
void ledInit(void)
{
	RCC->APB2ENR |= 1<<2;    //使能PORTA时钟	   	 
	RCC->APB2ENR |= 1<<4;    //使能PORTC时钟	   
	
	GPIOA->CRH &= 0XFFFFFFF0; 
	GPIOA->CRH |= 0X00000003;//PA8 推挽输出   	 
	
	GPIOC->CRH &= 0XFF0FFFFF;
	GPIOC->CRH |= 0X00300000;
}















