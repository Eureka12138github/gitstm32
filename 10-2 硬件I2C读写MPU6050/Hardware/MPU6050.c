#include "stm32f10x.h"                  // Device header
#include "MPU6050_Reg.h"
#define MPU6050_Address		0XD0

void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)//此函数用于等待与超时退出
{
	uint32_t TimeOut = 10000;
		while(I2C_CheckEvent(I2Cx,I2C_EVENT) != SUCCESS)//等待事件完成，似乎是等待把stm32配置为主机,可放卡死，超时可退出
	{
		TimeOut--;
		if(TimeOut == 0)
		{
			break;//此处还可做相应的错误处理操作
		}
	}
}
void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data)
{
//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_Address);//从机地址
//	MyI2C_ReceiveAck();//接收应答，此处可进行下一步判断。所谓应答，无论发送或接收，应答了（即1），便能继续，反之则停止
//	MyI2C_SendByte(RegAddress);
//	MyI2C_ReceiveAck();
//	MyI2C_SendByte(Data);
//	MyI2C_ReceiveAck();
//	MyI2C_Stop();
	I2C_GenerateSTART(I2C2,ENABLE);//生成起始条件
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);//等待事件完成，似乎是等待把stm32配置为主机

	I2C_Send7bitAddress(I2C2,MPU6050_Address,I2C_Direction_Transmitter);//发送从机地址（无需加应答函数，因此函数本身自带）
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);//等待发送模式已选择
	I2C_SendData(I2C2,RegAddress);//无需等待EV8_1事件，直接发送寄存器地址
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING);//等待字节正在发送事件完成
	I2C_SendData(I2C2,Data);//发送数据
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);//等待字节发送完毕
	I2C_GenerateSTOP(I2C2,ENABLE);//终止时序
	
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_Address);//从机地址
//	MyI2C_ReceiveAck();//接收应答，此处可进行下一步判断
//	MyI2C_SendByte(RegAddress);
//	MyI2C_ReceiveAck();
//	
//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_Address | 0x01);// |后变为0XD1，为读
//	MyI2C_ReceiveAck();
//	Data = MyI2C_ReceiveByte();
//	MyI2C_SendAck(1);//不给从机应答，多个字节时，需给应答，从机便会继续发送数据
//	MyI2C_Stop();
	
	I2C_GenerateSTART(I2C2,ENABLE);//生成起始条件
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);//等待事件完成，似乎是等待把stm32配置为主机
	
	I2C_Send7bitAddress(I2C2,MPU6050_Address,I2C_Direction_Transmitter);//发送从机地址（无需加应答函数，因此函数本身自带）
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);//等待发送模式已选择
	
	I2C_SendData(I2C2,RegAddress);//无需等待EV8_1事件，直接发送寄存器地址
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);//ING和ED略有区别但结果一致，解释见10-5 38分
	
	I2C_GenerateSTART(I2C2,ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2,MPU6050_Address,I2C_Direction_Receiver);//读；此处无需|0x01，因函数会处理
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);//等待进入主机接收模式
	
	I2C_AcknowledgeConfig(I2C2,DISABLE);//设置ACK=0,不给应答（这两条代码“提前”放这还有说法，具体见10-5 41分讲解）
	I2C_GenerateSTOP(I2C2,ENABLE);//申请产生终止条件
	//若加个循环及if判断，可进行指定地址多个字节的读取
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);//等待EV7事件完成，此后一个字节的数据便在DR中了
	Data = I2C_ReceiveData(I2C2);//此函数返回值便是DR中的值
	
	I2C_AcknowledgeConfig(I2C2,ENABLE);//默认是给应答的（此处不同理解240914）
	return Data;
	
}
/**
* @brief 接触睡眠，选择陀螺仪时钟，6个轴均不待机，采样分频为10，滤波参数最大，陀螺仪和加速度计均最大量程
  * @param  无
  * @retval 无
  */
void MPU6050_Init(void)
{
//	MyI2C_Init();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);//开启I2C2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;//复用开漏模式；开漏是I2C协议的设计要求，复用是GPIO的控制权要交给硬件外设
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Ack=I2C_Ack_Enable;//给应答
	I2C_InitStructure.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;//响应7位地址
	I2C_InitStructure.I2C_ClockSpeed = 50000;//通信速度，此时为50KHZ，标准100KHZ,最大400KHZ
	I2C_InitStructure.I2C_DutyCycle= I2C_DutyCycle_2;//10-5 24分有解释
	I2C_InitStructure.I2C_Mode=I2C_Mode_I2C;//模式
	I2C_InitStructure.I2C_OwnAddress1= 0x00;//此项目STM32不做从机，故此地址无实际意义
	I2C_Init(I2C2,&I2C_InitStructure);
	
	I2C_Cmd(I2C2,ENABLE);
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);//此处配置要参考MPU6050寄存器手册
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);//此处表示10分频
	MPU6050_WriteReg(MPU6050_CONFIG,0x06);//此处涉及外部同步与低通滤波器
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);//此处涉及陀螺仪配置，现在配置（0x18）为满量程
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);//此处涉及加速度配置量程及高通滤波器，现在配置（0x18）为满量程
	
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
