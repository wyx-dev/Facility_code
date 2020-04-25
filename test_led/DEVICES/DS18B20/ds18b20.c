#include "ds18b20.h"
#include "usart.h"
#include "delay.h"
u8 signFlag = 1;			//温度正负标志位

/* 控制时序延时函数 */
void delay_time(unsigned char i)
{
	for(; i > 0; i--);
}

/* ds18b20复位 */
void ds18b20Reset(void)
{
	Ds18b20IO_OUT(); //SET PA0 OUTPUT
	Ds18b20DQ_OUT=1; 
	delay_us(50);
	Ds18b20DQ_OUT=0; //拉低DQ
	delay_us(500);    //拉低500us
	Ds18b20DQ_OUT=1; //DQ=1 
	delay_us(40);     //15US
}


/* ds18b20初始化 */
void ds18b20Init(void)
{
	RCC->APB2ENR |= 1<<2;    //使能PORTA时钟	   	 

	GPIOA->CRL &= 0XFFF0FFFF; 
	GPIOA->CRL |= 0X00030000;//PA4推挽输出 50Mhz
	
	GPIOA->ODR |= 1<<4;
	
	ds18b20Reset();
}


//从DS18B20读取一个位
//返回值：1/0
u8 ds18b20ReadBit(void) 			 // read one bit
{
    u8 data;
    Ds18b20IO_OUT();//SET PA0 OUTPUT
    Ds18b20DQ_OUT=0; 
    delay_us(2);
    Ds18b20DQ_OUT=1; 
    Ds18b20IO_IN();//SET PA0 INPUT
    delay_us(12);
    if(Ds18b20DQ_IN)data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}


/* 读一个字节 */
unsigned char readOneChar(void)  		//主机数据线先从高拉至低电平1us以上，再使数据线升为高电平，从而产生读信号
{
	u8 i,j,dat;
    dat=0;
    for (i=1;i<=8;i++) 
    {
        j=ds18b20ReadBit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}


/* 写一个字节 */
void writeOneChar(unsigned char dat) 
{ 
	u8 j;
	u8 testb = 0;
	Ds18b20IO_OUT();//SET PA0 OUTPUT;
	for (j = 1; j <= 8; j++) 
	{
		testb = dat & 0x01;
		dat = dat >> 1;
		if (testb) 
		{
				Ds18b20DQ_OUT=0;// Write 1
				delay_us(2);                            
				Ds18b20DQ_OUT=1;
				delay_us(60);             
		}
		else 
		{
				Ds18b20DQ_OUT=0;// Write 0
				delay_us(60);             
				Ds18b20DQ_OUT=1;
				delay_us(2);                          
		}
	}
}

/*
等待DS18B20的回应
返回1:未检测到DS18B20的存在
返回0:存在
*/
u8 ds18b20Check(void) 	   
{   
    u8 retry=0;
    Ds18b20IO_IN();//SET PA0 INPUT	 
    while (Ds18b20DQ_IN&&retry<200)
    {
        retry++;
        delay_us(1);
    }
    if(retry>=200)return 1;
    else retry=0;
    while (!Ds18b20DQ_IN&&retry<240)
    {
        retry++;
        delay_us(1);
    }
    if(retry>=240)return 1;	    
    return 0;
}

/* 温度转换函数 */						//每读取一次都需要转换一次温度，和普通的不一样
void ds18b20Start(void)
{
	ds18b20Reset();		//复位
	
	ds18b20Check();
	
	writeOneChar(0xcc); 				//跳过读序列号的操作
	writeOneChar(0x44); 				//启动温度转换
}


/* 读温度函数
	integer：整数部分 
	decimal: 小数部分
*/

int readTemp(void)
{
	u8 temp;
	u8 TL,TH;
	int tem;
	ds18b20Start ();                    // ds1820 start convert
	ds18b20Reset();
	ds18b20Check();	 
	writeOneChar(0xcc);// skip rom
	writeOneChar(0xbe);// convert	    
	TL = readOneChar(); // LSB   
	TH = readOneChar(); // MSB  
	
	if(TH > 7)
	{
		//温度为负，取补码
		TH = ~TH;
		TL = ~TL; 
		temp = 0;//温度为负  
	}else temp = 1;//温度为正	  	  
	tem = TH; //获得高八位
	tem <<= 8;    
	tem += TL;//获得底八位
	tem = (int)((float)tem*0.625);//转换 0.625 = 5 / 8   除以16乘10
	if(temp)return tem; //返回温度值
	else return -tem;    
}
