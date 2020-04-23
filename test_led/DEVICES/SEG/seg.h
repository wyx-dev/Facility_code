#ifndef __SEG_H
#define __SEG_H	 
#include "sys.h"   

//SEG端口定义
#define SH_CP 	PAout(5)	// PA5
#define DS 			PAout(6)	// PA6	
#define ST_CP 	PAout(7)	// PA7

void segInit(void);	//初始化		
void seg4Display(float temp); //seg显示
void seg4Test(void);//seg测试函数
void segOut(unsigned char X); //seg 8位输出函数

#endif
