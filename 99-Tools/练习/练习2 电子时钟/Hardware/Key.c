#include "stm32f10x.h"                  // Device header
#include "Delay.h"
uint8_t Key_KeyNumber;
uint8_t Key(void)
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
* @brief 配置端口B1、端口B11，为上拉模式
  * @param  无
  * @retval 无
  */
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_11;//选择需要配置的端口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//在输入模式下，这里其实不用配置
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
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




