#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "seg.h"
#include "ds18b20.h"
#include "key.h"
#include "beep.h"
#include "timer.h"
#include "string.h"
#include "bt04.h"
#include "usart2.h"

/* 全局变量定义区 */
int targetTemp = 280;
int currentTemp = 0;
int alarmTemp = 400;
u8 set = 0, control_bluetooth = 0;
unsigned int time_count = 0;
int Kp = 72;
struct PID {
	unsigned int diff; // 设定目标与当前的差值 Desired Value
	unsigned int proportion; // 比例常数 ProporTIonal Const
	unsigned int integral; // 积分常数 Integral Const
	unsigned int derivative; // 微分常数 DerivaTIve Const
	unsigned int lastError; // Error［-1］
	unsigned int prevError; // Error［-2］
	unsigned int sumError; // Sums of Errors
}spid;


/* 函数定义区 */
void setTargetTemp(void);
void alarmCheck(int temp);
u16 PID_calc(struct PID *pp);
void PIDInit(struct PID *pp);

/*diy_wyx*/
#define FLAG_PROTUES 		0
void (*delay)(u16);
/*diy_wyx*/


int main(void)
{
	u16 heatOut = 0;
	u16 coldOut = 0;
	u16 coldDiff = 0;
	delay = delay_ms;
	#if FLAG_PROTUES
	delay = delay_diy;
	#endif

	Stm32_Clock_Init(9); 	//系统时钟设置
	delay_init(72);	     	//延时初始化
	uart_init(72, 9600);
	ledInit();		  	 		//初始化与LED连接的硬件接口
	segInit();						//初始化seg所用到的三个IO口
	ds18b20Init();			  //初始化ds18b20所用到的1个IO口
	keyInit();						//扫描按键IO初始化
	extiInit();						//中断按键初始化
	beepInit();						//初始化蜂鸣器
	while(BT04_Init()) 		//初始化BT04模块  
	{
		//出错指示灯闪烁
		LED0 = !LED0;
		delay_ms(500);
	}
	tim3Init(50,7199);		//10khz 计数到50
	tim1PwmInit(7199,0);	//10khz
	PIDInit(&spid);
	SEG_ON;

	spid.proportion = 36;
	spid.integral = 0;
	spid.derivative = 0;

	while(1)
	{
//		PWM_CH4_VAL = 2;//30--7.26   10---7.19  5---7.14  3---7.05 1--0 2--0
//		setPwm(7200,1);
//		/* 加入PID的系统 */
		if(set == 1)
		{
			BEEP_OFF;//关蜂鸣器
			PWM_OFF;
			setTargetTemp();
			/* 蓝牙发送相关 */
			u2_printf("A%.1f;%.1f\n",(float)targetTemp/10, (float)alarmTemp/10);
			delay_ms(500);
			u2_printf("BOFF");
			/* ************ */
		}

		//读取温度 并处理温度
		if((time_count % 3) == 0)
		{
//			LED0 = !LED0;
			currentTemp = readTemp();
			alarmCheck(currentTemp);

			//如果已到目标范围,则停止加热
			if((currentTemp > (targetTemp-5)) && ((currentTemp < (targetTemp+5))))
			{
				if((time_count % 100) == 0)
				{
					BEEP_ON;
					delay(50);
					BEEP_OFF;
					delay(50);
				}
			}

			//降温
			else if(currentTemp > targetTemp)
			{
				setPwm(0,1);//关闭加热器
				//降温
				coldOut = coldDiff * 12; //300 ---3600
				setPwm(coldOut ,0);
			}
			
			//升温
			else if(currentTemp < targetTemp)
			{
				setPwm(0,0);//关闭风扇
				spid.diff = targetTemp - currentTemp;
				if(spid.diff > 0)
					heatOut = spid.diff * spid.proportion;

				//PID数据处理
				else
				{
					heatOut = PID_calc(&spid);
				}

				setPwm(heatOut, 1);
			}
			
		}
		/* 加入PID的系统 */
	}	
}


/*
PID_calc函数功能：PID控制加热输出
输入参数：struct PID *  PID结构体参数；
输出：u16 加热输出PWM值；
*/
u16 PID_calc(struct PID *pp)
{ 
	u16 out = 0;
	u16 dError = 0;

	pp->sumError += pp->diff; //积分

	dError = pp->lastError - pp->prevError; //误差逐渐减小，起到抑制效果，所以上次减上上次为负
	
	//更新上上次误差和上次误差
	pp->prevError = pp->lastError;
	pp->lastError = pp->diff;

	out = (pp->proportion * pp->diff) + (pp->integral * pp->sumError) + (pp->derivative * dError);
	return out;
}


