#include "seg.h"
void segInit(void)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��	   	 

	GPIOA->CRL&=0X000FFFFF; 
	GPIOA->CRL|=0X33300000;//PA5 6 7 �������   	

	GPIOA->ODR|=1<<5;      //PA5 �����
	GPIOA->ODR|=1<<6;      //PA6 �����	
	GPIOA->ODR|=1<<7;      //PA7 �����
}


//7���������ʾ��
unsigned char SEG_0F[] = 
{// 0	 1	  2	   3	4	 5	  6	   7	8	 9	  A	   b	C    d	  E    F    -
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0xBF,0xC6,0xA1,0x86,0xFF,0xbf
};

/*�������ʾ����*/
void seg4Display(float temp)
{
	unsigned char * pTableSeg; //���ָ��
	unsigned char data;
	unsigned char seg_i = 0;
	//��ʾ
	for(seg_i = 1; seg_i < 5; ++seg_i)
	{
		//Ѱ����ʾ����
		pTableSeg = SEG_0F + seg_i;
		data = *pTableSeg;
		//����������
		segOut(data);
		//Ƭѡ
		segOut(1<<(seg_i - 1));
		
		ST_CP = 0;
		ST_CP = 1;
	}
}

/*������������*/
void segOut(unsigned char X)
{
	unsigned char i;
	for(i=8;i>=1;i--)
	{
		if (X&0x80) 
			DS=1; 
		else 
			DS=0;
		X<<=1;
		SH_CP = 0;
		SH_CP = 1;
	}
}
