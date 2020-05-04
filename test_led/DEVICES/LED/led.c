#include "sys.h"   
#include "led.h"
//初始化PC13 PA3为输出口.并使能这两个口的时钟		    
//LED IO初始化
void ledInit(void)
{ 	 
	RCC->APB2ENR |= 1<<4;    //使能PORTC时钟	   
	RCC->APB2ENR |= 1<<2;    //使能PORTA时钟	   	 
	
	GPIOA->CRL &= 0XFFFF0FFF;
	GPIOA->CRL |= 0X00003000;//PA3推挽输出 
	
	
	GPIOC->CRH &= 0XFF0FFFFF;
	GPIOC->CRH |= 0X00300000;//PC13
	
	LED0 = 1;
	LED1 = 1;
}















