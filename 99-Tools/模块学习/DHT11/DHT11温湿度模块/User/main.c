#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "DHT11.h"
int main()
{	
	u16 temp=0,humi=0;
	OLED_Init();
	Delay_s(1);
	while(DHT11_Init())	//检测DHT11是否存在
	{
		OLED_ShowString(0,0,"Error",OLED_8X16);	
		OLED_Update();	
	}
	OLED_Clear();
	OLED_ShowString(0,0,"Temp:   C",OLED_8X16);//现在就差°没显示了241026
	OLED_ShowString(0,18,"Humi:  %RH",OLED_8X16);
	OLED_Update();
//	Delay_ms(1500);	
	while(1)
	{

		DHT11_Read_Data(&temp,&humi);
		OLED_ShowNum(40,0,temp,2,OLED_8X16);//普中原来那样读取估计是因为没有ShowNum函数，现在我可以直接读了，不用再定义数组再对temp和humi进行处理了
		OLED_ShowNum(40,18,humi,2,OLED_8X16);
		OLED_Update();	
		Delay_ms(1500);		
	}
}
