#include "stm32f10x.h"                  // Device header
#include "OLED.h"

//若需改变SPI模式，看11-3 18分左右
void MySPI_W_CS(uint8_t BitValue)//包装SPI四个通信引脚
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)BitValue);
}

void MySPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,(BitAction)BitValue);
}

void MySPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_7,(BitAction)BitValue);
}

uint8_t MySPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
}
void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;//输出（片选，时钟，主机输出）为推挽，输入为浮空或上拉
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//输出（片选，时钟，主机输出）为推挽，输入为浮空或上拉
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	MySPI_W_CS(1);//片选置高电平，初始化完成后默认不选中从机
	MySPI_W_SCK(0);//使用模式0
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
	uint8_t i,ByteReceive=0;
//	temp = ByteReceive;
//	OLED_ShowHexNum(4,1,temp,2);
	/*
	通过弹幕终于把问题解决了！
	问题描述如下：
	ByteReceive不初始为0时，存入W25Q64的数据会+8，比如存入0X01，取出却变成了0X09
	对于这个问题，暂时还没明白。（240915）解决方法是定义时给ByteReceive置0；
	尝试过自己排查问题，未能成功，我首先用子程序替换的方法排查问题，用正确的子程序
	把可能错误的一个个换过来，但我把MySPI、W25Q64甚至连main函数都换成UP的了，结果还是
	错误的结果；直接复制过来，如过不加修改的话，即使文件名完全相同，也不行，它似乎是认准了
	之前错误的版本了，只有重修修改一下程序，更新一下，它才能正确编译。哪怕是先把ByteReceive=0
	中的 =0 删去，再重新输入 =0 ，虽然前后并没有差别，但是编译器似乎能识别到我做了相应的修改，
	这时候它才能按已更正过来的子程序进行编译。
	按往常的设想，就算不给ByteReceive置0，它应该也是默认为0的啊！这次似乎默认为了8？为什么？
	还有如果这样："uint8_t i,ByteReceive;"，存入0x01读出必是0x09。
	但，若是：
	“uint8_t i,ByteReceive,temp;
	temp = ByteReceive;
	OLED_ShowHexNum(4,1,temp,2);”
	这时候即时没有将ByteReceive初始化为0，却也能正确输出。
	而若是：
	“uint8_t i,ByteReceive,temp;”
	却又不行了！！！！有什么区别吗？！
	我本来想temp=0，作为一个中间变量，然后紧接着
	temp= ByteReceive ，之后再用OLED显示一下temp，看看ByteReceive到底被初始化为了何值，但是这时候OLED显示
	的却是3，而非想象中的8。这个问题如果要我自己排查的话，估计是根本排不出来的，问题在于就算程序正确，它也不能得
	正确结果，这是最致命的。幸亏弹幕有啊！以后定义变量，保险起见，最好都置0吧（240915）
	至于为什么必须将ByteReceive置0，还没想明白。
	
	
	*/
	for(i = 0;i<8;i ++)
	{
		//SPI模式0交换一个字节的时序
		MySPI_W_MOSI(ByteSend & (0x80 >> i));
		MySPI_W_SCK(1);//产生上升沿，从机自动取走MOSI数据
		if(MySPI_R_MISO() == 1){ByteReceive |= (0x80 >> i);}//读入从机数据
		MySPI_W_SCK(0);//产生下降沿
	}

	return ByteReceive;//11-3 17分
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

