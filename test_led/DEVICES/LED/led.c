#include "sys.h"   
#include "led.h"
//初始化PA8和PA9为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟	   	 
	//RCC->APB2ENR|=1<<5;    //使能PORTD时钟	   	 
	GPIOA->CRH&=0XFFFFFF00; 
	GPIOA->CRH|=0X00000033;//PA8,9 推挽输出   	 
//  GPIOA->ODR|=1<<8;      //PA8 输出高
//	GPIOA->ODR|=1<<9;      //PA9 输出高
}















