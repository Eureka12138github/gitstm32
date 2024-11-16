#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "oled.h"
int main(void)
{
	OLED_Init();	
	OLED_ShowChar	(1,1,'A');
	OLED_ShowString(1,3,"Hello,World!");
	OLED_ShowNum(2,1,12345,5);
	OLED_ShowSignedNum(2,7,-66,2);
	OLED_ShowHexNum(3,1,0xaa55,4);
	OLED_ShowBinNum(4,1,0xaa55,16);
	while(1)
	{
	
	}
}
