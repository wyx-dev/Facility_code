#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define ADD	1		//ADD����
#define SUB	2		//SUB����
#define SET	3		//SET����

void KEY_Init(void);		//IO��ʼ��
u8 KEY_Scan(u8 mode);		//����ɨ�躯��
#endif
