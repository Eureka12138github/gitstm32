#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
uint8_t KeyNum;
int main(void)
{
	Key_Init();
	OLED_Init();
//	Timer_Init();
	OLED_ShowString(1,1,"IWDG TEST");
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)//如果复位是由看门狗产生的
	{
		OLED_ShowString(2,1,"IWDGRST");
		Delay_ms(500);
		OLED_ShowString(2,1,"         ");
		Delay_ms(100);
		RCC_ClearFlag();//清除标志位，该标志位必须软件清除！
	}
	else 
	{
		OLED_ShowString(3,1,"RST");
		Delay_ms(500);
		OLED_ShowString(3,1,"    ");
		Delay_ms(100);
	}
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//解除写保护
	IWDG_SetPrescaler(IWDG_Prescaler_16);//配置预分频
	IWDG_SetReload(2499);//配置重装值，重装置的范围是0~4096，现在是每隔1000ms喂一次狗，具体计算过程见14-2 11~12分，或者PPT190页
	IWDG_ReloadCounter();//先喂一次狗？这样CNT初始值就为2499？
	IWDG_Enable();//启动看门狗
	
	while(1)
	{
		//根据上面配置，如果while循环执行用时超过1000ms，即来不及喂狗，独立看门狗就会不断复位
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			OLED_ShowString(3,1,"OK");
		}
		IWDG_ReloadCounter();
		OLED_ShowString(4,1,"FEED");
		Delay_ms(200);
		OLED_ShowString(4,1,"     ");
		Delay_ms(600);
		
		
	
	}
}

	void TIM2_IRQHandler(void)
{
	static uint8_t T0Count1;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		T0Count1++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//这句还搞不太懂，只有一个事件时应该可以直接清零把？
	}
	
	if(T0Count1>=20)
	{
		T0Count1=0;
		Key_Loop();//这个函数不能出现Delay等耗时语句，否则定时器会出错
	}
}
