#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "Key.h"
#include "Buzzer.h"
#include "InfraredSensor.h"
uint8_t KeyNum,a;
int main(void)
{	Infrared_Init();
	Key_Init();
	Buzzer_Init();
	while(1)
	{
	a=Infrared_Get();
	KeyNum=Key_GetNum();
	if(a==0)
	{
		Buzzer_OFF();
	}
	else
	{
		Buzzer_ON();
	}
	
//	if(KeyNum==2)
//	{
//			Buzzer_Turn();
//	}		
	}
}
