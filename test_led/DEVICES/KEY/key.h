#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define ADD	1		//ADD按下
#define SUB	2		//SUB按下
#define SET	3		//SET按下

void keyInit(void);		//IO初始化
u8 keyScan(u8 mode);		//按键扫描函数
void EXTI_Init(void);
#endif
