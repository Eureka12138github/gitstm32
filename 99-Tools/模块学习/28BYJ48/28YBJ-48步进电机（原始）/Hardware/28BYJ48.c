#include "stm32f10x.h"                  // Device header
#include "Delay.h"
uint16_t phasecw[4] = {0x8000,0x4000,0x2000,0x1000};
uint16_t phaseccw[4] = {0x1000,0x2000,0x4000,0x8000};
void SMotor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);//表示刚开始时，A1、A2口为高电平，根据接线，此时不亮
}

void MotorCW(uint16_t Speed)
{
 uint8_t i;
 for(i=0;i<4;i++)
  {
	
   GPIO_Write(GPIOB,phasecw[i]);	  
   Delay_us(Speed);//转速调节
  }
}

void MotorCCW(void)
{
 uint8_t i;
 for(i=0;i<4;i++)
  {
   GPIO_Write(GPIOB,phaseccw[i]);  
  
   Delay_ms(2);//转速调节
  }
}

void MotorStop(void)
{
 GPIO_Write(GPIOB,0);
}

/*
ULN2003驱动5V减速步进电机程序
Target:STC89C52RC-40C
Crystal:12MHz
Author:战神单片机工作室
Platform:51&avr单片机最小系统板+ULN2003步进电机驱动套件
http://zsmcu.taobao.com   QQ:284083167
*******************************************************
接线方式:
IN1 ---- P10//原来是P00开始的，但普中板是P1
IN2 ---- P11
IN3 ---- P12
IN4 ---- P13
+   ---- +5V
-   ---- GND
*********************
#include <REGX52.H>
#define uchar unsigned char
#define uint  unsigned int
#define MotorData P1                   //步进电机控制接口定义
uchar phasecw[4] ={0x08,0x04,0x02,0x01};//正转 电机导通相序 D-C-B-A
uchar phaseccw[4]={0x01,0x02,0x04,0x08};//反转 电机导通相序 A-B-C-D
//ms延时函数
void Delay_xms(uint x)
{
 uint i,j;
 for(i=0;i<x;i++)
  for(j=0;j<112;j++);
}
//顺时针转动
void MotorCW(void)
{
 uchar i;
 for(i=0;i<4;i++)
  {
   MotorData=phasecw[i];
   Delay_xms(2);//转速调节
  }
}
//逆时针转动
void MotorCCW(void)
{
 uchar i;
 for(i=0;i<4;i++)
  {
   MotorData=phaseccw[i];
   Delay_xms(2);//转速调节
  }
}
//停止转动
void MotorStop(void)
{
 MotorData=0x00;
}
//主函数
void main(void)
{
// uint i;
 Delay_xms(50);//等待系统稳定
 while(1)
 {
//  for(i=0;i<500;i++)
//  {
//   MotorCW();   //顺时针转动
//  } 
//  MotorStop();  //停止转动
//  Delay_xms(500);
//  for(i=0;i<500;i++)
//  {
//   MotorCCW();  //逆时针转动
//  } 
//  MotorStop();  //停止转动
//  Delay_xms(500);  
	 MotorCW();
 }
}
*/
