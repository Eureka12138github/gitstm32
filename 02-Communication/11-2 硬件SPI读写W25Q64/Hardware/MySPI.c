#include "stm32f10x.h"                  // Device header
#include "OLED.h"

//若需改变SPI模式，看11-3 18分左右
void MySPI_W_CS(uint8_t BitValue)//包装SPI四个通信引脚
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)BitValue);
}


void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//SPI1是APB2的外设
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//72MHZ/128，传输速度在这里改
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//这里与SPI模式有关，现在是模式0
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial = 7;//此为CRC校验的多项式，现因不用校验，故填默认值7
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//八位数据帧
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//配置为双线全双工
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//高位先行
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//配置为主机模式
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//软件模拟NSS
	SPI_Init(SPI1,&SPI_InitStructure);
	
	SPI_Cmd(SPI1,ENABLE);
	MySPI_W_CS(1);//默认不选中从机
}

void MySPI_Start(void)
{
	MySPI_W_CS(0);
}

void MySPI_Stop(void)
{
	MySPI_W_CS(1);
}

uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) != SET);//等待TXE为1
	SPI_I2S_SendData(SPI1,ByteSend);//发送字节
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) != SET);//等待RXNE为1，表示收到一个字节，此时表示发生也完成了。
	return SPI_I2S_ReceiveData(SPI1);//读取RDR接收的数据
	//此处标志位TXE与RNXE无需手动清除
	
}

//uint8_t MySPI_SwapByte_2(uint8_t ByteSend)//此函数与MySPI_SwapByte作用相同，但暂时不太理解
//{
//	此函数要结合PPT148页来理解，顺序是先取出 ByteSend 的最高位，
//	然后 ByteSend 左移一位，从机读取主机发来数据（ByteSend）最高位，即SCK上升沿到来
//	主机读取从机发来数据最高位，SCK置0，一次数据交换结束，如此循环八次，便可接收一个字节数据。
//	可能缺点在于ByteSend一直在变
//	uint8_t i;
//	for(i = 0;i<8;i ++)
//	{
//		//SPI模式0交换一个字节的时序
//		MySPI_W_MOSI(ByteSend & 0x80);
//		ByteSend <<= 1;
//		MySPI_W_SCK(1);//产生上升沿，从机自动取走MOSI数据
//		if(MySPI_R_MISO() == 1){ByteSend |= 0x01;}//读入从机数据
//		MySPI_W_SCK(0);//产生下降沿
//	}

//	return ByteSend;
//}

