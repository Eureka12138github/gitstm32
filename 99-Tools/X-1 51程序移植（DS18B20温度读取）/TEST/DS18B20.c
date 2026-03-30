#include "stm32f10x.h"                  // Device header
#include "OneWire.h"
#include "Delay.h"
#define DS18B20_SKIP_ROM 0xcc
#define DS18B20_CONVERT_T 0x44
#define DS18B20_READ_SCRATCHPAD 0xBE
void DS18B20_ConvertT(void)
{
	OneWire_Init();
	OneWire_SendByte(DS18B20_SKIP_ROM);
	OneWire_SendByte(DS18B20_CONVERT_T);
}
float DS18B20_ReadT(void)
{
	uint8_t TLSB,TMSB;
	int Temp;
	float T;
	OneWire_Init();
	OneWire_SendByte(DS18B20_SKIP_ROM);
	OneWire_SendByte(DS18B20_READ_SCRATCHPAD);
	TLSB=OneWire_ReceiveByte();
	TMSB=OneWire_ReceiveByte();
	Temp=(TMSB<<8)|TLSB;
	T=Temp/16.0;
	return T;
	/*
	无法正确读温度，只能返回一个固定值，不知道为什么，似乎DS18B20根本没能影响到输出
	可能是32相较于51性能高了，数据每收集到？我感觉很可能是OneWire出问题了。难道IO口不能随便给？我看51板P3^7是串口的接收口，
	它接到了DS18B20中间引脚了，但这不是单总线吗？和串口应该是无关的啊，我现在的理解是IO可随便给。为什么返回的T是一个恒定的值啊？
	还是95点多，不懂。
	*/
}	
