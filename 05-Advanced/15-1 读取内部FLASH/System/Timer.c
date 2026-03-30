#include "stm32f10x.h"                  // Device header
//extern uint16_t Num;定义外部变量？
void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//tIM2是APB1总线的外设，所以要开启APB1时钟
	
	TIM_InternalClockConfig(TIM2);//这句表明TIM2的时基单元由内部时钟驱动，系统默认由内部时钟驱动，不写这句也行
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//TIM_ClockDivision与滤波的频率有关，决定是否对内部时钟进行分频，需滤波强一点，就TIM_CKD_DIV2或者TIM_CKD_DIV4
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//此处高级计数器才用到，故直接给零了
	TIM_TimeBaseInitStructure.TIM_Period=10-1;//ARR,减一是因为预分频器和计数器都有1个数的偏差
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//这里是设置计数器的计数模式的，设置为向上计数
	TIM_TimeBaseInitStructure.TIM_Prescaler=7200-1;//这里是设置预分频器的主频的“切分”，也就是把主频“切”成7200份
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化通用定时器TIM2的时基单元
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//开启了更新中断到NVIC的通路，这里不太理解，为什么是更新中断？

	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;//这句是找到TIM2在NVIC中的中断通道，不同类型的中断，在NVIC中的中断名称也不一
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//这句是将该通道打通
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//这句和下一句是配置优先级的为什么这样配我现在也不清楚
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	//上面优先级配置我感觉影响不大，因为只有一个中断源
	NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM2,ENABLE);//这句相当于给TIM2使能，我的理解是相当于51中的ET0=1;吧，要使用定时器，这句一定得加上，否则定时器都没打开你让它怎么工作？
}

/*
定时器名称是规定好的，需要在start文件里面的mds里面找
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		Num++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}

*/
