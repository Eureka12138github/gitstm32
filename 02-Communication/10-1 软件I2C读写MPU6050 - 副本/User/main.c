#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "MPU6050.h"
int16_t AX,AY,AZ,GX,GY,GZ,ID;
	float K1 = 0.1;
	float Dt = 0.001;
	float Pitch,Roll;
int main(void)
{

	OLED_Init();	
	MPU6050_Init();
	ID = MOU6050_GetID();
	OLED_ShowString(1,1,"ID:");
	OLED_ShowHexNum(1,4,ID,2);
//	MPU6050_WriteReg(0X6B,0X00);//�	�除睡眠模式
//	MPU6050_WriteReg(0X19,0Xaa);
//	
//	uint8_t ID = MPU6050_ReadReg(0X19);
//	OLED_ShowHexNum(1,1,ID,2);
	
	while(1)
	{
		/*
			换算：读取的数据/32768=x/满量程，此处具体介绍看10-2的16分
		*/
		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
		Roll = 
		OLED_ShowSignedNum(2,1,AX,5);
		OLED_ShowSignedNum(3,1,AY,5);
		OLED_ShowSignedNum(4,1,AZ,5);
		
		OLED_ShowSignedNum(2,8,GX,5);
		OLED_ShowSignedNum(3,8,GY,5);
		OLED_ShowSignedNum(4,8,GZ,5);
		
	}
}
float YJHB_Pitch(float angle_m,float gyro_m)
{
	Pitch = K1*angle_m + (1-K1)*(Pitch+gyro_m*Dt);
	return Pitch;
}

float YJHB_Roll(float angle_m,float gyro_m)
{
	Roll = K1*angle_m + (1-K1)*(Roll+gyro_m*Dt);
	return Roll;
}
	