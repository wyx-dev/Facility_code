#include "key.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//�������� ��������		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/06
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////	 
 	    
//������ʼ������ 
//PA0.15��PC5 ���ó�����
void KEY_Init(void)
{
	RCC->APB2ENR|=1<<3;     //ʹ��PORTBʱ��
	//RCC->APB2ENR|=1<<4;     //ʹ��PORTCʱ��
	JTAG_Set(SWD_ENABLE);	//�ر�JTAG,����SWD
	GPIOB->CRL&=0XFFFFFF00;	//PA0���ó�����	  
	GPIOB->CRL|=0X00000088;   
	//GPIOA->CRH&=0X0FFFFFFF;	//PA15���ó�����	  
	//GPIOA->CRH|=0X80000000; 			 
	//GPIOB->ODR|=1<<15;	   	//PA15����,PA0Ĭ������
	//GPIOC->CRL&=0XFF0FFFFF;	//PC5���ó�����	  
	//GPIOC->CRL|=0X00800000;   
	GPIOB->ODR|=1<<0;	   	//PC5���� 
	GPIOB->ODR|=1<<1;	   	//PC5���� 
} 
//������������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//����ֵ��
//0��û���κΰ�������
//KEY0_PRES��KEY0����
//KEY1_PRES��KEY1����
//WKUP_PRES��WK_UP���� 
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(KEY0==0||KEY1==0)
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES; 
	}else if(KEY0==1&&KEY1==1) key_up=1; 	     
	return 0;// �ް�������
}



















