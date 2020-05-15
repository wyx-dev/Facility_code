#include "seg.h"
#include "math.h"
#include "delay.h"
#include "ds18b20.h"
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
void seg4Display(int temp)
{
	unsigned char * pTableSeg; //���ָ��
	unsigned char data;
	unsigned char seg_i = 0;

	//int integer = (int )(temp*10);
	//��ʾ
	for(seg_i = 4; seg_i >0; --seg_i)
	{
		//Ѱ����ʾ����
		pTableSeg = SEG_0F + (int)((int)(temp/pow(10,(4-seg_i))) % 10);
		data = *pTableSeg;
		if(seg_i == 3)
			data &= ~(1<<7);
		
		
		//����������
		segOut(data);
		//Ƭѡ
		segOut(1<<(4 - seg_i));
		
		ST_CP = 0;
		ST_CP = 1;
		delay_ms(5);
	}
}


u8 seg_i = 4;
/* 5ms��ʱ���ж� */
void TIM3_IRQHandler(void)
{
	unsigned char * pTableSeg; //���ָ��
	unsigned char data;
	if(TIM1->SR&0X0001)//����ж�
	{
//		{
		//������20ms
		//seg4Display(currrentTemp);
		
		//����ÿ��40 * 25 ms = 1s
		//ʵ�ʲ���46 * �� ms = 1s
		//����ʵ�ʲ���23/46��s��500ms��ȡһ���¶� 
//		if(timCount == 23)
//		{
//			timCount = 0;
//			PCout(13) = !PCout(13);
//			currrentTemp = readTemp();
//		}
		pTableSeg = SEG_0F + (int)((int)(currentTemp/pow(10,(4-seg_i))) % 10);
		data = *pTableSeg;
		if(seg_i == 3)
			data &= ~(1<<7);

		//����������
		segOut(data);
		//Ƭѡ
		segOut(1<<(4 - seg_i));
		
		ST_CP = 0;
		ST_CP = 1;
		
		if(seg_i == 1)
		{
			seg_i = 4;
		}
		else
			seg_i --;
		
		//count��������һ Ϊ�������ṩ��׼��ʱ
		time_count ++;
	}
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}


/*����ܲ��Ժ���*/
void seg4Test(void)
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

