#ifndef __ADC_H
#define __ADC_H	
/**************************************************************************
���ߣ�Mini Balance 
�Ա����̣�http://shop114407458.taobao.com/
**************************************************************************/
#include <stm32f10x.h>
#include "sys.h"
int Get_Adc(u8 ch) ;

void Adc_Init(void);
void Get_battery_volt(void); 
float readAngle(void);  //��ȡ�Ƕȴ�������ֵ
 
#endif 















