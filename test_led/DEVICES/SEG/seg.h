#ifndef __SEG_H
#define __SEG_H	 
#include "sys.h"   

//SEG�˿ڶ���
#define SH_CP 	PAout(5)	// PA5
#define DS 			PAout(6)	// PA6	
#define ST_CP 	PAout(7)	// PA7

void segInit(void);	//��ʼ��		
void seg4Display(float temp); //seg��ʾ
void seg4Test(void);//seg���Ժ���
void segOut(unsigned char X); //seg 8λ�������

#endif
