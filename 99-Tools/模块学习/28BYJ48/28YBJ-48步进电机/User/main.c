#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "28BYJ48.h"
#include "Serial.h"
#include "Timer.h"
// 定义步进电机速度，值越小，速度越快
// 最小不能小于1
#define STEPMOTOR_MAXSPEED        1  
#define STEPMOTOR_MINSPEED        5  	
u8 step = 0;
u8 dir = 0;//默认逆时针方向
u8 speed = STEPMOTOR_MAXSPEED;//默认最大速度旋转
float Round = 4096;//延时4096ms时可使步进电机转动360°（此值需根据实际情况进行适当调整）
u8 Run = 0;//判断标志位，与设定角度的转动有关
u8 SerialData = 0;//接收到的串口数据
u8 Mannul = 0;//判断标志位，与手动模式有关
float time = 0;
int main(void)
{
	float Angle = 0;
	OLED_Init();
	Motor_Init();
	Timer_Init();
	Serial_Init();
	while(1)
	{
		if(Serial_GetRxFlag()==1)
		{
		SerialData=Serial_GetRxData();
		OLED_ShowNum(50,40,SerialData,2,OLED_8X16);
		OLED_Update();	
		}
		
		if(SerialData == 1)//换向
		{
			dir=!dir;
			SerialData = 0;
					
		}
		else if(SerialData == 2)//加速
		{
			if(speed>STEPMOTOR_MAXSPEED) {speed-=1;}
			else if(speed == 0) {speed = 1;}
			SerialData = 0;
				
		}
		else if(SerialData == 3)//减速
		{
			if(speed<STEPMOTOR_MINSPEED)
			{
			 speed+=1;
					
			}
			SerialData = 0;
			
		}
		else if(SerialData == 4)//停止
		{
			speed = 0;
			GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
			OLED_ShowString(0,20,"              ",OLED_8X16);
			SerialData = 0;
		}
		
		if(SerialData == 5)
		{
			Run = SerialData;
			SerialData = 0;
		}
		time =  (Round/360.0)*Angle;//计算延时时间
		if(Run == 5)//若条件成立，则以设定角度转动
		{
			OLED_ShowString(48,0," ",OLED_8X16);
			OLED_ShowString(48,0,"1",OLED_8X16);
			OLED_ShowString(95,0,"  ",OLED_8X16);
			OLED_ShowString(82,0,"ON",OLED_8X16);
			OLED_Update();
			Delay_ms(time);
			Run = 0;
			GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
		}	
		
		if(SerialData == 6)//当SerialData为6时，进行设定角度加1操作
		{
			Angle += 1;
			if(Angle>360){Angle = Angle -360;}//边界判断
			SerialData = 0;//操作玩一次后需要将SerialData清零，否则下一次串口数据到来前，此if语句将一直成立
			
		}
		
		if(SerialData == 7)//当SerialData为7时，进行设定角度减1操作
		{
			Angle -= 1;
			if(Angle<0){Angle = Angle+361;}
			SerialData = 0;
			
		}
		
		if(SerialData == 8)//当SerialData为8时，进行设定角度加10操作
		{
			Angle += 10;
			if(Angle>360){Angle = Angle -360;}
			SerialData = 0;
			
		}
		
		if(SerialData == 9)//当SerialData为9时，进行设定角度减10操作
		{
			Angle -= 10;
			if(Angle<0){Angle = Angle+360;}
			SerialData = 0;
			
		}
		
		if(SerialData == 0X0A)//当SerialData为10时，电机转动/停止
		{
			Mannul =~Mannul;
			if(Mannul!=0)
			{
				speed = 1;
			}
			else if(Mannul == 0)
			{
				speed = 0;
			}
			SerialData = 0;
			
		}
		OLED_ShowNum(0,40,Angle,3,OLED_8X16);//显示当前设定的角度，默认为0
		OLED_ShowString(0,0,"Speed:",OLED_8X16);//速度显示

		if(speed!=0)//模式显示
		{
			OLED_ShowString(95,0,"  ",OLED_8X16);
			OLED_ShowString(82,0,"ON",OLED_8X16);
		}
		else 
		{
			OLED_ShowString(82,0,"OFF",OLED_8X16);
		}
		OLED_ShowNum(48,0,speed,1,OLED_8X16);
		
		
		if(dir == 0)//方向显示
		{
			OLED_ShowString(0,20,"Direction:",OLED_8X16);
			OLED_ShowString(95,20,"  ",OLED_8X16);
			OLED_ShowString(82,20,"CW",OLED_8X16);
			
		}
		else if(dir == 1)
		{
			OLED_ShowString(0,20,"Direction:",OLED_8X16);
			OLED_ShowString(82,20,"ACW",OLED_8X16);
		}
		OLED_Update();
	}
}
void TIM2_IRQHandler(void)
{
	static unsigned int T0Count;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
	T0Count++;
	if(T0Count>=speed) 
	{
		T0Count=0;
		if((speed!=0)||((Run == 5)&&(time!=0))||((Mannul!=0)&&(speed == 0)))
		/**
		//三种情况电机可动，speed不为0时，（此时为自动），Run == 5时，（此时为设定角度后的转动），
		而Mannul表示手动，且自动关闭时，（即speed为0时），可手动使电机运行，但为不设定角度的转动。
		**/
		{
		step_motor_28BYJ48_send_pulse_8(step++,dir);//这里是步进电机的转动模式选择，可以在28BYJ48.h里面选择其他两种工作方式
		if(step==8)step=0;	//这里要根据工作拍数修改		
		}	
		
	}
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}


