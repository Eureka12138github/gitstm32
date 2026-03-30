#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
uint8_t KeyNum,min,sec,minisec,runflag=1;
int main(void)
{
	Timer_Init();
	Key_Init();
	OLED_Init();
	LED_Init();
	while(1)
	{
	KeyNum = Key();
	if(KeyNum == 1)	
	{
		LED1_Turn();
		OLED_ShowString(0,0,"             ",OLED_8X16);
		OLED_ShowString(0,0,"LED1_ON!",OLED_8X16);
		OLED_Update();
	}
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1)==1)
	{
		OLED_ShowString(0,0,"             ",OLED_8X16);
		OLED_ShowString(0,0,"LED1_OFF!",OLED_8X16);
		OLED_Update();
	}
		OLED_ShowNum(0,20,min,2,OLED_8X16);
		OLED_ShowNum(20,20,sec,2,OLED_8X16);
		OLED_ShowNum(40,20,minisec,2,OLED_8X16);
		OLED_Update();
	
	}
}

void sec_loop(void)
{
	if(runflag)
	{
		minisec++;
	if(minisec>=100)
	{
		minisec=0;
		sec++;
		if(sec>=60)
		{
			sec=0;
			min++;
			if(min>=60)
			{
				min=0;
			}
		}
	}

}
	}

void TIM2_IRQHandler(void)
{
	static uint8_t T0Count1,T0Count3;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		T0Count1++;
		T0Count3++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//这句还搞不太懂，只有一个事件时应该可以直接清零把？
	}
	
	if(T0Count1>=20)
	{
		T0Count1=0;
		Key_Loop();//这个函数不能出现Delay等耗时语句，否则定时器会出错
	}

	if(T0Count3>=10)
	{
		T0Count3=0;
		sec_loop();
	}
}

