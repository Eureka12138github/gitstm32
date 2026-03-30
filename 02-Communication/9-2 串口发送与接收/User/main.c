#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Serial.h"
int main(void)
{
	OLED_Init();
	Serial_Init();
	LED_Init();
	uint8_t RxData=0;
//	OLED_ShowString(1,1,"RxData:");
	OLED_ShowString(1,1,"BT_Test1:");
//	Serial_Printf("TEST");
//	Serial_Printf("你好世界\r\n");
	char String[100];//定义字符串
	sprintf(String,"Hello,World!\r\n");//打印字符串
	Serial_SendString(String);//发送字符串
	while(1)
	
	{
	
	if(Serial_GetRxFlag()==1)
	{
	//标志位会自动清零
	RxData=Serial_GetRxData();
//	Serial_SendByte(RxData);//把接收到的数据回传给电脑
	if(RxData == 0x00) 
	{
		LED1_OFF();
		OLED_ShowString(2,1,"LED_OFF_OK");
	}
	else if(RxData == 0x01)
	{
		LED1_ON();
		OLED_ShowString(2,1,"               ");
		OLED_ShowString(2,1,"LED_ON_OK");
	}
//	OLED_ShowHexNum(1,8,RxData,2);	
	}
	OLED_ShowHexNum(4,1,RxData,2);	
	}
}


/*	查询方式接收串口数据(适用于简单程序)：
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
	{
	//标志位会自动清零
	RxData=USART_ReceiveData(USART1);
	OLED_ShowHexNum(1,1,RxData,2);	
	}
*/
