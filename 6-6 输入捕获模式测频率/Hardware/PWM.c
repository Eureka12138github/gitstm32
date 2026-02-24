#include "stm32f10x.h"                  // Device header
void PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//tIM2是APB1总线的外设，所以要开启APB1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
//	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);//部分重映射，这样PA0就换到PA15了
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//有的IO口是有其默认功能的要解出其默认功能方能使用，此函数可以用来解出IO口的默认功能
	/*
	一、让调试端口变正常端口，用：
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(X,ENABLE);
	二、若想重映射定时器或者其他外设的复用引脚，用：
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);
	三、若重映射的正好是调试端口，上面两种情况代码结合。
	上函数使得PA15、PB3、PB4变回了正常IO口
	有的IO口是有其默认功能的要解出其默认功能方能使用，此函数可以用来解出IO口的默认功能
	比如在此工程中，将PA0口重映射到PA15就需要解除其测试功能，使其变回正常的IO口，
	注意该参数GPIO_Remap_SWJ_Disable不可随便使用，否则就烧录不进程序了，重新弄回来需要花费一番功夫
	具体可见手册8.3的表35
	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出，只有这样，引脚的控制权才能交给偏上外设，PWM波形才能通过引脚输出视频解释在6-4 21分
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	TIM_InternalClockConfig(TIM2);//这句表明TIM2的时基单元由内部时钟驱动，系统默认由内部时钟驱动，不写这句也行
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//TIM_ClockDivision与滤波的频率有关，决定是否对内部时钟进行分频，需滤波强一点，就TIM_CKD_DIV2或者TIM_CKD_DIV4
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//此处高级计数器才用到，故直接给零了
	TIM_TimeBaseInitStructure.TIM_Period=100-1;//ARR,减一是因为预分频器和计数器都有1个数的偏差
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//这里是设置计数器的计数模式的，设置为向上计数
	TIM_TimeBaseInitStructure.TIM_Prescaler=720-1;//这里是设置预分频器的主频的“切分”，也就是把主频“切”成7200份
	/*
	时基单元：PSC(预分频器)、ARR(自动重装器)、CNT(计数器)
	计数器溢出频率:CK_CNT_OV=CK_CNT/(ARR+1)=CK_PSC/(PSC+1)/(ARR+1)
	害，实际要计算每次计数时间就相当于1/(接到分频器上的频率(Mhz)*1000000/预分频器的值-1/重装的值-1)
	例如：现在我接到PSC上的频率为单片机的主频即72Mhz，而PSC的值为7200，即进行7200分频，重装值为10000，即
	此时根据公式可得：1/(72000000/7200-1+1/10000-1+1)=1s
	PSC预分配器，对主频72Mhz进行分频，为0表示不分频，即还是72Mhz,为1表示二分频，即72Mhz/2
	故在此处表示的是进行7200分频，即72Mhz/7200，但不能直接写7200，根据其规则，应该要写7200-1
	它后面会自己“+1”从而把“-1”给抵消掉
	另：PSC和ARR都有自己的缓冲寄存器，它们的目的是为了在中断更改相应值的时候不会出现错误，用或不用，可自己设置
	具体见江协视频6-1 的39分左右
	另：PSC与ARR的取值范围是0~65535，不能超范围，若预PSC小点，ARR大点，那就是以较高的频率计较多的数
	若若预PSC大点，ARR小点，那就是以较低的频率计较少的数，但结果是不变的，例如在这里我可以
	*/

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化通用定时器TIM2的时基单元
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);//给结构体没用到的参数赋初始值后，再对个别需要用到的参数赋值，这样，可以避免一些问题
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;//设置输出比较的模式
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//设置输出比较的极性
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//输出使能
	TIM_OCInitStructure.TIM_Pulse=0;//CCR,在这里，CCR是改变占空比的，是多少遍百分之多少
	//结合公式，再根据CCR,ARR,PSC的值可知，此时配置的PWM波为1000Hz，占空比为50%，分辨率为1%
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM2,ENABLE);//这句相当于给TIM2使能，我的理解是相当于51中的ET0=1;吧
}
void PWM_SetCompare1(uint16_t Compare)
	/*
	这个函数是用来改变占空比的，虽然占空比是CCR和ARR+1共同决定的，但在此函数中由于ARR为100，
	根据公式，此时更改CCR的值即可更改占空比，而且其值就等于占空比的百分数

	*/
{
	TIM_SetCompare1(TIM2,Compare);
}

void PWM_SetPrescaler(uint16_t Prescaler)
{
	//这是通过改变PSC改变频率的，之所以不改ARR是因为该值与占空比有关
	TIM_PrescalerConfig(TIM2,Prescaler,TIM_PSCReloadMode_Update);//这是设置PSC的
	//TIM_PSCReloadMode_Immediate与TIM_PSCReloadMode_Update不同之处在于前者会马上改变PSC的值，后者在更新事件时改变，
	//前者具有即时性但会导致出现不完整周期，后者有所滞后但保证周期完整，在要求不高时，两者都可
	
}

