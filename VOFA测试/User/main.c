#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Serial.h"
#include "math.h"
float t = 0;
int main(void)
{
	OLED_Init();
	Serial_Init();
/*
			void setup() {
		 Serial.begin(115200);
		}
		float t = 0;
		void loop() {
		 t += 0.1;
		 Serial.print("samples:%f, %f, %f, %f\n", sin(t), sin(2*t), sin(3*t), sin(4*t));
		 // 不加any和冒号也可以
		 // Serial.print("%f, %f, %f, %f\n", sin(t), sin(2*t), sin(3*t), sin(4*t));  
		 delay(100);
}*/

	while(1)
	
	{
		 t += 0.1;
		 Serial_Printf("samples:%f, %f, %f, %f\n\r", sin(t), sin(2*t), sin(3*t), sin(4*t));
		 OLED_ShowFloatNum(0,0,sin(t),1,1,OLED_6X8);
		 OLED_ShowFloatNum(0,20,sin(2*t),1,1,OLED_6X8);
		 OLED_ShowFloatNum(40,0,sin(3*t),1,1,OLED_6X8);
		 OLED_ShowFloatNum(40,20,sin(4*t),1,1,OLED_6X8);
		 OLED_Update();
		 // 不加any和冒号也可以
		 // Serial.print("%f, %f, %f, %f\n", sin(t), sin(2*t), sin(3*t), sin(4*t));  
		 Delay_ms(100);
	}
}


/*	查询方式接收串口数据(适用于简单程序)：
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
	{
	//标志位会自动清零
	RxData=USART_ReceiveData(USART1);
	OLED_ShowHexNum(1,1,RxData,2);	
	}
*/
