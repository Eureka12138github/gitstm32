#include "stm32f10x.h"                  // Device header
uint16_t MyDMA_Size;
void MyDMA_Init(uint32_t AddrA,uint32_t AddrB,uint16_t Size)//不太明白为什么是uint32_t，其他的不行吗？为什么是这个？240806
{
	MyDMA_Size=Size;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr=AddrA;//外设站点地址
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;//以字节方式传输
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Enable;//地址自增
	DMA_InitStructure.DMA_MemoryBaseAddr=AddrB;//存储器站点地址
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;//以字节方式传输	
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//地址自增
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//传输方向为外设到存储器，DMA_DIR_PeripheralDST相反
	DMA_InitStructure.DMA_BufferSize=Size;//Size决定传输次数（这里不太理解）
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//自动重装和软件触发不能同时使用，否则DMA会连续触发，无法停止。见DMA_Mode定义中note
	DMA_InitStructure.DMA_M2M=DMA_M2M_Enable;//DMA_M2M_Enable即表示使用软件触发
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;	
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1,DISABLE);
	/*
	DMA转运三个条件：1、传输计数器大于0；
					 2、触发源有信号；
					 3、DMA使能；
	*/
}

void MyDMA_Transfer(void)
{
	DMA_Cmd(DMA1_Channel1,DISABLE);//给传输计数器赋值之前，先给DMA失能
	DMA_SetCurrDataCounter(DMA1_Channel1,MyDMA_Size);//重新给传输计数器赋值
	DMA_Cmd(DMA1_Channel1,ENABLE);//重新使能
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1)==RESET);//等待转运完成，即Flag为SET时，转运完成
	DMA_ClearFlag(DMA1_FLAG_TC1);//Flag置位后需手动清除
}
	
	
