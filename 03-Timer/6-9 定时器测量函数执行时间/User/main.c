#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Micro_Timer.h"
uint16_t Num;
int main(void)
{
	OLED_Init();
	MicroTimer_Init();
	OLED_ShowString(1,1,"Time:");
	uint32_t last_time;
	uint32_t current_time;
	while(1)
	{
		last_time = Get_Micros();
		Delay_us(2026);
		current_time = Get_Micros();
		OLED_ShowNum(2,1,Calculate_Elapsed_Time(last_time,current_time),6);
		

	}
}


