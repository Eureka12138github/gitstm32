#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "28BYJ48.h"
uint16_t speed;
int main(void)
{
	OLED_Init();
	SMotor_Init();
	speed = 2000;
	Delay_ms(100);//等待系统稳定
	while(1)
	{
		
//		  Delay_ms(100);//等待系统稳定
//		 
//		  for(i=0;i<500;i++)
//		  {
//		   MotorCW();   //顺时针转动
//		  } 
//		  MotorStop();  //停止转动
//		  Delay_ms(500);
//		  for(i=0;i<500;i++)
//		  {
//		   MotorCCW();  //逆时针转动
//		  } 
//		  MotorStop();  //停止转动
//		  Delay_ms(500); 
		  uint16_t i;
		  
		OLED_ShowNum(0,0,speed,4,OLED_8X16);
		OLED_Update();	
		for(i=0;i<1000;i++)
		  {
		   MotorCW(speed);  //顺时针转动 
		  }  
		  speed--;     //减速 
		if(speed<500)  
		  {
		   speed=2000;    //重新开始减速运动
		   MotorStop();
		   Delay_ms(500);
		  } 
	
	}
}
