#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Encoder.h"
int16_t Num;
int main(void)
{
	OLED_Init();	
	Encoder_Init();
	LED_Init();
	OLED_ShowString(1,1,"Num:");
	while(1)
	{
		
		Num+=Encoder_Get();
//		if(Num>=10)//为什么灯不亮啊？亮了，代码没变，但是现在亮了！20240402
//		{
//		LED2_ON();
//		LED1_ON();
//		}
//		if(Num<10)
//		{
//		LED2_OFF();
//		LED1_OFF();		
//		}
			
		OLED_ShowSignedNum(1,5,Num,5);
//		LED2_ON();
//		LED1_ON();
	}
}
/*
两个编程建议：
1，中断函数尽量不要用延时函数；
2，最好不要在中断函数中和主函数中调用相同的函数或操作同一个硬件，
正确的做法是，在中断里操作变量或者标志位，当中断返回时再对变量进行显示和操作
*/
