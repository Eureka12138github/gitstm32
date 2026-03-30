#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
uint8_t KeyNum,min,sec,minisec,runflag=1;
int main(void)
{

	OLED_Init();
	OLED_ShowString(0,0,"SYSCLK:",OLED_8X16);
	OLED_ShowNum(55,0,SystemCoreClock,8,OLED_8X16);//SystemCoreClock是系统当前的主频
	OLED_Update();
	while(1)
	{
		/*
			修改主频办法：在system_stm32f10x.c文件里面的第112行附近，解除
		相应的注释即可，一般不改主频，这是牵一发动全身的事情，要进行其他的
		对应配置，否则会不准确。
		*/
		OLED_ShowString(0,20,"RUNNING",OLED_8X16);
		OLED_Update();
		Delay_ms(500);
		OLED_ShowString(0,20,"          ",OLED_8X16);
		OLED_Update();
		Delay_ms(500);
	
	}
}


