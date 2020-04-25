#ifndef __DS18B20_H
#define __DS18B20_H	 
#include "sys.h"   

//IO方向设置PA4
#define Ds18b20IO_IN()  {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL |= 0X00080000;}
#define Ds18b20IO_OUT() {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL |= 0X00030000;}

////IO操作函数											   
#define	Ds18b20DQ_OUT PAout(4) //数据端口	PA4 
#define	Ds18b20DQ_IN  PAin(4)  //数据端口	PA4 

void ds18b20Init(void);										//初始化

/* 读温度函数
	返回值：温度值
*/
int readTemp(void);	//读温度函数	
#endif
