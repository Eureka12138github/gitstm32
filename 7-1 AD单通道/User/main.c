#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "AD.h"
uint16_t ADValue;
float Voltage;
int main(void)
{
	OLED_Init();
	AD_Init();
	OLED_ShowString(1,1,"ADValue:");
	OLED_ShowString(2,1,"Voltage:0.00V");	
	while(1)
	{
		ADValue=AD_GetValue();
		OLED_ShowNum(1,9,ADValue,4);//可以采用滤波方式使得信号变得平滑
		Voltage=(float)ADValue/4095*3.3;//转换为电压形式，这里强制转换为浮点数是因为要保留小数部分
		OLED_ShowNum(2,9,Voltage,1);//显示浮点数，由于现在的OLED还没有显示浮点数的函数
		OLED_ShowNum(2,11,(uint16_t)(Voltage*100)%100,2);//这里强制转换为整数是因为浮点数不可取余
		Delay_ms(100);
		//目前显示略有波动，以后可以尝试使用定时器并结合均值滤波等方法来使得显示稳定
		
		
	}
}
