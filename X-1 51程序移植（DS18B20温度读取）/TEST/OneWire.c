#include "stm32f10x.h"                  // Device header
#include "Delay.h" 
uint8_t OneWire_Init(void)
{
	uint8_t i,AckBit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD ;//实现DS18B20温度读取，需外接上拉电路
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);//主机拉低总线500us
	Delay_us(500);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);//主机释放总线
	Delay_us(70);//从机会在这期间响应主机，即从机将会拉低总线
	AckBit=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);//读取响应，告诉主机有从机响应了
	Delay_us(500);
	return AckBit;
}

void OneWire_SendBit(uint8_t Bit)
{
	uint8_t i;
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	Delay_us(10);
	if(Bit==0)
	{
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	}
	else 
	{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	}
	Delay_us(50);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
}

uint8_t OneWire_ReceiveBit(void)
{
	uint8_t i,Bit;
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	Delay_us(5);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	Delay_us(5);
	Bit=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);//将GPIO_ReadOutputDataBit改为GPIO_ReadInputDataBit终于可以啦！~
	Delay_us(50);
	return Bit;
	
}

void OneWire_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		OneWire_SendBit(Byte&(0x01<<i));
	}
}

uint8_t OneWire_ReceiveByte(void)
{
	uint8_t i,Byte=0x00;
				for(i=0;i<8;i++)
			{
				if(OneWire_ReceiveBit()){Byte|=(0x01<<i);}
			}
			return Byte;
}
