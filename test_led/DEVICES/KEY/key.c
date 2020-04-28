#include "key.h"
#include "delay.h"
 	    
//������ʼ������ 
//PB 5 6 7 ���ó�����
void KEY_Init(void)
{
	RCC->APB2ENR|=1<<3;     //ʹ��PORTBʱ��
	//RCC->APB2ENR|=1<<4;     //ʹ��PORTCʱ��
	
	GPIOB->CRL&=0X000FFFFF;	//PB 5 6 7���ó�����	  
	GPIOB->CRL|=0X88800000;   
	
	GPIOB->ODR|=7<<5;	   	//PB 5 6 7���� 
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
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(PBin(5)==0||PBin(6)==0||PBin(7)==0)
	{
		delay_ms(10);//ȥ����
		key_up=0;
		if(PBin(5)==0)return ADD;
		else if(PBin(6)==0)return SUB;
		else if(PBin(7)==0)return SET;
	}else if(PBin(5)==1&&PBin(6)==1&&PBin(7)==1) key_up=1;    
	return 0;// �ް�������
}




















