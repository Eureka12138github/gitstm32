#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "Key.h"
#include "OLED.h"
#include "Servo.h"
/*
	在ARR=20000,PSC=72,时即可满足舵机要求周期，即20ms，即50Hz
	此时改变CRR，便可输出不同类型的PWM波，根据舵机不同方向要求不同的占空比
	设置不同CRR便可使舵机转动不同方向。西方向时CRR为500是这样算出来的：
	CRR*20/(ARR+1)=0.5ms,由于已知ARR=20000，于是可算出CRR为500(+1就当是忽略了吧，对整体影响不大？)
	
*/
//uint8_t Num=1;
//uint16_t i=500,pwmValues[] = {500, 1000, 1500, 2000, 2500, 2000, 1500, 1000, 500};
//int main(void)
//{
//	
//		OLED_Init();	
//		Key_Init();
//		PWM_Init();
//	while(1)
//	{
//		Num=Num+Key_GetNum();
//		if(Num==1)
//		{
//		PWM_SetCompare2(500);
//		}
//		if(Num==2)
//		{
//		PWM_SetCompare2(1000);
//		}
//		if(Num==3)
//		{
//		PWM_SetCompare2(1500);
//		}
//		if(Num==4)
//		{
//		PWM_SetCompare2(2000);
//		}
//		if(Num==5)
//		{
//		PWM_SetCompare2(2500);
//		}
//		if(Num==6)
//		{
//		PWM_SetCompare2(2000);
//		}
//		if(Num==7)
//		{
//		PWM_SetCompare2(1500);
//		}
//		if(Num==8)
//		{
//		PWM_SetCompare2(1000);
//		}
//		if(Num==9)
//		{
//		PWM_SetCompare2(500);
//		}
//		if(Num==10)
//		{
//		Num=2;
//		}
		// 假设pwmValues数组已经定义，并包含了相应的PWM值


//Num = Num + Key_GetNum();
//if (Num < 10) // 如果Num的值在1到10之间，则从数组中获取相应的PWM值
//	{

//    PWM_SetCompare2(pwmValues[Num - 1]);
//	}
//	else if (Num==10)// 如果Num的值超过10，则重置Num为2
//	{
//    Num = 2;
//	}
////有空用51驱动舵机玩玩
//		
//	}
//}
int main(void)
{
	uint8_t Num;
	float Angle=-5;
	Key_Init();
	Servo_Init();
	OLED_Init();
	OLED_ShowString(1,1,"Angle:");
	OLED_ShowNum(1,7,Angle+5,3);
	Servo_SetAngle(Angle);
	
	while(1)
	{
		Num=Key_GetNum();
		/*
			以下由于舵机硬件问题，无法正常从零度开始，而到了180度还会往前，
		所以，人为地调了一下，发现设置80度时现实中目视舵机为90度，也就是原来0度向上抬了
		10度，180向下沉了10，通过程序判断，现在程序中设置的角度基本与现实角度对应了，但是
		刚开始时，舵机实际上还是10的，即使程序中已经设置为0度，这是硬件问题，无法改变，
		但还是决定显示为0度。对以下程序稍作修改，舵机即可正确转向10~180度。
		原因未知，若果将Angle设置为-5，那么目视角度是零度，（按照上面的情况应该是-10的时候为零度才对啊）
		但确实是在-5的时候为零度，而在20时为30度，80时为90度，170时为180度
		*/
		if(Num==1)
		{
			if(Angle==-5)
			{
			Angle+=25;	
			}
			else
			{
			Angle+=30;	
			}
		if(Angle>180)
		{
			Angle=-5;
			
		}
		Servo_SetAngle(Angle);
		OLED_ShowNum(1,7,Angle+5,3);
		}
/*以下这段程序是为了测试新焊的板子能否正常工作，控制舵机的。结果可以，有点开心。
	if(Angle<180)
	{
	Angle+=30;
	Delay_s(3);		
	}
	else Angle=-5;
	Servo_SetAngle(Angle);
	*/
	}
}
