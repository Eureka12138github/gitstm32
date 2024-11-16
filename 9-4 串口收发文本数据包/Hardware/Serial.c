#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
char Serial_RxPacket[100];//给char用于接收字符
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
	9600波特率，8位字长，无校验，1位停止位，无流控，只有发送模式
	*/
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=9600;//设置波特率
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
	
	
//uint8_t Serial_GetRxFlag(void)//此函数用于判断是否接收到了数据包
//{
//	//此函数读取标志位后将标志位清零
//	if(Serial_RxFlag==1)
//	{
//		Serial_RxFlag=0;
//		return 1;
//	}
//	return 0;
//}

 /*
 * @brief USART1中断服务例程
 * 
 * 功能：接收固定格式的数据包
 * 
 * 1. 等待包头 '@'，进入接收状态。
 * 2. 逐字节接收数据包中的字符，直到遇到包尾 '\r'。
 * 3. 接收到包尾 '\r' 后，等待包尾 '\n'，完成一个数据包的接收。
 * 4. 设置接收完成标志 Serial_RxFlag。
 *
   * @param  无
   * @retval 无
   */
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;//函数进入只会初始化一次0，在函数推出后，数据仍然有效，此静态变量只能用于本函数
	//此处RxState为状态变量，与状态机编程有关
	static uint8_t pRxPacket = 0;//指示接收到哪一个变量
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET)
	{
		uint8_t RxData=USART_ReceiveData(USART1);//收取数据
		if(RxState == 0)
		{
			if(RxData == '@' && Serial_RxFlag == 0)
				/*
			此处Serial_RxFlag==0时，表示上一个数据已经接收完成了，
			此时可以接收下一个数据包，&&上Serial_RxFlag的意义在于
			防止数据包，特别是这种文本数据包发送过快时，导致的接收出错问题
			&&上Serial_RxFlag后，只有接收完上一个数据包后方可接收下一个
			数据包，即若文本数据包发送过快的话，会来不及接收，就被忽略了
			若数据包发送较慢，那其实没什么区别
			*/
			{
			RxState = 1;
			pRxPacket = 0;//进入到下一个状态之前，pRxPacket清零
			}
		}
		else  if(RxState == 1)
		{
			//因为载荷字符数量不确定，所以每次接收前，必须先判断是否为包尾（这里还不太理解240908）
			if(RxData == '\r')
			{
				RxState = 2;
			}
			else 
			{
			Serial_RxPacket[pRxPacket] = RxData;
			pRxPacket++;		
			}
		}
		else  if(RxState == 2)
		{
			if(RxData == '\n')
			{
			RxState=0;
			Serial_RxPacket[pRxPacket]='\0';//如此才是完整的字符串（这里不太懂240908）好像是字符串结尾自带一个\0
			Serial_RxFlag=1;//接收标志位置1，表示已接收完成
			}
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}
