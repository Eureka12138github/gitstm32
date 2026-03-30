#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "MyRTC.h"
#include "DHT11.h"
u8 temp=0,humi=0;
int main(void)
{

	OLED_Init();
	MyRTC_Init();
	//Timer_Init();
	Delay_s(1);
	while(DHT11_Init())	//检测DHT11是否存在
	{
		OLED_ShowString(0,0,"Error",OLED_8X16);		
	}
	OLED_Clear();
	OLED_ShowString(0,35,"Temp:   C",OLED_8X16);//现在就差°没显示了241026
	OLED_ShowString(0,50,"Humi:  %RH",OLED_8X16);
	OLED_ShowString(0,0,"Date:    -  -  ",OLED_8X16);
	OLED_ShowString(0,15,"Time:  :  :  ",OLED_8X16);
	OLED_Update();
	MyRTC_SetTime();
	while(1)
	{
		//up to date
		MyRTC_ReadTime();
//		//看到43分，现在日期获取失败！
//		/*
//			问题找到了，真是坑爹的，我把MyRTC_ReadTime及MyRTC_SetTime函数中的
//			time_date.tm_year 写成 time_date.tm_yday 了！！！
//			小小研究了一下，tm_yday似乎是离1月1号的天数，比如1月15号tm_yday应该是15？
//			验证了一下，是的。
//		*/
		OLED_ShowNum(40,0,MyRTC_Time[0],4,OLED_8X16);//显示年
		OLED_ShowNum(81,0,MyRTC_Time[1],2,OLED_8X16);//显示月
		OLED_ShowNum(106,0,MyRTC_Time[2],2,OLED_8X16);//显示日
		
		OLED_ShowNum(40,15,MyRTC_Time[3],2,OLED_8X16);//显示时
		OLED_ShowNum(65,15,MyRTC_Time[4],2,OLED_8X16);//显示分
		OLED_ShowNum(87,15,MyRTC_Time[5],2,OLED_8X16);//显示秒
//		OLED_ShowNum(32,35,MyRTC_Time[6],1,OLED_8X16);//显示星期
		
		DHT11_Read_Data(&temp,&humi);
		OLED_ShowNum(40,35,temp,2,OLED_8X16);//
		OLED_ShowNum(40,50,humi,2,OLED_8X16);
		OLED_Update();
//		Delay_ms(2000);
		//16分
	}
}
//void TIM2_IRQHandler(void)
//{
//    static uint16_t T0Count1;

//    // 检查是否有更新中断发生
//    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
//    {
//        // 对不同的计数器进行加1操作
//        T0Count1++;
//        // 清除更新中断的标志位
//        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//    }
//    
//    if (T0Count1 >= 1500)
//		
//	
//    {
//        T0Count1 = 0;
//        //DHT11_Read_Data(&temp,&humi);
//    }

//}

