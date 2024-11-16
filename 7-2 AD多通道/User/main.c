#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "AD.h"
uint16_t AD0,AD1,AD2,AD3;
int main(void)
{
	OLED_Init();
	AD_Init();
	OLED_ShowString(1,1,"AD0:");
	OLED_ShowString(2,1,"AD1:");
	OLED_ShowString(3,1,"AD2:");
	OLED_ShowString(4,1,"AD3:");
	while(1)
	{
		/*
		依次启动四次转换，并且在转换之前指定了转换的通道，每次转换完成之后，
		把结果分别存在4个数据里，这就是使用单次转换非扫描的方式实现AD多通道的
		方法。
		*/
		AD0=AD_GetValue(ADC_Channel_0);
		AD1=AD_GetValue(ADC_Channel_1);
		AD2=AD_GetValue(ADC_Channel_2);
		AD3=AD_GetValue(ADC_Channel_3);
		OLED_ShowNum(1,5,AD0,4);
		OLED_ShowNum(2,5,AD1,4);
		OLED_ShowNum(3,5,AD2,4);
		OLED_ShowNum(4,5,AD3,4);
		Delay_ms(100);
		//目前显示略有波动，以后可以尝试使用定时器并结合均值滤波等方法来使得显示稳定
		
		
	}
}
