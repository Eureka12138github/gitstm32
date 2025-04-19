#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
int main(void)
{
	Timer_Init();
	Key_Init();
	OLED_Init();
	LED_Init();
	u8 num = 0;
	KeyEventType Event[4] = {KEY_EVENT_NONE};
	OLED_ShowString(0,0,"num:",OLED_8X16);
	OLED_Update();
	//现在可以进行单击，双击，长按，但长按功能未完善，现在只返回一次长按状态。
	while(1)
	{
		Key_GetEvent(Event,4);     
		if(Event[0] == KEY_EVENT_CLICK){
			num++;
		}
		if(Event[1] == KEY_EVENT_DOUBLE_CLICK){
			num--;
		}		
		if(Event[2] == KEY_EVENT_LONG_PRESS){
			num += 1;
		}		
		if(Event[3] == KEY_EVENT_TRIPLE_CLICK){
			num = 0;
		}
		OLED_ShowNum(34,0,num,3,OLED_8X16);
		OLED_Update();
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
		Key_Scan();//这个函数不能出现Delay等耗时语句，否则定时器会出错
	}


}

