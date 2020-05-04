#include "sys.h"   
#include "beep.h"
//初始化PA0为输出口.并使能这两个口的时钟
//BEEP IO初始化
void beepInit(void)
{
	RCC->APB2ENR |= 1<<2;    //使能PORTA时钟	   	 
	
	GPIOA->CRL &= 0XFFFFFFF0;
	GPIOA->CRL |= 0X00000003;//PA0 推挽输出 

  BEEP_OFF;
}















