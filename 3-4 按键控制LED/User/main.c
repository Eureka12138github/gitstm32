#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"
uint8_t KeyNum;
int main(void)
{
	LED_Init();//端口配置感觉还是不太明白，有空再研究研究
	Key_Init();
	LED2_ON();
	while(1)
	{
//		KeyNum=Key_GetNum();
//		if(KeyNum==1)
//		{
//			LED1_Turn();//这个判断当前输出并将之反转的
//		}
//		if(KeyNum==2)
//		{
//			LED2_Turn();
//		}		
//		LED2_ON();
	}
}