/* 
		设置目标/报警温度
		双击set设置报警温度
		单机set设置目标温度
*/
void setTargetTemp(void)
{
	
	int* setTemp;
	u8 flagSetType = 1;	//1表示目标温度，0表示报警温度
	u8 control_key = 0;
	
	//如果在500ms内连按set两次，就会进入设置报警温度
	delay_diy(5000);

	if(set == 0)
	{
		setTemp = &alarmTemp;
		set = 1;
		flagSetType = 0;
	}
	//否则设置目标温度
	else
	{
		setTemp = &targetTemp;
		flagSetType = 1;
		LED1 = 0;
	}

	while(1)
	{
		control_key = 0;
		control_bluetooth = 0;
		/* 蓝牙发送相关 */
		if(flagSetType == 0)//零 报警值
			u2_printf("BalarmTemp:%.1f",((float)*setTemp/10));
		else				//否则 目标值
			u2_printf("BtargetTemp:%.1f",((float)*setTemp/10));
		/* ************ */
		//等待按键按下
		while((control_key == 0) && (control_bluetooth == 0))
		{
			//显示目标温度，便于设置
			currentTemp = *setTemp;

			//如果设置的是报警模式，指示灯为闪烁。
			if(flagSetType == 0)
				LED1 = !LED1;
			delay(200);
			//set中断按下改变set变量值，退出设置模式，清除相关初始化变量
			if(set == 0)
			{
				//SEG_ON;
				LED1 = 1;
				set = 0;											//初始化set变量
				//alarmTemp = targetTemp + 100;	//默认报警温度为目标温度+10°。此条语句加上会出现alarm温度设置完没作用的效果,谨记!!!
				return;
			}

			//查询式 扫描按键（参数1代表支持连按）
			control_key = keyScan(1);
		}

		//判断按的那个按键，加或减目标温度
		if((control_key == ADD) || (control_bluetooth == ADD))
		{
			if(*setTemp < 65535)//限幅
				*setTemp += 5;
		}
		else if((control_key == SUB) || (control_bluetooth == SUB))
		{
			if(*setTemp > 0)		//限幅
				*setTemp -= 5;
		}

		//防误触
		//delay(200);
	}
}

/*
接口名称：void alarmCheck(int temp);
输入参数：temp(要检测的温度值)；
输出参数：无
*/
void alarmCheck(int temp)
{
	static u8 alarmCount = 0;

	//当超出目标温度一定时间才报警,相当于消抖
	(temp > alarmTemp) ? alarmCount++ : (alarmCount = 0);

	if(alarmCount == 5)
	{
		BEEP_ON;
		alarmCount = 0;
	}
}
void PIDInit(struct PID *pp)
{
	memset(pp, 0, sizeof(struct PID));
}
/*
汇编：队长：黄超 队员：王宇祥、刘定远、崔宇航、王晨
FPGA：队长：黄超 队员：孙逍遥、王宇祥、刘定远、江时盼
while(1)
{
	PAout(0) = 0;
	delay_ms(1000);
	PAout(0) = 1;
	delay_ms(1000);
}
*/
//while(1)
//	{
// 		delay_ms(10);	 
//		if(dir)led0pwmval++;
//		else led0pwmval--;	 
// 		if(led0pwmval>7199)dir=0;
//		if(led0pwmval==0)dir=1;	   					 
//		PWM_VAL=led0pwmval;
//		PWM_VAL = 7100;
//		delay(1500);
//		PWM_VAL = 000;
//		delay(1500);

//		PAout(3) = 1;
//		if(PBout(7))
//			PAout(3) = 1;
//		//delay(1000);
//		else
//			PAout(8) = 1;
//		delay(1000);
/* 数码管测试 */
//		seg4Test();
//		seg4Display(102);
/* 读取温度测试 */
//		currentTemp = readTemp();
//		seg4Display(currentTemp);
/* 按键测试 */
//		button = keyScan(0);
//		if(button != 0)
//			PCout(13) = ~PCout(13);
/* 蜂鸣器测试 */
//		BEEP_ON;
//		delay(500);
//		BEEP_OFF;
//		delay(500);
		
/* 定时器和PWM测试(PWM_VAL越大，输出电压越小) */
//		PWM_VAL = 7100;
//		delay(1500);
//		PWM_VAL = 1000;
//		delay(1500);
/* 蜂鸣器 按键 温度 数码管联合测试 */
//		if(set == 1)
//		{
//			BEEP_OFF;
//			setPwm(7199, 1);
//			setTargetTemp();
//		}

//		//读取温度 并处理温度
//		if((time_count % 3) == 0)
//		{
////			BEEP = !BEEP;
////			LED0 = !LED0;
//			currentTemp = readTemp();

//			//当前温度处于目标温度正负0.5，停止加热。蜂鸣器响。
//			if((currentTemp > (targetTemp-5)) && ((currentTemp < (targetTemp+5))))
//			{
//				if((time_count % 100) == 0)
//				{
//					BEEP_ON;
//					delay(50);
//					BEEP_OFF;
//					delay(50);
//				}
//			}

//			//小于目标温度，加热
//			if(currentTemp < (targetTemp-5))
//			{
//				moto = 7200 - (targetTemp - currentTemp) * Kp;
//			}

//			//大于目标温度，停止加热
//			else if(currentTemp < (targetTemp+5))
//			{
//				moto = 7199;
//			}
//			setPwm(moto, 1);
//		}

//		//检测是否报警
//		alarmCheck(currentTemp);
//		PAout(3) = 1;
//		delay(1000);
//		PAout(3) = 0;
//		delay(1000);
		//printf("running\r\n");
//	}
	
	

