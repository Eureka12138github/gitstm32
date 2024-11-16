#include "stm32f10x.h"                 // Device header
#include "Delay.h"
int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//时钟开启
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//在推挽模式下，高低电平都有驱动能力，一般情况用推挽模式，特殊情况用开漏模式
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;//开漏输出模式下，高电平没有驱动能力，具体表现为，LED正极到GPIOA口，负极接到面包板负极时，灯不会亮
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	/*
	经过以上配置，可使用GPIO的库函数，见gpio.h文件后面的函数
	GPIO_SetBits可把指定端口设置为高电平
	GPIO_ResetBits可把指定端口设置为低电平
	GPIO_WriteBit前两个参数是用来指定端口的，可该函数可用来根据第三个参数的值来设置指定端口
	GPIO_Write此函数可同时对16个端口进行写入操作
	
	
	*/
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_SET);
	while(1)
	{
//		GPIO_ResetBits(GPIOA,GPIO_Pin_0);
//		Delay_ms(100);
//		GPIO_SetBits(GPIOA,GPIO_Pin_0);
//		Delay_ms(100);
//		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
//		Delay_ms(500);
//		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
//		Delay_ms(500);
		
		GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)0);
		Delay_ms(1000);
		GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)1);
		Delay_ms(1000);
	}
}//新建工程的具体步骤见江协的3-2视频的前小半部分，注意此行的中括号下面必须再有一空行，即在输入完}后需按回车，否则会有警告
