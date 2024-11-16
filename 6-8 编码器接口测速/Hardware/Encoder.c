#include "stm32f10x.h"                  // Device header
void Encoder_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//配置GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//配置时基
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//TIM_ClockDivision与滤波的频率有关，决定是否对内部时钟进行分频，需滤波强一点，就TIM_CKD_DIV2或者TIM_CKD_DIV4
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//此处高级计数器才用到，故直接给零了
	TIM_TimeBaseInitStructure.TIM_Period=65536-1;//ARR,减一是因为预分频器和计数器都有1个数的偏差
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//这里是设置计数器的计数模式的，设置为向上计数
	TIM_TimeBaseInitStructure.TIM_Prescaler=1-1;//这里是设置预分频器的主频的“切分”，也就是把主频“切”成0份
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	//配置两个通道的滤波和极性
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);//结构体初始化，用来给未配置的变量赋初始值
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;//选择通道1是因为配置TIM3通道1
	TIM_ICInitStructure.TIM_ICFilter=0xf;//滤波值
	TIM_ICInit(TIM3,&TIM_ICInitStructure);//还不太懂这个流程240413
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_2;//选择通道1是因为配置TIM3通道1
	TIM_ICInitStructure.TIM_ICFilter=0xf;//滤波值
	TIM_ICInit(TIM3,&TIM_ICInitStructure);//此时TIM_ICInit只剩滤波作用了
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Falling);
	//上个函数，选择定时器3，选择TI12都计次，选择上升沿（不反相）
	TIM_Cmd(TIM3,ENABLE);
}

int16_t Encoder_Get(void)
{
	int16_t Temp;
	Temp=TIM_GetCounter(TIM3);//返回CNT
	TIM_SetCounter(TIM3,0);
	return Temp;
}