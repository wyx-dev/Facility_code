#include "delay.h" 			 
#include "usart.h" 			 
#include "usart2.h" 			 
#include "bt04.h" 
#include "led.h" 
#include "string.h"	 
#include "math.h"

//��ʼ��ATK-BT04ģ��
//����ֵ:0,�ɹ�;1,ʧ��.
u8 BT04_Init(void)
{
	USART2_Init(36,9600);	//��ʼ������2Ϊ:9600,������
	return 0;
}
