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
#include "DataScope_DP.h"

/* 全局变量定义区 */
int targetTemp = 400;
int currentTemp = 0;
int alarmTemp = 300;
u8 set = 0, control_bluetooth = 0, flag_seeTemp = 0;
unsigned int time_count = 0;
struct PID {
	float diff; // 设定目标与当前的差值 Desired Value
	int proportion; // 比例常数 ProporTIonal Const
	int integral; // 积分常数 Integral Const
	int derivative; // 微分常数 DerivaTIve Const
	int lastError; // Error［-1］
	int prevError; // Error［-2］
	float sumError; // Sums of Errors
}spid;


/* 函数定义区 */
void setTargetTemp(void);
u8 alarmCheck(int temp);
int PID_calc(struct PID *pp);
void PIDInit(struct PID *pp);

/*diy_wyx*/
#define FLAG_PROTUES 		0
#define BLUETOOTH			1
#define DEBUG				0
void (*delay)(u16);
/*diy_wyx*/

int main(void)
{
	int heatOut = 0, temp = 0;
	u16 coldOut = 0;
	u16 coldDiff = 0;
	u8 i = 0, Send_Count = 0, normal = 0, n = 0, alarmFlag = 0;
	
	delay = delay_ms;
	#if FLAG_PROTUES
	delay = delay_diy;
	#endif

	Stm32_Clock_Init(9); 	//系统时钟设置
	delay_init(72);	     	//延时初始化
	uart_init(72, 128000);
	ledInit();		  	 		//初始化与LED连接的硬件接口
	segInit();						//初始化seg所用到的三个IO口
	ds18b20Init();			  //初始化ds18b20所用到的1个IO口
	keyInit();						//扫描按键IO初始化
	extiInit();						//中断按键初始化
	beepInit();						//初始化蜂鸣器
	#if BLUETOOTH
		while(BT04_Init()) 		//初始化BT04模块  
		{
			//出错指示灯闪烁
			LED0 = !LED0;
			delay_ms(500);
		}
	#endif
	tim3Init(50,7199);		//10khz 计数到50
	tim1PwmInit(7199,0);	//10khz
	PIDInit(&spid);
	SEG_ON;
	currentTemp = readTemp();
		
	spid.proportion = 30;//晚上35
	spid.integral = 1;
	spid.derivative = 500;

#if DEBUG
	while(1)
	{
		setPwm(7200,1);
		delay(1000);
		setPwm(6000,1);
		delay(1000);
	}
#endif

	while(1)
	{
//		PWM_CH4_VAL = 2;//30--7.26   10---7.19  5---7.14  3---7.05 1--0 2--0
//		setPwm(7200,1);
//		/* 加入PID的系统 */
		if((time_count % 100) == 0)
		{
			//判断是否进入设置模式
			if(set == 1)
			{
				BEEP_OFF;//关蜂鸣器
				PWM_OFF;
				setTargetTemp();
				/* 蓝牙发送相关 */
				#if BLUETOOTH
					u2_printf("A%.1f;%.1f\n",(float)targetTemp/10, (float)alarmTemp/10);
					delay_ms(50);
					u2_printf("BOFF");
					delay_ms(20);
				#endif
				/* ************ */
				spid.sumError = 0;//初始化累计误差
				normal = 0;//初始化正常工作标志符
				currentTemp = readTemp();//初始化正常工作温度值, 不初始化的话,滤波过不了
			}

			/* 蓝牙发送相关 */
				#if BLUETOOTH
					u2_printf("C%.1f",(float)currentTemp/10);
				#endif
			/* ************ */

			//检测报警
			alarmFlag = alarmCheck(currentTemp);
		}
		//读取温度 并处理温度
		if((time_count % 10) == 0)
		{
			//在到达目标温度范围内,按下ADD键即正常工作
			if(keyScan(0) == ADD)
			{
				normal = 1;
				BEEP_OFF;
			}

			//读取温度,并滤波
			n = 0;//此变量可防止温差突变导致卡死的现象.
			while(n++ < 10)
			{
				temp = readTemp();
				
				//当差小于10(即1℃时),取温度
				if(((temp - currentTemp) < 5) && ((temp - currentTemp) > -5))
					break;
//				currentTemp = temp;
//				delay_ms(100);
			}
			currentTemp = temp;//取温度

			//串口调试
			
			DataScope_Get_Channel_Data(currentTemp,1);
			
			Send_Count = DataScope_Data_Generate(3);
			for( i = 0 ; i < Send_Count; i++) 
			{
				while((USART1->SR&0X40)==0);
				USART1->DR = DataScope_OutPut_Buffer[i]; 
			}

			//如果已到目标范围,则停止加热
			if((currentTemp > (targetTemp-5)) && (currentTemp < (targetTemp+5)))
			{
				heatOut = 0;
				setPwm(heatOut,1);//关闭加热器
				
				//提示音
				if((time_count % 50) == 0)
				{
					/* 蓝牙发送相关 */
					#if BLUETOOTH
						u2_printf("2");
					#endif
					/* ************ */
						
					//非正常工作时,蜂鸣器响.
					if(normal == 0)
						BEEP = !BEEP;
					else
						BEEP_OFF;
				}
				
			}

			//未到目标温度,调整温度
			else
			{
				//降温
				if(currentTemp > targetTemp)
				{
					//关闭加热器
					heatOut = 0;
					setPwm(heatOut,1);

					//打开风扇
					coldOut = coldDiff * 12; //300 ---3600 
					setPwm(coldOut ,0);
				}
				//升温
				else if((currentTemp < targetTemp) && (alarmFlag == 0))
				{
					BEEP_OFF;//关蜂鸣器
					setPwm(0,0);//关闭风扇
					spid.diff = targetTemp - currentTemp;//做差

					//6℃之外之开启比例调节
					if(spid.diff > 60)
						heatOut = spid.diff * spid.proportion;

					//PID数据处理
					else
					{
						heatOut = PID_calc(&spid);
					}

					setPwm(heatOut, 1);
				}
				/* 蓝牙发送相关 */
				if((time_count % 110) == 0)
				{
					#if BLUETOOTH
						u2_printf("3");
					#endif
				}
				/* ************ */
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
int PID_calc(struct PID *pp)
{ 
	int  outp = 0, outi = 0;
	float out = 0, outd = 0;
	int dError = 0;
	static u8 lastErrorCount = 0;
	
	pp->sumError += pp->diff * 0.045; //积分

	if(pp->sumError > 1500)
		pp->sumError = 1500;
	if(pp->sumError < 0)
		pp->sumError = 0;
	
	dError = pp->lastError - pp->prevError; //误差逐渐减小，起到抑制效果，所以上次减上上次为负

	//更新上上次误差和上次误差
	
	if(lastErrorCount++ % 20== 0)//20 d调节没变化,看看200如何!!!!
	{
		pp->prevError = pp->lastError;
		pp->lastError = pp->diff;
	}
	outp = pp->proportion * pp->diff;
	outi = pp->integral * pp->sumError;
	DataScope_Get_Channel_Data(outi, 3);
	outd = pp->derivative * dError;
	DataScope_Get_Channel_Data(outd, 2);
	
	out = outp + outi + outd;
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
		#if BLUETOOTH
			if(flagSetType == 0)//零 报警值
				u2_printf("BalarmTemp:%.1f",((float)*setTemp/10));
			else				//否则 目标值
				u2_printf("BtargetTemp:%.1f",((float)*setTemp/10));
		#endif
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
u8 alarmCheck(int temp)
{
	static u8 alarmCount = 0;

	//当超出目标温度一定时间才报警,相当于消抖
	(temp > alarmTemp) ? alarmCount++ : (alarmCount = 0);

	//报警
	if(alarmCount > 2)
	{
		PWM_OFF;
		#if BLUETOOTH
		delay_ms(30);
		u2_printf("1");
		delay_ms(30);
		#endif
		BEEP_ON;
		alarmCount = 2;
		return 1;
	}
	else if(temp <= alarmTemp){
		#if BLUETOOTH
		delay_ms(30);
		u2_printf("0");
		delay_ms(30);
		#endif
		return 0;
	}
	return 0;
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
	
	

