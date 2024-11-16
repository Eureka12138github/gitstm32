#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "PWM.h"
int main(void)
{
	int_least8_t i;
	OLED_Init();	
	PWM_Init();
	while(1)
	{
		for(i=0;i<=100;i++)
		{
			
			PWM_SetCompare1(i);
			Delay_ms(10);
		}
		for(i=100;i>=0;i--)
		{
			if(i!=0)
			{
			PWM_SetCompare1(i);
			Delay_ms(10);
			}
			else 
			{
			PWM_SetCompare1(i);
			Delay_ms(20);
			}
		}
		//上面这样为何不行，渐暗可以，但是渐亮不行，而是马上变最亮
		//原因在于之i为uint8_t，即unsigned char，无符号数，i在第二个for循环里面无论怎么减都大于0，故第二个循环一直成立
//		for(i=0;i<=100;i++)
//		{
//			PWM_SetCompare1(i);
//			Delay_ms(10);
//		}
//		for(i=0;i<=100;i++)
//		{
//			PWM_SetCompare1(100-i);
//			Delay_ms(10);
//		}
		
	}
}
