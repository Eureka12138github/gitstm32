#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Timer.h"
#include "Store.h"
//测试是否可被git识别到修改具体某行！
/*
	问题描述如下：OLED无法正常显示，必须注释掉Timer_Init();或者在Timer_Init()中将ARR与PSC改大才可以
	目前实测，将ARR改为10000,PSC改为7200时能够正常显示。按理说两者不应该冲突的，而且在练习1 秒表中
	我定时器也是设置为1ms,即ARR = 10,PSC = 7200，在那个项目中又没有问题，怎么到了这里就有问题了呢？
	不懂！换成新的OLED函数也是如此，目前无法解决！241022
	2410221512:通过排除法找到的不知道原理的解决方法：在main函数后面加上定时器的中断函数TIM2_IRQHandler，
	哪怕它啥也不干，仅仅只是当有定时器中断来的时候将标志位清零，这样就可以正常显示了。
	
*/
u8 KeyNum = 0;
int main(void)
{
	Store_Init();
	Key_Init();
	Timer_Init();
	OLED_Init();
	OLED_ShowString(1,1,"FLAG:");
	OLED_ShowString(2,1,"DATA:");
//	Store_Clear();

	while(1)
	{	
	KeyNum = Get_Key();
	if(KeyNum == 1)
	{
		Store_Data[1]++;
		Store_Data[2]++;
		Store_Data[3]++;
		Store_Data[4]++;
		Store_Save();//放到FLASH中，实现掉电不丢失
	}
	if(KeyNum == 3)	
	{
		Store_Clear();
	}	
	OLED_ShowHexNum(1,6,Store_Data[0],4);
	OLED_ShowHexNum(3,1,Store_Data[1],4);
	OLED_ShowHexNum(3,6,Store_Data[2],4);
	OLED_ShowHexNum(4,1,Store_Data[3],4);
	OLED_ShowHexNum(4,6,Store_Data[4],4);
	
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
