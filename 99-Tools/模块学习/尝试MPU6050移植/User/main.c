#include "led.h"
#include "delay.h"
#include "sys.h"
#include "mpu6050.h"  
#include "inv_mpu.h"
#include "usart.h"
#include "OLED.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "Timer.h" 
float pitch,roll,yaw; 
int main(void)
{
			//欧拉角
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	    //串口初始化 波特率为115200
	delay_init();	            //延时函数初始化 
	LED_Init();		  			    //初始化与LED连接的硬件接口
	MPU_Init();	
	OLED_Init();
//	Timer_Init();
	while(mpu_dmp_init())
	{
	delay_ms(20);
	}
	while(1)
	{
		delay_ms(2);
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)//这里就是角度的获取
		{
			LED=~LED;
		}
		//加入显示函数后串口发送的数据有所滞后，现在的想法是定时器中断中获取角度，然后通过串口中断发送数据，
		//主函数的while循环只调用显示函数。
//		OLED_ShowFloatNum(0,0,pitch,3,2,OLED_8X16);
//		OLED_ShowFloatNum(0,20,roll,3,2,OLED_8X16);
//		OLED_ShowFloatNum(0,40,yaw,3,2,OLED_8X16);
//		OLED_Update();
		printf("samples:%f, %f, %f\n\r", pitch,roll,yaw);
	
	}
}

//void TIM2_IRQHandler(void)
//{
//    // 静态变量T0Count1和T0Count3用于计数中断发生的次数
//    static uint8_t T0Count1;

//    // 检查是否有更新中断发生
//    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
//    {
//        // 对不同的计数器进行加1操作
//        T0Count1++;
//        // 清除更新中断的标志位
//        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//    }
//    
//    // 当T0Count1达到20时，将其重置，并调用Key_Loop函数处理按键循环
//    if (T0Count1 >= 2)
//    {
//		OLED_ShowFloatNum(0,0,pitch,3,2,OLED_8X16);
//		OLED_ShowFloatNum(0,20,roll,3,2,OLED_8X16);
//		OLED_ShowFloatNum(0,40,yaw,3,2,OLED_8X16);
//		OLED_Update();
//        T0Count1 = 0;
//    }


//}



