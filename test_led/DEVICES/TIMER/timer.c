#include "timer.h"
#include "led.h"
 
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void tim3Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM3->PSC=psc;  	//Ԥ��Ƶ������
	TIM3->DIER|=1<<0;   //��������ж�				
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);//��ռ1�������ȼ�3����2									 
}
//TIM1_CH1 CH4 PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void tim1PwmInit(u16 arr,u16 psc)
{
	//�˲������ֶ��޸�IO������
	RCC->APB2ENR|=1<<11; 	//TIM1ʱ��ʹ��    
	GPIOA->CRH&=0XFFFF0FF0;	//PA8���֮ǰ������
	GPIOA->CRH|=0X0000B00B;	//���ù������ 
	
	TIM1->ARR=arr;			//�趨�������Զ���װֵ 
	TIM1->PSC=psc;			//Ԥ��Ƶ������
  
	TIM1->CCMR1|=6<<4;  	//CH1 PWM2ģʽ
	TIM1->CCMR1|=1<<3; 		//CH1Ԥװ��ʹ��	 
 	TIM1->CCER|=1<<0;   	//OC1 ���ʹ��	   

	TIM1->CCMR2|=6<<12;  	//CH4 PWM2ģʽ		 
	TIM1->CCMR2|=1<<11; 	//CH4Ԥװ��ʹ��	 
 	TIM1->CCER|=1<<12;   	//OC4 ���ʹ��	 

	TIM1->BDTR|=1<<15;   	//MOE �����ʹ��	   

	TIM1->CR1=0x0080;   	//ARPEʹ�� 
	TIM1->CR1|=0x01;    	//ʹ�ܶ�ʱ��1 										  
}  

//flag�����մɼ���Ƭ�͵����. 1 Ϊ�մɼ���Ƭ 0 �����
void setPwm(int moto, u8 flag)
{
	int tmp = 0;

	//�����մɼ���Ƭ�͵����. 1 Ϊ�մɼ���Ƭ 0 �����
	(flag==1)? (tmp = 7199) : (tmp = 30);
	
	//�޷�
	if(moto > tmp)
	{
		moto = tmp;
	}
	if(moto < 0)
	{
		moto = 0;
	}

	//��ֵ���Ĵ���
	(flag==1)? (PWM_CH1_VAL = moto) : (PWM_CH4_VAL = moto);
		
}












