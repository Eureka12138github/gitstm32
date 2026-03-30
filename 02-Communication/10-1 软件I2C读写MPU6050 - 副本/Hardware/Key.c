#include "stm32f10x.h"                  // Device header
#include "Delay.h"
/**
* @brief 配置端口B1、端口B11，为上拉模式
  * @param  无
  * @retval 无
  */
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_11;//选择需要配置的端口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//在输入模式下，这里其实不用配置
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
}
/**
  * @brief 监测端口B1与端口B11是否因为按键按下而变为低电平
  * @param  无
  * @retval 返回哪个端口的值被下拉了
  */
uint8_t Key_GetNum(void)//uint8_t就是unsigned char
{
	uint8_t KeyNum=0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)//相当于if(PB_1==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0);//表示若按键一直按下，就停在这里
		Delay_ms(20);
		KeyNum=1;
		
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0)//相当于if(PB_11==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0);//表示若按键一直按下，就停在这里
		Delay_ms(20);
		KeyNum=2;
		
	}
	
	return KeyNum;
	
}