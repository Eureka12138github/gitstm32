#include "stm32f10x.h"                 // Device header
#include "bsp_delay.h"
#include "LED.h"
#include "OLED.h"
#include "CountSensor.h"
/**
 * @note此对射式红外传感器无遮挡时应输出低电平，遮挡时输出高电平
 *  	现在外部中断配置为下降沿除非，理想情况下（无抖动）应在移除挡板时 CountSensor_Count++；
 * @brief 接线：
 *			   PA2 -> D0（触发时D0输出高电平，闲时状态低电平）
 *		  预期现象：撤去遮挡时，Count显示数值+1，OLED第二行辅助测试数字快速递增表示此时PIN口电平为高电平
 *  
*/
int main(void)
{
	Delay_Init();
	OLED_Init();
	CountSensor_Init();
	OLED_ShowString(1,1,"Count:");
	u32 num = 0;
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)) {
			OLED_ShowNum(2,1,num++,8);
		}
		OLED_ShowNum(1,7,CountSensor_Get(),5);
		
	}
}
