#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"	   

//LED�˿ڶ���
#define BEEP 			PAout(8)	// PA8
#define BEEP_ON 	BEEP = 0
#define BEEP_OFF 	BEEP = 1

void beepInit(void);	//��ʼ��		 				    
#endif

