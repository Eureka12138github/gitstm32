#include "stm32f10x.h"                  // Device header
/*
	说明：
	下限：在原始条件下，PSC=72-1,ARR=65536-1,时测量的最低频率=1M/65536,约为15hz，若频率再低计时器就要溢出了
	若想降低最低频率的限制，可以加大PSC的值，如此标注频率就会更低，所支持的额最低频率便更低
	上限：根据要求误差来确定，若要求误差为千分之一时，频率为上限，那么此上限便是：1M/1000=1khz，
	同理若要求误差为百分之一，则上限频率为1M/100=10khz，若想提高频率上限，可以减小PSC的值，
	若输入频率过高，可考虑采用测频法，这个区分高与低的值为中界频率，即测频法与测周法误差相等的频率点
	fm=根号(fc/T)，fc为测周法标准频率，T为测周法阀门时间，具体讲解见江协6-5 17分左右
	
*/
void IC_Init(void)
{
	//开启时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//配置GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//配置时基
	TIM_InternalClockConfig(TIM3);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//TIM_ClockDivision与滤波的频率有关，决定是否对内部时钟进行分频，需滤波强一点，就TIM_CKD_DIV2或者TIM_CKD_DIV4
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//此处高级计数器才用到，故直接给零了
	TIM_TimeBaseInitStructure.TIM_Period=65536-1;//ARR,减一是因为预分频器和计数器都有1个数的偏差
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//这里是设置计数器的计数模式的，设置为向上计数
	TIM_TimeBaseInitStructure.TIM_Prescaler=72-1;//这里是设置预分频器的主频的“切分”，也就是把主频“切”成7200份
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;//选择通道1是因为配置TIM3通道1
	TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//捕获输入信号上升沿
	TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;//直连通道
	TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;//不对输入信号分频
	TIM_ICInitStructure.TIM_ICFilter=0xf;//滤波值
	TIM_PWMIConfig(TIM3,&TIM_ICInitStructure);
	//这个函数可进行相反的配置，上面对一个通道进行配置，此函数可在配置配置一个通道的同时按相反的方向配置另一个通道
	//以上配置实现了，两个通道（1和2）同时捕获同一个引脚的的信号，两个通道捕获捕获同一个信号可求出出其占空比和频率
	TIM_SelectInputTrigger(TIM3,TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);
	TIM_Cmd(TIM3,ENABLE);
}

uint32_t IC_GetFreq(void)
{
	return 1000000/(TIM_GetCapture1(TIM3)+1);//出现1001Hz可能原因，计数到1000Hz时，信号也刚好跳变，导致这个数刚好没计到，才会有一点误差
}

uint32_t IC_GetDuty(void)
{
	return (TIM_GetCapture2(TIM3)+1)*100/(TIM_GetCapture1(TIM3)+1);//因为高电平的计数值在CRR2里，整个周期的计数值在CRR1里
	//原始函数应该是：TIM_GetCapture2(TIM3)/(TIM_GetCapture1(TIM3))严格对应CRR2/CEE1，但是这样的话范围为0~1，不够直观，故
	//给CRR2*100，这样范围就是0~100了，而+1是“误差修正”，具体解释同6-6的IC函数末尾
}
