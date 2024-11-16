#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "OLED.h"
#include "Motor.h"
int main(void)
{
	uint8_t Num;
	int_least8_t Speed;
	OLED_Init();
	Motor_Init();
	Key_Init();
	
	while(1)
	{
		OLED_ShowString(1,1,"Speed:");
		Num=Key_GetNum();
		if(Num==1)
		{
//		if(Speed>=0)
//		{
//		Speed+=20;
//		Motor_SetSpeed(Speed);	
//		}
//		if(Speed>100)
//		{
//		Speed=-100;	
//		Motor_SetSpeed(Speed);	
//		}	
//		else if((Speed==-100)||(Speed<0))
//		{
//		Speed+=20;
//		Motor_SetSpeed(Speed);
//		}
		
		Speed += 20;
		if (Speed > 100) 
		{
        Speed = -100;
		}
		//看人的代码多简洁
		
//		else 
//		{
//		Speed += 20;
//		}
		Motor_SetSpeed(Speed);	
			
		


		/*
		若电机有蜂鸣器的声音，可以加大PWM频率，可考虑改变PSC值，这样不改变占空比，在此程序中
		PSC改为36时，频率到20khz,便听不到了，若想出现蜂鸣则减小频率即可，（加大PSC的值）
		*/
		
		}
		OLED_ShowSignedNum(1,7,Speed,3);
	}
}
