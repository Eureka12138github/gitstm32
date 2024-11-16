#include "stm32f10x.h"                  // Device header
#include "Delay.h"
uint8_t Key_KeyNumber;
u8 Key_State;
/************************************************/
/*
	说明：该按键获取函数主要有两条，一是 Get_Key 、二是 Key_GetNum
	Get_Key需要配合定时器中断使用，要在主函数后面加上该函数末尾注释中的
	定时器中断函数；而 Key_GetNum 可以直接使用，不需要定时器中断函数，缺点是
	按键按下不放的话程序会卡死在当前位置，无法执行后续程序。
	
	241021：刚才发现了Key_GetNum的另一个缺点，那就是如果主函数中调用较多延时函数时，
	将会导致按键键码获取不及时的问题，例如：
			if(KeyNum == 1)
		{
			OLED_ShowString(3,1,"OK");
		}
//		OLED_ShowString(4,1,"FEED");
//		Delay_ms(200);
//		OLED_ShowString(4,1,"     ");
//		Delay_ms(600);
如果后面四行不注释掉的话，要按下老半天才会显示OK。配合定时器中断使用的 Get_Key 就不会有这样的问题

*/
/***********************************************/

uint8_t Get_Key(void)
{
	uint8_t Temp=0;
	Temp=Key_KeyNumber;
	Key_KeyNumber=0;//不太明白，为什么要把Key_KeyNumber清零。因为不把它清零的话，在下一个不同的值来临前它就一直是这个值
	//具体表现为，按下按键1后，返回1，但即时是松开了，还是返回1，直到下一个不同值到来才能切换值，
	return Temp;
}


uint8_t Key_GetState(void)
{
	uint8_t KeyNumber=0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0){ KeyNumber=1;}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0){ KeyNumber=2;}
//	if(GPIO_ReadInputDataBit(GPIOX,GPIO_Pin_X)==0){ KeyNumber=3;}
//	if(GPIO_ReadInputDataBit(GPIOX,GPIO_Pin_X)==0){ KeyNumber=4;}
	return KeyNumber;
}

void Key_Loop(void)
{
	static uint8_t NowState,LastState;
	LastState=NowState;
	NowState=Key_GetState();
		/*
	现在还不太明白，但是，这样可以实现，LastState==1&&NowState==0时，按键按下松手后有效，
	若1和0交换位置，即LastState==0&&NowState==1时就变成了按下按键即有效。其实就是松手时
	决定有效还是按下时决定有效的区别。240928明白了，就是前后状态的判断问题。
	
	*/
	if(LastState==0&&NowState==1)

	{
		Key_KeyNumber=1;
	}
	if(LastState==0&&NowState==2)
	{
		Key_KeyNumber=2;
	}
//	if(LastState==0&&NowState==3)
//	{
//		Key_KeyNumber=3;
//	}
//	if(LastState==0&&NowState==4)
//	{
//		Key_KeyNumber=4;
//	}
}

/**
* @brief 配置端口B1、端口B11，为上拉模式，初始化定时器
  * @param  无
  * @retval 无
  */
