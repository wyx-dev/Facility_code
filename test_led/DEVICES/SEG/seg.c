#include "seg.h"
#include "math.h"
void segInit(void)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟	   	 

	GPIOA->CRL&=0X000FFFFF; 
	GPIOA->CRL|=0X33300000;//PA5 6 7 推挽输出   	

	GPIOA->ODR|=1<<5;      //PA5 输出高
	GPIOA->ODR|=1<<6;      //PA6 输出高	
	GPIOA->ODR|=1<<7;      //PA7 输出高
}


//7段数码管显示表
unsigned char SEG_0F[] = 
{// 0	 1	  2	   3	4	 5	  6	   7	8	 9	  A	   b	C    d	  E    F    -
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0xBF,0xC6,0xA1,0x86,0xFF,0xbf
};

/*数码管显示函数*/
void seg4Display(float temp)
{
	unsigned char * pTableSeg; //查表指针
	unsigned char data;
	unsigned char seg_i = 0;
	
	int integer = (int )(temp*10);
	//显示
	for(seg_i = 4; seg_i >0; --seg_i)
	{
		//寻找显示数据
		pTableSeg = SEG_0F + (int)((int)(integer/pow(10,(4-seg_i))) % 10);
		data = *pTableSeg;
		if(seg_i == 3)
			data &= ~(1<<7);
		
		
		//输出到数码管
		segOut(data);
		//片选
		segOut(1<<(seg_i - 1));
		
		ST_CP = 0;
		ST_CP = 1;
	}
}

/*数码管测试函数*/
void seg4Test(void)
{
	unsigned char * pTableSeg; //查表指针
	unsigned char data;
	unsigned char seg_i = 0;
	//显示
	for(seg_i = 1; seg_i < 5; ++seg_i)
	{
		//寻找显示数据
		pTableSeg = SEG_0F + seg_i;
		data = *pTableSeg;
		//输出到数码管
		segOut(data);
		//片选
		segOut(1<<(seg_i - 1));
		
		ST_CP = 0;
		ST_CP = 1;
	}
}

/*数码管输出函数*/
void segOut(unsigned char X)
{
	unsigned char i;
	for(i=8;i>=1;i--)
	{
		if (X&0x80) 
			DS=1; 
		else 
			DS=0;
		X<<=1;
		SH_CP = 0;
		SH_CP = 1;
	}
}

