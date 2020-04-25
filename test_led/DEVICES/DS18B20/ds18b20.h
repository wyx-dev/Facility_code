#ifndef __DS18B20_H
#define __DS18B20_H	 
#include "sys.h"   

//IO��������PA4
#define Ds18b20IO_IN()  {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL |= 0X00080000;}
#define Ds18b20IO_OUT() {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL |= 0X00030000;}

////IO��������											   
#define	Ds18b20DQ_OUT PAout(4) //���ݶ˿�	PA4 
#define	Ds18b20DQ_IN  PAin(4)  //���ݶ˿�	PA4 

void ds18b20Init(void);										//��ʼ��

/* ���¶Ⱥ���
	����ֵ���¶�ֵ
*/
int readTemp(void);	//���¶Ⱥ���	
#endif
