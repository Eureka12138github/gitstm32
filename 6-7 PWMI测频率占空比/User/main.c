#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"
int main(void)
{
	int_least8_t i;
	IC_Init();
	OLED_Init();	
	PWM_Init();
	OLED_ShowString(1,1,"Freq:00000Hz");
	OLED_ShowString(2,1,"Duty:00%");
	PWM_SetPrescaler(720-1);//Freq=72M/(PSC+1)/(ARR+1)
	PWM_SetCompare1(50);//Duty=CCR/(ARR+1)
	/*
	不知为何，按理说改变CCR不会影响到输出的频率，但是当CCR输入大于等于100这样的非法值时，OLED屏上显示的频率
	统一变为22hz，占空比变成66%
	之后打算将两个PWM函数合并一下，使得调用一个函数便可设置PWM的频率（周期）与占空比
	
	*/
	
	while(1)
	{
	OLED_ShowNum(1,6,IC_GetFreq(),5);
	OLED_ShowNum(2,6,IC_GetDuty(),2);	
	}
}
