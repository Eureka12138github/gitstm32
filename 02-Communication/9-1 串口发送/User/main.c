#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Serial.h"
/*
	注意使用printf要先勾上Use MicroLIB具体使用见江科大9-3的30分左右有详细介绍
*/
int main(void)
{
	OLED_Init();
	Serial_Init();
	uint8_t num1=2,num2=3,num3=4;
//	uint16_t d=66;
//	Serial_SendByte(0x41);
//	uint8_t MyArray[]={0x42,0x43,0x44,0x45};
//	Serial_SendArray(MyArray,4);
	Serial_SendString("ABCDE\r\n");//\r\n表示换行
//	OLED_ShowString(1,1,"Test!");
//	Serial_SendNum(12345,5);
	printf("Num1=%d\r\n",111);//这种最常用
	char String[100];//定义字符串
	sprintf(String,"Num2=%d\r\n",222);//打印字符串
	Serial_SendString(String);//发送字符串
	/*
	这里如果只是一个串口使用printf函数，便重定向后直接用便是，但此后其他串口便不可使用printf
	第二种sprintf似乎可以实现多个串口使用printf，但目前还是一知半解，不知道怎么使用24/08/31
	补充：sprintf可以把格式化字符输出到一个字符串里
	因为sprintf可以指定打印位置，不涉及重定向，故每个串口都可以使用sprintf进行格式化打印
	*/
	Serial_Printf("你好世界\r\n");//关于乱码问题在9-3的40分左右有解决方法“--no-multibyte-chars”
	Serial_Printf("testNum1=%d\r\ntestNum2=%d\r\ntestNum3=%d\r\n",num1,num2,num3);
	while(1)
	{
	
	}
}
