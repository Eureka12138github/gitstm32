#include "stm32f10x.h"                  // Device header
#include "Delay.h"
uint8_t Key_KeyNumber;
u8 Key_State;
/************************************************/
/*
	说明：该按键获取函数主要有两条，一是 Get_Key 、二是 Key_GetNum
	Get_Key需要配合定时器中断使用，要在主函数后面加上该函数末尾注释中的
	定时器中断函数；而 Key_GetNum 可以直接使用，不需要定时器中断函数，缺点是
	按键按下不放的话程序会卡死在当前位置，无法执行后续程序。
	
	241021：刚才发现了Key_GetNum的另一个缺点，那就是如果主函数中调用较多延时函数时，
	将会导致按键键码获取不及时的问题，例如：
			if(KeyNum == 1)
		{
			OLED_ShowString(3,1,"OK");
		}
//		OLED_ShowString(4,1,"FEED");
//		Delay_ms(200);
//		OLED_ShowString(4,1,"     ");
//		Delay_ms(600);
如果后面四行不注释掉的话，要按下老半天才会显示OK。配合定时器中断使用的 Get_Key 就不会有这样的问题
本来想把定时器初始化的函数一起放到按键初始化里面的，但是放到里面后不知道为什么按键刷新就变慢了好多
具体表现为，按下按键没反应，按好几次才识别过来，但是定时器单独初始化就灵敏一点。原因未知。
*/
/***********************************************/

uint8_t Get_Key(void)
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
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0){ KeyNumber=3;}
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
	if(LastState==0&&NowState==3)
	{
		Key_KeyNumber=3;
	}
//	if(LastState==0&&NowState==4)
//	{
//		Key_KeyNumber=4;
//	}
}

/**
* @brief 配置端口B1、端口B11，为上拉模式，初始化定时器
  * @param  无
  * @retval 无
  */
void Key_Init(void)
{
	//按键初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_11;//选择需要配置的端口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//在输入模式下，这里其实不用配置
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//选择需要配置的端口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//在输入模式下，这里其实不用配置
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0);
		Delay_ms(20);
		KeyNum=1;
		
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0);
		Delay_ms(20);
		KeyNum=3;
		
	}
	
	return KeyNum;
	
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




