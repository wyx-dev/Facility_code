#include "timer.h"
#include "led.h"
 
//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void tim3Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值 
	TIM3->PSC=psc;  	//预分频器设置
	TIM3->DIER|=1<<0;   //允许更新中断				
	TIM3->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);//抢占1，子优先级3，组2									 
}
//TIM1_CH1 CH4 PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void tim1PwmInit(u16 arr,u16 psc)
{
	//此部分需手动修改IO口设置
	RCC->APB2ENR|=1<<11; 	//TIM1时钟使能    
	GPIOA->CRH&=0XFFFF0FF0;	//PA8清除之前的设置
	GPIOA->CRH|=0X0000B00B;	//复用功能输出 
	
	TIM1->ARR=arr;			//设定计数器自动重装值 
	TIM1->PSC=psc;			//预分频器设置
  
	TIM1->CCMR1|=6<<4;  	//CH1 PWM2模式
	TIM1->CCMR1|=1<<3; 		//CH1预装载使能	 
 	TIM1->CCER|=1<<0;   	//OC1 输出使能	   

	TIM1->CCMR2|=6<<12;  	//CH4 PWM2模式		 
	TIM1->CCMR2|=1<<11; 	//CH4预装载使能	 
 	TIM1->CCER|=1<<12;   	//OC4 输出使能	 

	TIM1->BDTR|=1<<15;   	//MOE 主输出使能	   

	TIM1->CR1=0x0080;   	//ARPE使能 
	TIM1->CR1|=0x01;    	//使能定时器1 										  
}  

//flag区分陶瓷加热片和电风扇. 1 为陶瓷加热片 0 电风扇
void setPwm(int moto, u8 flag)
{
	int tmp = 0;

	//区分陶瓷加热片和电风扇. 1 为陶瓷加热片 0 电风扇
	(flag==1)? (tmp = 7199) : (tmp = 30);
	
	//限幅
	if(moto > tmp)
	{
		moto = tmp;
	}
	if(moto < 0)
	{
		moto = 0;
	}

	//赋值给寄存器
	(flag==1)? (PWM_CH1_VAL = moto) : (PWM_CH4_VAL = moto);
		
}












