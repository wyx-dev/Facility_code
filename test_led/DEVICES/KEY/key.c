#include "key.h"
#include "delay.h"
 	    
//������ʼ������ 
//PB 5 6 7 ���ó�����
void keyInit(void)
{
	RCC->APB2ENR|=1<<3;     //ʹ��PORTBʱ��
	//RCC->APB2ENR|=1<<4;     //ʹ��PORTCʱ��
	
	GPIOB->CRL&=0XF00FFFFF;	//PB5 6���ó�����	  
	GPIOB->CRL|=0X08800000;   
	
	GPIOB->ODR &= ~(3<<5);	   	//PB5 6���� 
}
/*
����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//����ֵ��
//0��û���κΰ�������
//KEY0_PRES��ADD����
//KEY1_PRES��SUB����
//KEY2_PRES��SET����
//ע��˺�������Ӧ���ȼ�,ADD>SUB>SET!!
*/
u8 keyScan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if((PBin(5)==1||PBin(6)==1) && key_up)
	{
		delay_ms(10);//ȥ����
		key_up=0;
		if(PBin(5)==1)return ADD;
		else if(PBin(6)==1)return SUB;
	}else if(PBin(5)==0&&PBin(6)==0) key_up=1;    
	return 0;// �ް�������
}

/* �жϰ�����ʼ������ */
void EXTI_Init(void)
{
	RCC->APB2ENR|=1<<3;     		//ʹ��PORTBʱ��
	GPIOB->CRL&=0X0FFFFFFF;			//PB 7���ó�����
	GPIOB->CRL|=0X80000000;
	GPIOB->ODR &= ~(1<<7);	   	//PB 7����

	Ex_NVIC_Config(GPIO_B,7,RTIR);		//�����ش���
	MY_NVIC_Init(2,1,EXTI9_5_IRQn,2);  	//��ռ2�������ȼ�1����2
}

void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);   		//����
  if(PBin(7) == 1)		//����SET
	{
		if(set == 1)
			set = 0;
		else
			set = 1;

	}
 	EXTI->PR=1<<7;			//���LINE5�ϵ��жϱ�־λ  
}




















