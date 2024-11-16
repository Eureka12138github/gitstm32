#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Encoder.h"
int16_t Speed;
int main(void)
{
	OLED_Init();
	Encoder_Init();
	Timer_Init();
	OLED_ShowString(1,1,"CNT:");
	while(1)
	{
	OLED_ShowSignedNum(1,7,Speed,5);
//	OLED_ShowNum(2,5,TIM_GetCounter(TIM2),5);	
	}
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		Speed=Encoder_Get();//每隔1秒读取，存在Speed变量里，如此在主循环便可快速刷新Speed了，此时可去掉主循环中的Delay，减少阻塞
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
//未完成任务：用定时器实现旋转编码器计次20240513
/*
	快速完成负数转换技巧：
	将返回的格式uint16_t改为int16_t，h文件声明也改成int16_t之后在主函数中调用相关函数
	这时候的值可以是负数了，原来是0到65526，现在是0到负数
*/
