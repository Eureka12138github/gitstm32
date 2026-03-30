#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
u16 ArrayWrite[] = {0X1234,0X5678};
u16 ArrayRead[2];
u8 KeyNum;
int main(void)
{
	OLED_Init();
	Key_Init();
	Timer_Init();
	OLED_ShowString(0,0,"W:",OLED_8X16);
	OLED_ShowString(0,20,"R:",OLED_8X16);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//使能PWR和BKP时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);
	PWR_BackupAccessCmd(ENABLE);//使能BKP对RTC的访问
//	BKP_WriteBackupRegister(BKP_DR1,0x1235);//写入备份寄存器，中容量只有BKP_DR1~BKP_DR10,不能超了
//	OLED_ShowHexNum(0,0,BKP_ReadBackupRegister(BKP_DR1),4,OLED_8X16);
//	OLED_Update();
	
	while(1)
	{
		KeyNum = Key();//获取按键值
		
		if(KeyNum == 1)
		{
			ArrayWrite[0]++;//将数组自增1
			ArrayWrite[1]++;
			BKP_WriteBackupRegister(BKP_DR1,ArrayWrite[0]);//将数据写入BKP
			BKP_WriteBackupRegister(BKP_DR2,ArrayWrite[1]);
			OLED_ShowHexNum(20,0,ArrayWrite[0],4,OLED_8X16);//显示自增后的数组
			OLED_ShowHexNum(60,0,ArrayWrite[1],4,OLED_8X16);
			OLED_Update();
		}
		ArrayRead[0] = BKP_ReadBackupRegister(BKP_DR1);//读出BKP中的数据
		ArrayRead[1] = BKP_ReadBackupRegister(BKP_DR2);
		OLED_ShowHexNum(20,20,ArrayRead[0],4,OLED_8X16);//显示读出的数据
		OLED_ShowHexNum(60,20,ArrayRead[1],4,OLED_8X16);
		OLED_Update();
	
	}
}
	void TIM2_IRQHandler(void)
{
	static uint8_t T0Count1;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		T0Count1++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//这句还搞不太懂，只有一个事件时应该可以直接清零把？
	}
	
	if(T0Count1>=20)
	{
		T0Count1=0;
		Key_Loop();//这个函数不能出现Delay等耗时语句，否则定时器会出错
	}
}



