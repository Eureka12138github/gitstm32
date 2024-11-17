#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "DS18B20.h"
#include "OLED.h"
#include "Encoder.h"
/*
关于DS18B20需要注意的是，GPIO要配置为GPIO_Mode_Out_OD模式，而且外部还要接一个上拉

*/
float T;
int16_t H,L;
int main(void)
{
	OLED_Init();
	Encoder_Init();
	DS18B20_ConvertT();
	Delay_ms(1000);
	OLED_ShowString(1,1,"Temperature:");
	OLED_ShowString(3,1,"H:");
	OLED_ShowString(3,10,"L:");
	OLED_ShowSignedNum(3,3,H,3);
	OLED_ShowSignedNum(3,12,L,3);
	
	while(1)
	{
		H+=Encoder_Get();
		DS18B20_ConvertT();
		T=DS18B20_ReadT();
		if(H>T)
		{
			OLED_ShowString(4,1,"OV:H");
		}
		else
		{
		OLED_ShowString(4,1,"OV:L");
		}
		OLED_ShowSignedNum(3,3,H,3);
		if(T<0)
		{
			OLED_ShowChar(2,1,'-');
			T=-T;
		}
		else {OLED_ShowChar(2,1,'+');}
		OLED_ShowNum(2,2,T,3);
		OLED_ShowChar(2,5,'.');
		OLED_ShowNum(2,6,(unsigned long)(T*10000)%10000,4);
	}
}

/*
#include <REGX52.H>
#include "Delay.h"
#include "LCD1602.h"
#include "DS18B20.h"
//unsigned char Ack;
float T;
void main()
{
	DS18B20_ConvertT();
	Delay(1000);//优化，使得刚开始时不会显示出默认值
	LCD_Init();
	LCD_ShowString(1,1,"Temperature:");
//	Ack=OneWire_Init();
//	LCD_ShowNum(2,1,Ack,3);//现在正在测试DS18B20是否相应，
	//根据视频所说，若1602显示000，则表示相应，拔下DS18B20后按复位
	//变成了显示001，则表示测试通过。现在我不知道为什么这样。
	
	while(1)
	{
		DS18B20_ConvertT();
		T=DS18B20_ReadT();
		if(T<0)
		{
			LCD_ShowChar(2,1,'-');
			T=-T;
		}
		else {LCD_ShowChar(2,1,'+');}
		LCD_ShowNum(2,2,T,3);
		LCD_ShowChar(2,5,'.');
		LCD_ShowNum(2,6,(unsigned long)(T*10000)%10000,4);
		
	
}
}
	

	
*/