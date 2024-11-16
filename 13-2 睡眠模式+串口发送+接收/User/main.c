#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Serial.h"
u8 RxData;
int main(void)
{
	OLED_Init();
	Serial_Init();
	OLED_ShowString(1,1,"RxData:");
	while(1)
	
	{
	if(Serial_GetRxFlag()==1)
	{
		RxData = Serial_GetRxData();
		Serial_SendByte(RxData);
		OLED_ShowHexNum(1,8,RxData,2);
	}
		OLED_ShowString(2,1,"Running");
		Delay_ms(100);
		OLED_ShowString(2,1,"        ");
		Delay_ms(100);
	
	__WFI();//程序执行到此便会进入睡眠，等待下次唤醒。
	}
}



