#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
uint8_t KeyNum,min,sec,minisec,runflag=1;
int main(void)
{
	//ID:[ 31 00 66 06 31 42 32 38 39 46 11 43]，这是小端读取。实际ID号应该是：0031 0666 3832 4231 4311 4639
	//想把ID获取函数封装起来但不知道如何做241023
	OLED_Init();
	OLED_ShowString(0,0,"F_SIZW:",OLED_8X16);
	OLED_ShowHexNum(58,0,*((__IO u16 *)(0X1FFFF7E0)),4,OLED_8X16);//读取设备FLASH容量大小
	OLED_ShowString(0,18,"U_ID:",OLED_8X16);//读取设备ID
	OLED_ShowHexNum(45,18,*((__IO u16 *)(0X1FFFF7E8)),4,OLED_8X16);
	OLED_ShowHexNum(80,18,*((__IO u16 *)(0X1FFFF7E8 + 0X02)),4,OLED_8X16);
	OLED_ShowHexNum(0,34,*((__IO u32 *)(0X1FFFF7E8+ 0X04)),8,OLED_8X16);
	OLED_ShowHexNum(0,50,*((__IO u32 *)(0X1FFFF7E8+ 0X08)),8,OLED_8X16);
	OLED_Update();
	
	while(1)
	{

	
	}
}