void Key_Init(void)
{
	//按键初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_11;//选择需要配置的端口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//在输入模式下，这里其实不用配置
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//定时器初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//tIM2是APB1总线的外设，所以要开启APB1时钟
	
	TIM_InternalClockConfig(TIM2);//这句表明TIM2的时基单元由内部时钟驱动，系统默认由内部时钟驱动，不写这句也行
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//TIM_ClockDivision与滤波的频率有关，决定是否对内部时钟进行分频，需滤波强一点，就TIM_CKD_DIV2或者TIM_CKD_DIV4
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//此处高级计数器才用到，故直接给零了
	TIM_TimeBaseInitStructure.TIM_Period=10-1;//ARR,减一是因为预分频器和计数器都有1个数的偏差
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//这里是设置计数器的计数模式的，设置为向上计数
	TIM_TimeBaseInitStructure.TIM_Prescaler=7200-1;//这里是设置预分频器的主频的“切分”，也就是把主频“切”成7200份
	/*
	时基单元：PSC(预分频器)、ARR(自动重装器)、CNT(计数器)
	计数器溢出频率:CK_CNT_OV=CK_CNT/(ARR+1)=CK_PSC/(PSC+1)/(ARR+1)
	害，实际要计算每次计数时间就相当于1/(接到分频器上的频率(Mhz)*1000000/预分频器的值-1/重装的值-1)
	例如：现在我接到PSC上的频率为单片机的主频即72Mhz，而PSC的值为7200，即进行7200分频，重装值为10000，即
	此时根据公式可得：1/(72000000/7200-1+1/10000-1+1)=1s，此处的加一应为硬件自动加的
	PSC预分配器，对主频72Mhz进行分频，为0表示不分频，即还是72Mhz,为1表示二分频，即72Mhz/2
	故在此处表示的是进行7200分频，即72Mhz/7200，但不能直接写7200，根据其规则，应该要写7200-1
	它后面会自己“+1”从而把“-1”给抵消掉
	另：PSC和ARR都有自己的缓冲寄存器，它们的目的是为了在中断更改相应值的时候不会出现错误，用或不用，可自己设置
	具体见江协视频6-1 的39分左右
	另：PSC与ARR的取值范围是0~65535，不能超范围，若预PSC小点，ARR大点，那就是以较高的频率计较多的数
	若若预PSC大点，ARR小点，那就是以较低的频率计较少的数，但结果是不变的，例如在这里我可以
	*/
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化通用定时器TIM2的时基单元
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	/*
	加了上面这句，复位后数才从0开始，原问题来自于TIM_TimeBaseInit函数带来的副作用，
	即更新事件和更新中断是同时发生的，更新中断会置中断标志位，初始化一旦完成了，更新中断就会立刻进入
	这就是刚上电就进入中断的原因（依旧不太懂。。。）
	
	04042218的理解：这个函数的作用是将TIM2的时基单元进行初始化的，上面利用结构体已经将PSC和ARR的值进行进行重装
	但是这PSC和ARR都是有缓冲寄存器的，真正起作用的也是这两个缓冲寄存器，故想对PSC和ARR的值进行重装，就需要改变他们缓冲
	寄存器的值，而它们的缓冲寄存器只有在更新事件时才会改变，但是刚上电时并没有更新事件，所以需要手动写一个更新事件，就是
	这句“TIMx->EGR = TIM_PSCReloadMode_Immediate; ”，这样PSC和ARR的值才会在初始化的时候马上重装，没有这句的话只能等到下次
	更新事件到来时，才会真正重装，这样初始化就晚了，相当于“如初”。但人为地使更新事件来临会有一个缺点，那就是在上电初始化的
	一瞬间就会就会进入中断，因为更新事件和更新中断是同时发生的，所以每次复位的一瞬间就会进入一次中断，故在这个程序中OLED显示
	的Num一开始的值会是1而不是0，因为在复位的一瞬间已经进过一次中断了。这句“TIM_ClearFlag(TIM2,TIM_FLAG_Update);”是用来
	清除更新中断标志位的，这样在初始化时PSC和ARR的值能马上重装而又不会触发多余的一次中断了。（总算是理解了其中原理了，废了老
	大劲了，那个UP是当真厉害，这是从根本上把问题解决了，如果是我的话，可能在OLED显示时直接显示Num-1吧。。。）
	另：- **更新中断**：计数器达到自动重装载值时，触发的中断，需要通过中断控制器通知处理器。
		- **更新事件**：计数器达到自动重装载值时发生的内部事件，可以触发其他定时器操作。
		两者通常同时发生，但更新中断需要处理器的中断服务程序来处理，而更新事件可以由定时器内部逻辑处理。

	
	*/
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//开启了更新中断到NVIC的通路，这里不太理解，为什么是更新中断？
	/*
	因为此处需要定时器触发中断而并非事件，另外这个函数是用来使能中断的，即“告诉”TIM2，你可以进行中断了
	*/
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
/*
	凡是中断都得由NVIC管理，所以这里它又出现了
	其配置都是有套路的，基本和GPIO的配置差不多，
	需要搞明白的是结构体中的各个参数，要理解它们对应的功能
	结合不同应用场景对它们进行相对应的配置

*/	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;//这句是找到TIM2在NVIC中的中断通道，不同类型的中断，在NVIC中的中断名称也不一
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//这句是将该通道打通
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//这句和下一句是配置优先级的为什么这样配我现在也不清楚
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	//上面优先级配置我感觉影响不大，因为只有一个中断源
	NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM2,ENABLE);
	
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0);
		Delay_ms(20);
		KeyNum=1;
		
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0);
		Delay_ms(20);
		KeyNum=2;
		
	}
	
	return KeyNum;
	
}
/**现在按键要配合定时器使用！放在主函数后面，而主函数直接调用Key();函数即可获取按键值
	void TIM2_IRQHandler(void)
{
	static uint8_t T0Count1;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		T0Count1++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//这句还搞不太懂，只有一个事件时应该可以直接清零把？
	}
	
	if(T0Count1>=20)
	{
		T0Count1=0;
		Key_Loop();//这个函数不能出现Delay等耗时语句，否则定时器会出错
	}
}
**/




