#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"	   

//LED�˿ڶ���
#define BEEP 			PAout(0)	// PA0
#define BEEP_ON 	BEEP = 0
#define BEEP_OFF 	BEEP = 1

void beepInit(void);	//��ʼ��		 				    
#endif

