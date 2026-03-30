#include "stm32f10x.h"                 // Device header
#include "Delay.h"
int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//时钟开启，因为蜂鸣器接到了PB12口上，所以这里要把原来的GPIOA改为GPIOB
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//在推挽模式下，高低电平都有驱动能力，一般情况用推挽模式，特殊情况用开漏模式
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;//开漏输出模式下，高电平没有驱动能力，具体表现为，LED正极到GPIOA口，负极接到面包板负极时，灯不会亮
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;//同样，这里改为12
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);//对GPIOB初始化
	/*
	经过以上配置，可使用GPIO的库函数，见gpio.h文件后面的函数
	GPIO_SetBits可把指定端口设置为高电平
	GPIO_ResetBits可把指定端口设置为低电平
	GPIO_WriteBit前两个参数是用来指定端口的，可该函数可用来根据第三个参数的值来设置指定端口
	GPIO_Write此函数可同时对16个端口进行写入操作
	***使用库函数的方法：
	一、先打开gpio.h文件的最后看一下有那些函数，再点击鼠标右键跳转到函数的定义，查看函数的参数的用法
	二、查询江协提供的固件函数库用户手册，（在参考文档里），但此手册为老版本，所以某些函数的用法会和v3.5.0版本有所出入
		但整体上差异不大
	三、固件库中用一个名为“stm32f10x_stdperiph_lib_um”的帮助文档找到GPIO相关章节
	四、百度搜索，参考别人代码
	
	*/

	while(1)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(700);		
		
		//A15,B3,B4这三个端口是调试端口，若要将它们用作普通端口，需进行另外配置
	}
}//新建工程的具体步骤见江协的3-2视频的前小半部分，注意此行的中括号下面必须再有一空行，即在输入完}后需按回车，否则会有警告
