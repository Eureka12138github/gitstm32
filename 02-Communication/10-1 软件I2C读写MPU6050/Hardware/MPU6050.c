#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"
#define MPU6050_Address		0XD0

void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data)
{
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_Address);//从机地址
	MyI2C_ReceiveAck();//接收应答，此处可进行下一步判断。所谓应答，无论发送或接收，应答了（即1），便能继续，反之则停止
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(Data);
	MyI2C_ReceiveAck();
	MyI2C_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_Address);//从机地址
	MyI2C_ReceiveAck();//接收应答，此处可进行下一步判断
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_Address | 0x01);// |后变为0XD1，为读
	MyI2C_ReceiveAck();
	Data = MyI2C_ReceiveByte();
	MyI2C_SendAck(1);//不给从机应答，多个字节时，需给应答，从机便会继续发送数据
	MyI2C_Stop();
	return Data;
	
}
/**
* @brief 接触睡眠，选择陀螺仪时钟，6个轴均不待机，采样分频为10，滤波参数最大，陀螺仪和加速度计均最大量程
  * @param  无
  * @retval 无
  */
void MPU6050_Init(void)
{
	MyI2C_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);//此处配置要参考MPU6050寄存器手册
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);//此处表示10分频
	MPU6050_WriteReg(MPU6050_CONFIG,0x06);//此处涉及外部同步与低通滤波器
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);//此处涉及陀螺仪配置，现在配置（0x18）为满量程，
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);//此处涉及加速度配置量程及高通滤波器，现在配置（0x18）为满量程，//加速度计最大量程为16g，也就是32768对应16g
	
	//45:19
}

//以下此函数涉及多参返回，三种方法，一、定义全局变量；二、指针；三、结构体；
//以下为第二种方法，通过指针，把主函数变量的地址传递到子函数中，子函数中通过传递过来的地址，操作主函数变量
void MPU6050_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ)
{
	uint8_t DataH,DataL;
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH<<8) | DataL;//八位数据左移之后会自动进行类型转换，左移的为不会丢失
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH<<8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH<<8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH<<8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH<<8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH<<8) | DataL;
}

uint8_t MOU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}
