#include "key.h"
#include "delay.h"
 	    
//按键初始化函数 
//PB 5 6 7 设置成输入
void keyInit(void)
{
	RCC->APB2ENR|=1<<3;     //使能PORTB时钟
	//RCC->APB2ENR|=1<<4;     //使能PORTC时钟
	
	GPIOB->CRL&=0XF00FFFFF;	//PB5 6设置成输入	  
	GPIOB->CRL|=0X08800000;   
	
	GPIOB->ODR &= ~(3<<5);	   	//PB5 6下拉 
}
/*
按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//返回值：
//0，没有任何按键按下
//KEY0_PRES，ADD按下
//KEY1_PRES，SUB按下
//KEY2_PRES，SET按下
//注意此函数有响应优先级,ADD>SUB>SET!!
*/
u8 keyScan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if((PBin(5)==1||PBin(6)==1) && key_up)
	{
		delay_ms(10);//去抖动
		key_up=0;
		if(PBin(5)==1)return ADD;
		else if(PBin(6)==1)return SUB;
	}else if(PBin(5)==0&&PBin(6)==0) key_up=1;    
	return 0;// 无按键按下
}

/* 中断按键初始化函数 */
void EXTI_Init(void)
{
	RCC->APB2ENR|=1<<3;     		//使能PORTB时钟
	GPIOB->CRL&=0X0FFFFFFF;			//PB 7设置成输入
	GPIOB->CRL|=0X80000000;
	GPIOB->ODR &= ~(1<<7);	   	//PB 7下拉

	Ex_NVIC_Config(GPIO_B,7,RTIR);		//上升沿触发
	MY_NVIC_Init(2,1,EXTI9_5_IRQn,2);  	//抢占2，子优先级1，组2
}

void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);   		//消抖
  if(PBin(7) == 1)		//按键SET
	{
		if(set == 1)
			set = 0;
		else
			set = 1;

	}
 	EXTI->PR=1<<7;			//清除LINE5上的中断标志位  
}




















