#include "stm32f10x.h"                  // Device header
//extern uint16_t Num;
void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//tIM2是APB1总线的外设，所以要开启APB1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	/*
	关于GPIO口模式的配置参考手册的8.1.11中有推荐，其推荐位浮空输入，这里没选择悬空是因为
	一旦悬空电平就会跳个没完，所以从个人喜好来说UP选择了上拉输入，但其实原则上应该要用浮空
	另外UP给出了使用浮空模式的场景：“外部输入信号很小，内部上拉电阻可能会影响到这个输入信号时”
	*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	TIM_ETRClockMode2Config(TIM2,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,0X04);
	/*上一句的函数，
	第一个参数选择外设，
	第二个参数是决定对输入信号是否分频的，
	第三个参数是决定是否对输入信号进行反向的，若反向就低电平或下降沿有效，不反向即高电平或上升沿有效
	第四个参数是滤波的，范围为0x00~0x0f,值越大滤波越强，但延迟越厉害，要根据实际应用选择合适的值
	*/
	//这个滤波真是太棒了，完美解决抖动问题，就是不知道是怎么实现的，是内部自带的?
	//另外，为什么选择TIM_ETRClockMode2Config这个函数，选择TIM_ETRClockMode1Config不行吗？
	/*
	在模式1中，ETR被明确指定为触发输入源，定时器直接对此信号响应。
	在模式2中，虽然ETR同样用作时钟输入，但代码中并没有明确指定ETR作为触发输入源。
	这可能意味着在模式2下，ETR的使用更加灵活，可以与其他触发源或模式结合使用。
	因此，模式1和模式2之间的主要区别在于它们如何处理ETR信号以及它们在定时器操作中的灵活性。
	选择哪种模式取决于你的具体应用需求，例如你是否需要一个简单的直接时钟源，或者需要一个更复杂的同步操作。
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//此处高级计数器才用到，故直接给零了
	TIM_TimeBaseInitStructure.TIM_Period=10-1;//ARR,减一是因为预分频器和计数器都有1个数的偏差
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Prescaler=5-1;//UP说手动的没那么快，所以PSC和ARR该小一点，不太懂现在
	//现在是没预分频，若有则分频计次就遮挡计次后再加1,我的理解是分了几次，再重新构成时，就要几次
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	/*
	加了上面这句，复位后数才从0开始，原问题来自于TIM_TimeBaseInit函数带来的副作用，
	即更新事件和更新中断是同时发生的，更新中断会置中断标志位，初始化一旦完成了，更新中断就会立刻进入
	这就是刚上电就进入中断的原因（依旧不太懂。。。）
	
	*/
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//开启了更新中断到NVIC的通路
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM2,ENABLE);
}
/*
	240502：对于这个函数，现在我能想到的用法就是用来计数，外部输入一个信号，然后计数一次
*/
uint16_t Timer_GetCounter(void)
{
	return TIM_GetCounter(TIM2);
}
uint16_t Timer_GetPrescaler(void)
{
	return TIM_GetPrescaler(TIM2)+1;
}
/*
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		Num++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
*/