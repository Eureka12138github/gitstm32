#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
uint8_t Serial_RxData;
uint8_t Serial_RxFlag;
void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//开启USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启GPIOA时钟，因为需要用到PA9与PA10
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	/*
	下面是把PA9配置为复用推挽输出，供USART1的TX使用
	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//对于输入可以选用浮空输入或上拉输入模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	/*
	下面串口配置情况：
	9600波特率，8位字长，无校验，1位停止位，无流控，
	*/
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=115200;//设置波特率
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//同时开启发送与接收
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//此为开启RXNE标志位到NVIC的输出
	/*
	一旦RXNE标志位一旦置1了，就会向NVIC申请中断，之后可以在中断函数里接收数据
	*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	//使能串口
	USART_Cmd(USART1,ENABLE);
	
}
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);//此函数将Byte变量写入到TDR
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);//等待TDR的数据转移到移位寄存器
}
void Serial_SendArray(uint8_t*Array,uint16_t Length)
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
	Serial_SendByte(Array[i]);//发送一个数组
	}
}
void Serial_SendString(char*String)//字符串自带结束标志位故无需传递长度参数
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		Serial_SendByte(String[i]);//将String字符串一个个取出，通过SendByte发送
	}
}

uint32_t Serial_Pow(uint32_t x,uint32_t y)
{
	uint32_t Result=1;
	while(y--)
	{
		Result*=x;
	}
	return Result;
}
void Serial_SendNum(uint32_t Num,uint8_t Length)
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Num/Serial_Pow(10,Length-i-1)%10+0x30);//拆解一个确定位数的整数并一一发送，0x30是偏移，由ASCII码表可得
	}
	
}
int fputc(int ch,FILE*f)
{
	Serial_SendByte(ch);//将fput重定向
	return ch;
}

void Serial_Printf(char *format,...)//可变参数的用法，其他函数若需用到可来此参考
{
	char String[100];
	va_list arg;//定义一个参数列表变量
	va_start(arg,format);//从format位置开始接收参数表，放在arg里面
	vsprintf(String,format,arg);//sprinf只能接收直接写的参数，对于封装格式，要用vsprintf
	va_end(arg);//释放参数列表
	Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void)//此为自定义标志位获取函数
{
	if(Serial_RxFlag==1)
	{
		Serial_RxFlag=0;
		return 1;
	}
	return 0;
}
uint8_t Serial_GetRxData(void)//调取此函数便返回串口获取到的数值
{
	return Serial_RxData;
}
void USART1_IRQHandler(void)//这是中断程序，一旦串口有数据来，就给 Serial_RxData 赋新值，相当于通过中断刷新Serial_RxData
{
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
	{
		Serial_RxData=USART_ReceiveData(USART1);
		Serial_RxFlag=1;//此为自定义标志位，这里置一表示已收取完数据
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}
