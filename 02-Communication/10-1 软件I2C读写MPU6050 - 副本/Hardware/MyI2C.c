#include "stm32f10x.h"                  // Device header
#include "Delay.h"

//下面这三条函数是在进行引脚的封装，硬件引脚变化时，可来此改Pin
void MyI2C_W_SCL(uint8_t BitValue)//可以用宏定义或有参宏定义来指代对应引脚，此处这样定义函数的方式对于程序移植与主频较快情况有利
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_10,(BitAction)BitValue);
	Delay_us(10);//此处，对于STM32F1系列，MPU6050能“跟得上”引脚反转速度，此处延时实可不加，加是出于保险起见
}

void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_11,(BitAction)BitValue);
	Delay_us(10);
}

uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);
	Delay_us(10);
	return BitValue;
}


void MyI2C_Init(void)
{
	/*
	PB10、PB11被初始化为开漏输出模式，然后释放总线
	SCL、SDA处于高电平，此时I2C总线处于空闲状态
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;//配置成开漏输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);
}

void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);//先释放SDA是有原因的，如此可兼容起始条件与重复起始条件。
	/*
		如果先拉高SCL后拉高SDA，如原本SDA与SCL都是高电平时无事
		但在重复起始条件时，SDA是不定的，故有可能出现SCL高电平时，SDA从低到高的过程
		即，结束条件出现（240913是这么理解的。）
	*/
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i = 0; i < 8; i ++)
	{
	MyI2C_W_SDA(Byte & (0x80 >> i));//依次取出八位数据
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);	
	}
}

uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i,Byte = 0x00;
	MyI2C_W_SDA(1);
	for(i = 0; i < 8; i ++)
	{
	MyI2C_W_SCL(1);
	if(MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}
	MyI2C_W_SCL(0);
	}
	return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)
{

	MyI2C_W_SDA(AckBit);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);	

}

uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);//主机释放SDA，防止干扰从机；主机输出1，并不是强置SDA为高电平，而是释放SDA
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
}
