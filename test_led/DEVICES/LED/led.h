#ifndef __LED_H
#define __LED_H	 
#include "sys.h"	   

//LED端口定义
#define LED0 PCout(13)	// PC13
#define LED1 PAout(1)	// PA1
void ledInit(void);	//初始化		 				    
#endif

