#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define ADD	1		//ADD����
#define SUB	2		//SUB����
#define SET	3		//SET����

void keyInit(void);		//IO��ʼ��
u8 keyScan(u8 mode);		//����ɨ�躯��
void EXTI_Init(void);
#endif
