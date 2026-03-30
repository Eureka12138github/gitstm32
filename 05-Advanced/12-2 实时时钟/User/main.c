#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "MyRTC.h"
int main(void)
{
	OLED_Init();
	MyRTC_Init();
	OLED_ShowString(0,0,"Date:    -  -  ",OLED_8X16);
	OLED_ShowString(0,15,"Time:  :  :  ",OLED_8X16);
	OLED_ShowString(0,35,"CNT:",OLED_8X16);
	OLED_ShowString(0,50,"DIV:",OLED_8X16);
//	MyRTC_SetTime();
	while(1)
	{
		MyRTC_ReadTime();
		//看到43分，现在日期获取失败！
		/*
			问题找到了，真是坑爹的，我把MyRTC_ReadTime及MyRTC_SetTime函数中的
			time_date.tm_year 写成 time_date.tm_yday 了！！！
			小小研究了一下，tm_yday似乎是离1月1号的天数，比如1月15号tm_yday应该是15？
			验证了一下，是的。
		*/
		OLED_ShowNum(40,0,MyRTC_Time[0],4,OLED_8X16);
		OLED_ShowNum(81,0,MyRTC_Time[1],2,OLED_8X16);
		OLED_ShowNum(106,0,MyRTC_Time[2],2,OLED_8X16);
		
		OLED_ShowNum(40,15,MyRTC_Time[3],2,OLED_8X16);
		OLED_ShowNum(65,15,MyRTC_Time[4],2,OLED_8X16);
		OLED_ShowNum(87,15,MyRTC_Time[5],2,OLED_8X16);
		
		OLED_ShowNum(32,35,RTC_GetCounter(),10,OLED_8X16);
		OLED_ShowNum(32,50,(32768-RTC_GetDivider())/32767.0*999,10,OLED_8X16);//0~32767->0~999
		OLED_Update();
		//16分
	}
}


