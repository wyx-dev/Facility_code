#ifndef __LED_H
#define __LED_H	 
#include "sys.h"	   

//LED端口定义
#define LED0 PCout(13)	// PA13
#define LED1 PAout(3)	// PA13
void ledInit(void);	//初始化		 				    
#endif

