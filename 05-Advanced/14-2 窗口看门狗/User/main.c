#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
//关于看门狗，我感觉可以用在程序以及完全写好的情况下，先确定每次while循环运行的周期，如果一次循环运行时间大于这个
//周期就让看门狗复位一下。避免程序卡死，暂时理解是这样。241021
uint8_t KeyNum;
int main(void)
{
	Key_Init();
	OLED_Init();
//	Timer_Init();
	OLED_ShowString(1,1,"WWDG TEST");
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET)//如果复位是由看门狗产生的
	{
		OLED_ShowString(2,1,"WWDGRST");
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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE);//窗口看门狗需要自己开启时钟
	//关于超时时间、窗口时间的计算见14-2 22分，及PPT193页
	WWDG_SetPrescaler(WWDG_Prescaler_8);//设置预分频
	WWDG_SetWindowValue(0X40 | 21);//设置重装值，窗口时间30ms
	WWDG_Enable(0X40 | 54);//超时时间50ms，这表明喂狗要在30ms~50ms这段时间内进行，否则会复位
	//使能,54是要自己计算出来的，它是看门狗控制寄存器 WWDG_CR 的T[5:0]的值；而0X40代表 WWDG_CR T6 为1
	//0X40 | 54 == WWDG_CR。（54怎么来的看14-2 23分左右）
	
	while(1)
	{
		//根据上面配置，如果while循环执行用时超过1000ms，即来不及喂狗，独立看门狗就会不断复位
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			OLED_ShowString(3,1,"OK");
		}
		
		OLED_ShowString(4,1,"FEED");
		Delay_ms(20);
		OLED_ShowString(4,1,"     ");
		Delay_ms(20);
//		Delay_ms(30);//过快喂狗
		WWDG_SetCounter(0X40 | 54);//窗口看门狗喂狗函数
		//WWDG_SetCounter与WWDG_Enable不能离太近，否则喂狗过快也会不断复位，见14-2 29分左右的解释
		
	
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
