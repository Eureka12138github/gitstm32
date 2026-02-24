#include "stm32f10x.h"                  // Device header
int16_t Encoder_Count;
void Encoder_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//240429作出以下修改，将B口的0，1改为A口的6,7
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource6);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line6|EXTI_Line7;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel=EXTI1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
//	NVIC_Init(&NVIC_InitStructure);
}
int16_t Encoder_Get(void)
{
	int16_t temp;//每次调用此函数之后，返回Count的变化值，用于外部加减一个变量，但因为返回Count之后，函数结束，
	//无法将Count清零（不太懂，为什么Count一定要清零？）
	/*
	240430：不一定要清零，这里清零主要是为了配合主函数中的逻辑，
	若将Num+=Encoder_Get();改为Num=Encoder_Get();这里就不用清零，直接返回Encoder_Count即可
	
	！！！上面说法是错误的！！！还是要清零，不清的话，会一直返回旧值！
	*/
	temp=Encoder_Count;
	Encoder_Count=0;
	return temp;//此处返回的值为1或-1
}
//void EXTI0_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line0)==SET)//SET是中断标志位置1，即开启了中断
//	{
//		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
//			/*
//		此中断是接旋转编码器的A的，此中断产生表明此时A口输出的电平出现下降沿，再根据选择编码器的特性，
//		即若反转的话B口的电平输出滞后一半，故此时若监测到B口电平为低电平，则表明此时是反转
//		滞后与领先表示的是波形，右边为前
//		*/				
//		{
//		Encoder_Count--;
//		}
//	EXTI_ClearITPendingBit(EXTI_Line0);
//	}
//}
//void EXTI1_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line1)==SET)
//	{
//			/*
//		此中断是接旋转编码器的B的，此中断产生表明此时B口输出的电平出现下降沿，再根据选择编码器的特性，
//		即若正转的话B口的电平输出领先一半，故此时若监测到A口电平为低电平，则表明此时是正转
//		*/			
//		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==0)
//		{
//		Encoder_Count++;
//		}
//	EXTI_ClearITPendingBit(EXTI_Line1);
//	}
//}
//下面是测试程序，作了正反转判断的一点小小改动，关于选择编码器正反转判断我现在是理解了，到关于江协说的“要转到位”那里不太理解
//void EXTI0_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line0)==SET)//SET是中断标志位置1，即开启了中断
//	{
//		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==1)			
//		{
//		Encoder_Count++;
//		}
//	EXTI_ClearITPendingBit(EXTI_Line0);
//	}
//}
//void EXTI1_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line1)==SET)
//	{			
//		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==1)
//		{
//		Encoder_Count--;
//		}
//	EXTI_ClearITPendingBit(EXTI_Line1);
//	}
//}
//下面是改为A口的6、7PIN时的程序
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line7)==SET)
	{			
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==1)
		{
		Encoder_Count--;
		}
	EXTI_ClearITPendingBit(EXTI_Line7);
	}
	if(EXTI_GetITStatus(EXTI_Line6)==SET)
	{			
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==1)
		{
		Encoder_Count++;
		}
	EXTI_ClearITPendingBit(EXTI_Line6);
	}
}
