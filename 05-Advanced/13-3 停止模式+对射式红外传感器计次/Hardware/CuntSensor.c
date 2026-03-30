#include "stm32f10x.h"                  // Device header
#include "Delay.h"
uint16_t CountSensor_Count;
void CountSensor_Init(void)
{
	/*需打开外设的时钟，即GPIO，AFIO,EXTI,NVIC,前两个是需要自己配置打开的，但是后两个似乎默认打开，不需要配置*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//这是打开时钟的
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//若不清楚该配置为什么模式，可以看参考手册中的“通用和复用功能I/O的8.1.11”
	/*
	1. 四种输入模式

    GPIO_Mode_IN_FLOATING 浮空输入模式
    GPIO_Mode_IPU 上拉输入模式，即默认为高电平
    GPIO_Mode_IPD 下拉输入模式，即默认为低电平
    GPIO_Mode_AIN 模拟输入模式

	2. 四种输出模式

    GPIO_Mode_Out_OD 开漏输出模式
    GPIO_Mode_Out_PP 推挽输出模式
    GPIO_Mode_AF_OD 复用开漏输出模式
    GPIO_Mode_AF_PP 复用推挽输出模式
*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);//AFIO选择GPIOB的第十四号引脚，后续固定接到EXTI的EXTI14上
	/*
	FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line);
	void EXTI_ClearFlag(uint32_t EXTI_Line);
	ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);
	void EXTI_ClearITPendingBit(uint32_t EXTI_Line);
	查看标志位的函数，前两个在主函数中使用，后两个在中断函数中使用
	*/
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line14;//这个是选线的
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;//这个是让所选线是否开启的
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;//这是选中断模式的，分为中断，或事件
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//这个是选触发方式的分为，下降沿，上升沿，和双边沿//未知原因，只能上升沿和下降沿都触发
	/*原来是GPIO_Mode_IPU时只能EXTI_Trigger_Rising_Falling，后来改为GPIO_Mode_IPD变为正常，现在仍未正常，貌似会抖动不稳定
	又不能加延时消抖，日后学到定时器或许能解决这个问题？*/
	EXTI_Init(&EXTI_InitStructure);//中断初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//这是中断分组的，整个工程只需执行一次，亦可放到主函数最开始
	NVIC_InitTypeDef NVIC_InitStructure;//定义NVIC初始化的结构体名称
	NVIC_InitStructure.NVIC_IRQChannel=EXTI15_10_IRQn;//选择芯片的中断通道每个芯片有专属的中断通道，本芯片选后缀为MD的
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//这是决定选择的通道是否使能的
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//这是配置抢占优先级的，其取值范围要根据前面分组字再结合NVIC的表格决定
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;//这是配置响应优先级的，其取值范围要根据前面分组字再结合NVIC的表格决定
	//在此工程中，由于只有一个中断源，所以此处优先级配置也比较随意
	NVIC_Init(&NVIC_InitStructure);//NVIC初始化
}
uint16_t CountSensor_Get(void)
{
	return CountSensor_Count;
}
void EXTI15_10_IRQHandler(void)//在STM32中每个中断通道都对应一个固定的中断函数名称，这些名称需要在Start文件中以md.s为
								//后缀的文件中找到以IRQHandler结尾的字符串就是中断函数名称，在此工程中因为选的是外部中断14，
								//所以要选EXTI15_10_IRQHandler、（即中断10~15都选这个名称）
{
	if(EXTI_GetITStatus(EXTI_Line14)==SET)	//这个是通过中断标志位查询函数来进一步确认中断是否从通道14来的，SET就是1？
											//这个现在还不确定，反正就是中断来了的意思
		{
		//		Delay_s(1);加了延时的话如果把遮光片快速抽出能实现只加一次，但若遮光片放久点再拿出还是会在拿出后再进行加一
			//新现象：在14口接入一个放光二极管后就稳定很多，负极接14口，正极接电源负极，原因未知，类似于起到了滤波电容的作用？
			//经测试，在下拉模式且接上二极管，且在下降沿与双边沿时较为稳定，上升沿现在无法实现!!!原因未知！！！猜测可能是下拉能力不强
			//在下降为低电平的时候跳到了3.3v而红外口给IO的电平是2.6V，导致在放入挡光片和拿出的时候出现了两次上升沿？不应该啊、别人的
			//为什么上升沿触发正常？难道是我红外对射模块有问题？
			EXTI_ClearITPendingBit(EXTI_Line14);//这是用来清除中断标志位的，不清除的化就会一直响应中断，程序就会卡死在此
			CountSensor_Count++;		
		}
}
