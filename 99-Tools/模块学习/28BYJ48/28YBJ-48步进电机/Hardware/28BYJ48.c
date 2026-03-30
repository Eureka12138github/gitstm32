#include "stm32f10x.h"                  // Device header
#define PORT GPIOB
#define INA GPIO_Pin_12
#define INB GPIO_Pin_13
#define INC GPIO_Pin_14
#define IND GPIO_Pin_15
//若需更换引脚，只需修改宏定义，与开启时钟便可。（如果也是用GPIOB的话，时钟开启那里就不用改了。）
void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//外设时钟开启还是需要单独修改！！！
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=INA|INB|INC|IND;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(PORT,&GPIO_InitStructure);
	GPIO_ResetBits(PORT,INA|INB|INC|IND);
}
/*******************************************************************************
* 函 数 名       : step_motor_28BYJ48_send_pulse_8
* 函数功能		 : 输出一个数据给ULN2003从而实现向步进电机发送一个脉冲,工作方式为四项八拍
* 输    入       : step：指定步进序号，可选值0~7
				   dir：方向选择,1：顺时针,0：逆时针
* 输    出    	 : 无
*******************************************************************************/
void step_motor_28BYJ48_send_pulse_8(uint8_t step,uint8_t dir)
{
	/**
	void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);**/
	uint8_t temp=step;
	
	if(dir==0)	//如果为逆时针旋转
		temp=7-step;//调换节拍信号
	switch(temp)//8个节拍控制：A->AB->B->BC->C->CD->D->DA
	{
		case 0: GPIO_SetBits(PORT,INA);GPIO_ResetBits(PORT,INB|INC|IND);break;//A
		case 1: GPIO_SetBits(PORT,INA|INB);GPIO_ResetBits(PORT,INC|IND);break;//AB
		case 2: GPIO_ResetBits(PORT,INA|INC|IND);GPIO_SetBits(PORT,INB);break;//B
		case 3: GPIO_ResetBits(PORT,INA|IND);GPIO_SetBits(PORT,INB|INC);break;//BC
		case 4: GPIO_ResetBits(PORT,INA|INB|IND);GPIO_SetBits(PORT,INC);break;//C
		case 5: GPIO_ResetBits(PORT,INA|INB);GPIO_SetBits(PORT,INC|IND);break;//CD
		case 6: GPIO_ResetBits(PORT,INA|INB|INC);GPIO_SetBits(PORT,IND);break;//D
		case 7: GPIO_SetBits(PORT,INA|IND);GPIO_ResetBits(PORT,INB|INC);break;//DA
		default: GPIO_ResetBits(PORT,INA|INB|INC|IND);break;//停止相序	
	}
}

/*******************************************************************************
* 函 数 名       : step_motor_28BYJ48_send_pulse_d4
* 函数功能		 : 输出一个数据给ULN2003从而实现向步进电机发送一个脉冲,工作方式为双四拍
* 输    入       : step：指定步进序号，可选值0~3
				   dir：方向选择,1：顺时针,0：逆时针
* 输    出    	 : 无
*******************************************************************************/
void step_motor_28BYJ48_send_pulse_d4(uint8_t step,uint8_t dir)
{
	u8 temp=step;
	
	if(dir==0)	//如果为逆时针旋转
		temp=3-step;//调换节拍信号
	switch(temp)//4个节拍控制：AB->BC->CD->DA
	{
		case 0: GPIO_SetBits(PORT,INA|INB);GPIO_ResetBits(PORT,INC|IND);break;//AB
		case 1: GPIO_ResetBits(PORT,INA|IND);GPIO_SetBits(PORT,INB|INC);break;//BC
		case 2: GPIO_ResetBits(PORT,INA|INB);GPIO_SetBits(PORT,INC|IND);break;//CD
		case 3: GPIO_SetBits(PORT,INA|IND);GPIO_ResetBits(PORT,INB|INC);break;//DA
		default: GPIO_ResetBits(PORT,INA|INB|INC|IND);break;//停止相序
	}			
}

/*******************************************************************************
* 函 数 名       : step_motor_28BYJ48_send_pulse_4
* 函数功能		 : 输出一个数据给ULN2003从而实现向步进电机发送一个脉冲,工作方式为单四拍
* 输    入       : step：指定步进序号，可选值0~3
				   dir：方向选择,1：顺时针,0：逆时针
* 输    出    	 : 无
*******************************************************************************/
void step_motor_28BYJ48_send_pulse_4(uint8_t step,uint8_t dir)
{
	u8 temp=step;
	
	if(dir==0)	//如果为逆时针旋转
		temp=3-step;//调换节拍信号
	switch(temp)//4个节拍控制：A->B->C->D
	{
		case 0: GPIO_SetBits(PORT,INA);GPIO_ResetBits(PORT,INB|INC|IND);break;//A
		case 1: GPIO_ResetBits(PORT,INA|INC|IND);GPIO_SetBits(PORT,INB);break;//B
		case 2: GPIO_ResetBits(PORT,INA|INB|IND);GPIO_SetBits(PORT,INC);break;//C
		case 3: GPIO_ResetBits(PORT,INA|INB|INC);GPIO_SetBits(PORT,IND);break;//D
		default: GPIO_ResetBits(PORT,INA|INB|INC|IND);break;//停止相序	
	}			
}


