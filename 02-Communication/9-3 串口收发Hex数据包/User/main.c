#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Serial.h"
#include "Key.h"
uint8_t KeyNum;
int main(void)
{
	OLED_Init();
	Serial_Init();
	Key_Init();
	
	OLED_ShowString(1,1,"TxPacket");
	OLED_ShowString(3,1,"RxPacket");
	Serial_TxPaket[0] = 0x01;
	Serial_TxPaket[1] = 0x02;
	Serial_TxPaket[2] = 0x03;
	Serial_TxPaket[3] = 0x04;
	while(1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 1)
		{
		//按下按键，变换一下数据，发送一个数据包
		Serial_TxPaket[0] ++;
		Serial_TxPaket[1] ++;
		Serial_TxPaket[2] ++;
		Serial_TxPaket[3] ++;
		Serial_SendPacket();
			
		OLED_ShowHexNum(2,1,Serial_TxPaket[0],2);//此程序可能存在问题见9-5 13分左右
		OLED_ShowHexNum(2,4,Serial_TxPaket[1],2);
		OLED_ShowHexNum(2,7,Serial_TxPaket[2],2);
		OLED_ShowHexNum(2,10,Serial_TxPaket[3],2);
		}
	if(Serial_GetRxFlag() == 1)
	{
		OLED_ShowHexNum(4,1,Serial_RxPaket[0],2);//此程序可能存在问题见9-5 13分左右
		OLED_ShowHexNum(4,4,Serial_RxPaket[1],2);
		OLED_ShowHexNum(4,7,Serial_RxPaket[2],2);
		OLED_ShowHexNum(4,10,Serial_RxPaket[3],2);
	}
	}
}



