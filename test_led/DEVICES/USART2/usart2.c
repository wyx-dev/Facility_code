#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
#include "usart.h"
#include "key.h"

//串口发送缓存区 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
#ifdef USART2_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	

/*
函数功能:判断即将接受的一系列res字符是否符合"SXM***Q"(可以自定协议)协议,
		 如果符合,仅将数据内容存到长度为len,起始指针为buf的缓冲区中,具有可排除一切不符合的强大功能.待测试
输入参数:res：将检测的字符串的一个字符
		 buf：缓冲区
		 len：缓冲区长度
*/
u8 dataExec(char res, char* buf, int len)
{
	static u16 buf_i=0; 	//缓冲区指示针
	static u8 state = 0;
	static char USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
	//符合协议SXM******Q
	if((state == 3) && (buf_i < USART2_MAX_RECV_LEN))
	{
		//结束标志
		if(res == 'Q')
		{
			buf_i = 0;
			snprintf(buf,len,"%s",USART2_RX_BUF);
			memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
			state = 0;
			return 1;
		}
		//数据内容
		else
			USART2_RX_BUF[buf_i++]=res;		//记录接收到的值
	}
	else
	{
		//状态机判断数据头  如需需修改协议改此switch和上面的Q
		switch(res)		//还可以接收数据
		{
			case 'S': 
				if(state == 0)
					state++;
				else
					state = 0;
				break;
			case 'X': 
				if(state == 1)
					state++;
				else
					state = 0;
				break;
			case 'M': 
				if(state == 2)
					state++;
				else
					state = 0;
				break;
			default:
				state = 0;
				break;
		}	
	}
	return 0;
}


void USART2_IRQHandler(void)
{
	u8 res;	 			//接收字符缓冲区
	static char buf[USART2_MAX_RECV_LEN];
	if(USART2->SR&(1<<5))//接收到数据
	{
		res=USART2->DR;
		//当接受完整数据包时
		if(dataExec(res, buf, sizeof(buf)) == 1)
		{
			//SET按键
			if(strcmp((const char*)buf,"SET")==0)
				set = !set;
			if(strcmp((const char*)buf,"ADD")==0)
				control_bluetooth = ADD;
			if(strcmp((const char*)buf,"SUB")==0)
				control_bluetooth = SUB;
			
//			switch(buf)
//			{
//				case "SET"://set按键
//					set = !set;
//					break;
//			}
		}
	}
}   
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void USART2_Init(u32 pclk1,u32 bound)
{  	 		 
	RCC->APB2ENR|=1<<2;   	//使能PORTA口时钟  
	GPIOA->CRL&=0XFFFF00FF;	//IO状态设置
	GPIOA->CRL|=0X00008B00;	//IO状态设置	 
	RCC->APB1ENR|=1<<17;  	//使能串口时钟 	 
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);//停止复位	   	   
	//波特率设置
 	USART2->BRR=(pclk1*1000000)/(bound);// 波特率设置	 
	USART2->CR1|=0X200C;  	//1位停止,无校验位.
	USART2->CR3=1<<7;   	//使能串口2的DMA发送
	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF);//DMA1通道7,外设为串口2,存储器为USART2_TX_BUF 
#ifdef USART2_RX_EN		  	//如果使能了接收
	//使能接收中断
	USART2->CR1|=1<<8;    	//PE中断使能
	USART2->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(2,3,USART2_IRQn,2);//组2，最低优先级 
//	TIM4_Init(99,7199);		//10ms中断
//	TIM4_Set(0);			//关闭定时器4
#endif										  	
}
//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char* fmt,...)  
{  
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
	UART_DMA_Enable(DMA1_Channel7,strlen((const char*)USART2_TX_BUF)); 	//通过dma发送出去
}
//定时器4中断服务程序		    
void TIM4_IRQHandler(void)
{ 	
	if(TIM4->SR&0X01)//是更新中断
	{	 			   
		//USART2_RX_STA|=1<<15;	//标记接收完成
		TIM4->SR&=~(1<<0);		//清除中断标志位		   
		TIM4_Set(0);			//关闭TIM4  
	}	    
}
//设置TIM4的开关
//sta:0，关闭;1,开启;
void TIM4_Set(u8 sta)
{
	if(sta)
	{
    	TIM4->CNT=0;         //计数器清空
		TIM4->CR1|=1<<0;     //使能定时器4
	}else TIM4->CR1&=~(1<<0);//关闭定时器4	   
}
//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM4_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4时钟使能    
 	TIM4->ARR=arr;  	//设定计数器自动重装值   
	TIM4->PSC=psc;  	//预分频器
 	TIM4->DIER|=1<<0;   //允许更新中断				
 	TIM4->CR1|=0x01;  	//使能定时器4	  	   
   	MY_NVIC_Init(1,3,TIM4_IRQn,2);//抢占2，子优先级3，组2	在2中优先级最低								 
}
#endif		 
///////////////////////////////////////USART2 DMA发送配置部分//////////////////////////////////	   		    
//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址    
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
{
 	RCC->AHBENR|=1<<0;			//开启DMA1时钟
	delay_us(5);
	DMA_CHx->CPAR=cpar; 		//DMA1 外设地址 
	DMA_CHx->CMAR=cmar; 		//DMA1,存储器地址	 
	DMA_CHx->CCR=0X00000000;	//复位
	DMA_CHx->CCR|=1<<4;  		//从存储器读
	DMA_CHx->CCR|=0<<5;  		//普通模式
	DMA_CHx->CCR|=0<<6;  		//外设地址非增量模式
	DMA_CHx->CCR|=1<<7;  		//存储器增量模式
	DMA_CHx->CCR|=0<<8;  		//外设数据宽度为8位
	DMA_CHx->CCR|=0<<10; 		//存储器数据宽度8位
	DMA_CHx->CCR|=1<<12; 		//中等优先级
	DMA_CHx->CCR|=0<<14; 		//非存储器到存储器模式		  	
} 
//开启一次DMA传输
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 len)
{
	DMA_CHx->CCR&=~(1<<0);       //关闭DMA传输 
	DMA_CHx->CNDTR=len;          //DMA1,传输数据量 
	DMA_CHx->CCR|=1<<0;          //开启DMA传输
}	   
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 





















