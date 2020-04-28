#include "key.h"
#include "delay.h"
 	    
//按键初始化函数 
//PB 5 6 7 设置成输入
void KEY_Init(void)
{
	RCC->APB2ENR|=1<<3;     //使能PORTB时钟
	//RCC->APB2ENR|=1<<4;     //使能PORTC时钟
	
	GPIOB->CRL&=0X000FFFFF;	//PB 5 6 7设置成输入	  
	GPIOB->CRL|=0X88800000;   
	
	GPIOB->ODR|=7<<5;	   	//PB 5 6 7上拉 
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
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(PBin(5)==0||PBin(6)==0||PBin(7)==0)
	{
		delay_ms(10);//去抖动
		key_up=0;
		if(PBin(5)==0)return ADD;
		else if(PBin(6)==0)return SUB;
		else if(PBin(7)==0)return SET;
	}else if(PBin(5)==1&&PBin(6)==1&&PBin(7)==1) key_up=1;    
	return 0;// 无按键按下
}




















