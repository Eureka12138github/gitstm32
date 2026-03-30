#include "stm32f10x.h"                 // Device header
#include "Delay.h"
int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//时钟开启
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//在推挽模式下，高低电平都有驱动能力，一般情况用推挽模式，特殊情况用开漏模式
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;//开漏输出模式下，高电平没有驱动能力，具体表现为，LED正极到GPIOA口，负极接到面包板负极时，灯不会亮
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	//上面这句也可这样：GPIO_InitStructure.GPIO_Pin=GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	/*
	经过以上配置，可使用GPIO的库函数，见gpio.h文件后面的函数
	GPIO_SetBits可把指定端口设置为高电平
	GPIO_ResetBits可把指定端口设置为低电平
	GPIO_WriteBit前两个参数是用来指定端口的，可该函数可用来根据第三个参数的值来设置指定端口
	GPIO_Write此函数可同时对16个端口进行写入操作
	
	
	*/
	GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
	while(1)
	{
		GPIO_Write(GPIOA,~0X0001);//0000 0000 0000 0001,此处加~是因为低电平点亮LED
		Delay_ms(100);
		GPIO_Write(GPIOA,~0X0002);//0000 0000 0000 0010
		Delay_ms(100);		
		GPIO_Write(GPIOA,~0X0004);//0000 0000 0000 0100
		Delay_ms(100);
		GPIO_Write(GPIOA,~0X0008);//0000 0000 0000 1000
		Delay_ms(100);	
		GPIO_Write(GPIOA,~0X0010);//0000 0000 0001 0000
		Delay_ms(100);
		GPIO_Write(GPIOA,~0X0020);//0000 0000 0010 0000
		Delay_ms(100);		
		GPIO_Write(GPIOA,~0X0040);//0000 0000 0100 0000
		Delay_ms(100);
		GPIO_Write(GPIOA,~0X0080);//0000 0000 1000 0000
		Delay_ms(100);		
		//A15,B3,B4这三个端口是调试端口，若要将它们用作普通端口，需进行另外配置
	}
}//新建工程的具体步骤见江协的3-2视频的前小半部分，注意此行的中括号下面必须再有一空行，即在输入完}后需按回车，否则会有警告
