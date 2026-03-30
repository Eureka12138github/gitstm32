#include "stm32f10x.h"                  // Device header
uint16_t AD_Value[4];
void AD_Init(void)
{
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//选择6分频，分频后ADCCLK=72MHZ/6=12MHZ，这里为什么这样配置暂时还不理解240513
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	/*
	选择模拟输入模式，该模式下GPIO口是无效的，相当于断开GPIO，防止GPIO口的输入对模拟电压
	造成干扰，AIN模式就是ADC的专用模式
	*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//配置ADC
	
	//关于采样时间，若需要更快的转换，则采用小的参数，需稳定则采用大的参数
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,4,ADC_SampleTime_55Cycles5);//"点菜"
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;//选择独立模式
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;//指定扫描模式为多通道
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;//指定转换模式为连续模式
	//ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//触发源选择软件触发
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//数据对齐选择右对齐
	ADC_InitStructure.ADC_NbrOfChannel=4;//通道数目为4，此参数在多通道模式下才真正起作用，单通道默认只有序列1位置有效
	ADC_Init(ADC1,&ADC_InitStructure);
	
	//配置DMA
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&ADC1->DR;//外设站点地址
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//以半字方式传输
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//地址不自增，始终转运同一个位置的数据
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)AD_Value;//"端菜"目的地
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//以半字方式传输	
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//地址自增，存储器地址是自增的，所以ENABLE
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//传输方向为外设到存储器，DMA_DIR_PeripheralDST相反
	DMA_InitStructure.DMA_BufferSize=4;//此处决定传输次数，这里为4是因为ADC通道有四个
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;
	/*自动重装和软件触发不能同时使用，否则DMA会连续触发，无法停止。
	见DMA_Mode定义中note。DMA_Mode_Circular为自动重装
	*/
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//DMA_M2M_Disable即表示使用硬件触发，触发源为ADC1
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;	
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);//此处的通道不可随意选择，不同外设对应不同通道，详见江协stm32PPT第92页
	DMA_Cmd(DMA1_Channel1,ENABLE);//DMA使能
	ADC_DMACmd(ADC1,ENABLE);//开启ADC到DMA的输出，没此语句显然无法正常运行
	ADC_Cmd(ADC1,ENABLE);//开启ADC电源
	//下面是ADC校准，这样会使得转换更加精确
	ADC_ResetCalibration(ADC1);//复位校准
	/*
	视频说这里可以再加一个定时器外设，ADC用单次扫描，再用定时器定时触发ADC，ADC触发DMA，
	如此便可实现完全自动，减少软件资源。不过现在我是不知道怎么弄，也不知道有没有必要节省
	这软件资源。以后再说吧。240806
	*/
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);//若复位校准未完成，则在此等待
	/*ADC_GetResetCalibrationStatus这个函数似乎是用来获取复位校准的标志位的，若在
	校准，则返回值为SET(即1)，否则返回RESET(即0)
	*/
	ADC_StartCalibration(ADC1);//开始校准
	while(ADC_GetCalibrationStatus(ADC1)==SET);//等待校准完成
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	/*
	当ADC触发后，ADC连续转换，DMA循环转运，两者一直工作，始终把最新结果刷新到SRAM数组里，
	如此便可在数组里取各个模拟量输入的数据，此时后面的void AD_GetValue(void)函数便可删去
	
	*/
}

/**
  * @brief 调用此函数，ADC开始转换，连续扫描4个通道，DMA也同时进行转运，ADC转换结果依次放到AD_Value数组中
  * @param  无
  * @retval 无
  */
//void AD_GetValue(void)
//{
//	DMA_Cmd(DMA1_Channel1,DISABLE);//给传输计数器赋值之前，先给DMA失能
//	DMA_SetCurrDataCounter(DMA1_Channel1,4);//重新给传输计数器赋值
//	DMA_Cmd(DMA1_Channel1,ENABLE);//重新使能
//	
//	//ADC_SoftwareStartConvCmd(ADC1,ENABLE);//因为还是单次模式，所以还需软件触发转换。（这里不太理解）
//	//因为ADC肯定在DMA之前，所以只需等待DMA转运完成即可
//	while(DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET);//等待转运完成，即Flag为SET时，转运完成
//	DMA_ClearFlag(DMA1_FLAG_TC1);//Flag置位后需手动清除
//}
