#include "stm32f10x.h"                  // Device header
#define Buzzer_IO GPIO_Pin_12
void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=Buzzer_IO;//选择需要配置的端口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//在输入模式下，这里其实不用配置
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,Buzzer_IO);
	
}
//void BuzzerMode(uint8_t Mode)
//{
//	if(Mode==0)
//	{
//		GPIO_ResetBits(GPIOB,Buzzer_IO);
//	}
//	else
//	{
//		GPIO_SetBits(GPIOB,Buzzer_IO);
//	}
//}
//
void Buzzer_ON(void)
{
	GPIO_ResetBits(GPIOB,Buzzer_IO);
}
void Buzzer_OFF(void)
{
	GPIO_SetBits(GPIOB,Buzzer_IO);
}
void Buzzer_Turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOB,Buzzer_IO)==0)
	{
	GPIO_SetBits(GPIOB,Buzzer_IO);
	}
	else 
	{
	GPIO_ResetBits(GPIOB,Buzzer_IO);
	}
}