#include "stm32f10x.h"                  // Device header
void IC_Init(void)
{
	//开启时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//TIM2用于产生PWM，所以这里换成了TIM3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//配置GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//此处配置为上拉模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//这里表示使用了TIM3的通道1
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//配置时基
	TIM_InternalClockConfig(TIM3);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//TIM_ClockDivision与滤波的频率有关，决定是否对内部时钟进行分频，需滤波强一点，就TIM_CKD_DIV2或者TIM_CKD_DIV4
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//此处高级计数器才用到，故直接给零了
	TIM_TimeBaseInitStructure.TIM_Period=65536-1;//ARR值设置为最大防止计数溢出
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//这里是设置计数器的计数模式的，设置为向上计数
	TIM_TimeBaseInitStructure.TIM_Prescaler=72-1;
	/*在此工程中，PSC的值决定了测周法的标准频率fc，72MHZ/PSC=计数器自增频率=计数标准频率
	该值要根据信号的分布范围来调整（这里怎么个调整法还不太明白，也不明白为什么这里选72，UP说是比较方便计算）
	*/
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	//初始化输入捕获单元
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;//选择通道1是因为配置TIM3通道1
	TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//捕获输入信号上升沿
	TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;//直连通道
	TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;//不对输入信号分频，即保持原始的输入信号，若对其进行二分频，则需要两次上升沿才会触发一次
	TIM_ICInitStructure.TIM_ICFilter=0xf;//滤波值
	TIM_ICInit(TIM3,&TIM_ICInitStructure);//还不太懂这个流程240413
	//以下两条语句是配置主从模式的
	TIM_SelectInputTrigger(TIM3,TIM_TS_TI1FP1);//这句是配置TRGI的触发源为TI1FP1
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);//配置从模式为Reset
	TIM_Cmd(TIM3,ENABLE);//启动定时器3
	/*
	启动定时器后，CNT就会在内部时钟的驱动下不断自增，即使没有信号过来，也会自增，有信号来时，CNT会在从模式作用下自动清零，不会影响测量
	*/
}

uint32_t IC_GetFreq(void)
{
	return 1000000/(TIM_GetCapture1(TIM3)+1);
	//出现1001Hz可能原因，计数到1000Hz时，信号也刚好跳变，导致这个数刚好没计到，才会有一点误差
	//上面用到的是测周法测频率的公式：fx=fc/N，因为TIM3的PSC为72所以这里的fc为1MHZ即1 000 000HZ
	//N是TIM3中CCR的值，用TIM_GetCapture1();函数即可获取
}
