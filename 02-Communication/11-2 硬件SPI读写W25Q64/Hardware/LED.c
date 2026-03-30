#include "stm32f10x.h"                  // Device header
/**
* @brief 端口1、端口2初始化，推挽
  * @param  无
  * @retval 无
  */
void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2);//表示刚开始时，A1、A2口为高电平，根据接线，此时不亮
}
/**
* @brief 使端口1置低电平，即使得接到端口1上的LED亮
  * @param  无
  * @retval 无
  */
void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}
/**
* @brief 使端口1置高电平，即使得接到端口1上的LED灭
  * @param  无
  * @retval 无
  */
void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
}
/**
* @brief 反转端口输出值，为高时反转为低，为低时反转为高
  * @param  无
  * @retval 无
  */
void LED1_Turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1)==0)
	{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	}
	else 
	{
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	}
}
//以下为端口2的内容，与端口1相同，注释如上
void LED2_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
}
void LED2_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}
void LED2_Turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_2)==0)
	{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	}
	else 
	{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	}
}
