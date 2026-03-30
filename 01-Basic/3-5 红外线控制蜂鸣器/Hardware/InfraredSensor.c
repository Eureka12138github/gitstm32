#include "stm32f10x.h"                  // Device header
void Infrared_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;//选择需要配置的端口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//在输入模式下，这里其实不用配置
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
}
uint8_t Infrared_Get(void)
{
	return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13);
}
