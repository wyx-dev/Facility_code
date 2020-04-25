#include "ds18b20.h"
#include "usart.h"
#include "delay.h"
u8 signFlag = 1;			//�¶�������־λ

/* ����ʱ����ʱ���� */
void delay_time(unsigned char i)
{
	for(; i > 0; i--);
}

/* ds18b20��λ */
void ds18b20Reset(void)
{
	Ds18b20IO_OUT(); //SET PA0 OUTPUT
	Ds18b20DQ_OUT=1; 
	delay_us(50);
	Ds18b20DQ_OUT=0; //����DQ
	delay_us(500);    //����500us
	Ds18b20DQ_OUT=1; //DQ=1 
	delay_us(40);     //15US
}


/* ds18b20��ʼ�� */
void ds18b20Init(void)
{
	RCC->APB2ENR |= 1<<2;    //ʹ��PORTAʱ��	   	 

	GPIOA->CRL &= 0XFFF0FFFF; 
	GPIOA->CRL |= 0X00030000;//PA4������� 50Mhz
	
	GPIOA->ODR |= 1<<4;
	
	ds18b20Reset();
}


//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 ds18b20ReadBit(void) 			 // read one bit
{
    u8 data;
    Ds18b20IO_OUT();//SET PA0 OUTPUT
    Ds18b20DQ_OUT=0; 
    delay_us(2);
    Ds18b20DQ_OUT=1; 
    Ds18b20IO_IN();//SET PA0 INPUT
    delay_us(12);
    if(Ds18b20DQ_IN)data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}


/* ��һ���ֽ� */
unsigned char readOneChar(void)  		//�����������ȴӸ������͵�ƽ1us���ϣ���ʹ��������Ϊ�ߵ�ƽ���Ӷ��������ź�
{
	u8 i,j,dat;
    dat=0;
    for (i=1;i<=8;i++) 
    {
        j=ds18b20ReadBit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}


/* дһ���ֽ� */
void writeOneChar(unsigned char dat) 
{ 
	u8 j;
	u8 testb = 0;
	Ds18b20IO_OUT();//SET PA0 OUTPUT;
	for (j = 1; j <= 8; j++) 
	{
		testb = dat & 0x01;
		dat = dat >> 1;
		if (testb) 
		{
				Ds18b20DQ_OUT=0;// Write 1
				delay_us(2);                            
				Ds18b20DQ_OUT=1;
				delay_us(60);             
		}
		else 
		{
				Ds18b20DQ_OUT=0;// Write 0
				delay_us(60);             
				Ds18b20DQ_OUT=1;
				delay_us(2);                          
		}
	}
}

/*
�ȴ�DS18B20�Ļ�Ӧ
����1:δ��⵽DS18B20�Ĵ���
����0:����
*/
u8 ds18b20Check(void) 	   
{   
    u8 retry=0;
    Ds18b20IO_IN();//SET PA0 INPUT	 
    while (Ds18b20DQ_IN&&retry<200)
    {
        retry++;
        delay_us(1);
    }
    if(retry>=200)return 1;
    else retry=0;
    while (!Ds18b20DQ_IN&&retry<240)
    {
        retry++;
        delay_us(1);
    }
    if(retry>=240)return 1;	    
    return 0;
}

/* �¶�ת������ */						//ÿ��ȡһ�ζ���Ҫת��һ���¶ȣ�����ͨ�Ĳ�һ��
void ds18b20Start(void)
{
	ds18b20Reset();		//��λ
	
	ds18b20Check();
	
	writeOneChar(0xcc); 				//���������кŵĲ���
	writeOneChar(0x44); 				//�����¶�ת��
}


/* ���¶Ⱥ���
	integer���������� 
	decimal: С������
*/

int readTemp(void)
{
	u8 temp;
	u8 TL,TH;
	int tem;
	ds18b20Start ();                    // ds1820 start convert
	ds18b20Reset();
	ds18b20Check();	 
	writeOneChar(0xcc);// skip rom
	writeOneChar(0xbe);// convert	    
	TL = readOneChar(); // LSB   
	TH = readOneChar(); // MSB  
	
	if(TH > 7)
	{
		//�¶�Ϊ����ȡ����
		TH = ~TH;
		TL = ~TL; 
		temp = 0;//�¶�Ϊ��  
	}else temp = 1;//�¶�Ϊ��	  	  
	tem = TH; //��ø߰�λ
	tem <<= 8;    
	tem += TL;//��õװ�λ
	tem = (int)((float)tem*0.625);//ת�� 0.625 = 5 / 8   ����16��10
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
}
