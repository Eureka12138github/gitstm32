#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "MyRTC.h"
int main(void)
{
	/*
		这节课实现的是使单片机进入待机模式，
		可以通过RTC每隔10唤醒一次单片机，也
		可以通过将PA0手动置高电平，产生上升
		沿唤醒单片机。但注意，每次唤醒都是从
		头开始执行的。要想设备极致省电，光进
		入待机模式还不够，应该还要把能关掉的
		外设给关了。具体实施还得继续研究，
		没那么简单。
		
	*/
	OLED_Init();
	MyRTC_Init();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	//上面的时钟开启实际上在MyRTC_Init中已经开启了，现在在这里再开启是为了减少代码间的耦合性
	OLED_ShowString(0,0,"CNT:",OLED_8X16);
	OLED_ShowString(0,15,"ALR:",OLED_8X16);
	OLED_ShowString(0,30,"ALRF",OLED_8X16);
	PWR_WakeUpPinCmd(ENABLE);//引脚唤醒功能PA0
	u32 Alarm = RTC_GetCounter()+10;//因为Alarm是只写的
	RTC_SetAlarm(Alarm);//设定闹钟，秒数加10秒，定义闹钟为十秒后
	OLED_ShowNum(35,15,Alarm,10,OLED_8X16);
	OLED_Update();
//	MyRTC_SetTime();
	while(1)
	{
		MyRTC_ReadTime();
		
		OLED_ShowNum(35,0,RTC_GetCounter(),10,OLED_8X16);
		OLED_ShowNum(35,30,RTC_GetFlagStatus(RTC_FLAG_ALR),1,OLED_8X16);
		OLED_Update();
		OLED_ShowString(0,45,"Running",OLED_8X16);
		Delay_ms(100);
		OLED_Update();
		OLED_ShowString(0,45,"        ",OLED_8X16);
		Delay_ms(100);
		OLED_Update();
		OLED_ShowString(50,45,"STANDBY",OLED_8X16);
		Delay_ms(1000);
		OLED_Update();
		OLED_ShowString(50,45,"        ",OLED_8X16);
		Delay_ms(100);
		OLED_Clear();
		OLED_Update();
		PWR_ClearFlag(PWR_FLAG_WU);//这句是看弹幕加的，原理未知，但是不加这句就不可手动唤醒
		PWR_EnterSTANDBYMode();//进入待机模式,该模式唤醒后是从头开始运行的，这意味着如果还有语句的话将不会执行了。此处把while循环去掉都行
		
		//16分
	}
}


