#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "W25Q64.h"
#define RenShu   20
uint8_t KeyNum = 0,min = 0,sec = 0,minisec = 0,runflag = 0;
uint8_t ArrayWrite[3] = {0};
uint8_t ArrayRead[3] = {0};
static uint32_t w = 0x000000;
static uint32_t r = 0x000000;
/**
	240929：该定时器的基本功能以基本完成，计次人数可从define处修改，
	按下按键1启动，按下按键2复位，每按下按键3计次一次，按下按键4，显示计次结果
	UI界面或许能再做得好看一点，但暂时不想做了。
**/
int main(void)
{
		// 定义无符号8位整数变量i和n，并初始化为0
		uint8_t i = 0, n = 0;
		
		// 初始化定时器
		Timer_Init();
		// 初始化按键
		Key_Init();
		// 初始化OLED显示
		OLED_Init();
		// 初始化LED
		LED_Init();
		// 初始化闪存芯片W25Q64
		W25164_Init();
		// 擦除闪存芯片W25Q64的指定扇区
		W25Q64_Sector_Erase(0X000000);
//		OLED_ShowString(0, 40, "TEST", OLED_8X16);

    // 主循环
    while (1)
    {
        // 读取按键值
        KeyNum = Key();
        // 如果按键1被按下
        if (KeyNum == 1)
        {
            // 切换runflag的值
            runflag = !runflag;
            // 切换LED1的状态
            LED1_Turn();
        }
        // 如果按键2被按下
        if (KeyNum == 2)
        {
            // 重置分钟、秒、毫秒计数
            min = 0;
            sec = 0;
            minisec = 0;
            // 擦除闪存的指定扇区
            W25Q64_Sector_Erase(0X000000);
            // 重置计数变量i和n
            i = 0;
            n = 0;
			w = 0;
			r = 0;
            // 清除OLED显示的内容
            OLED_ShowString(0, 20, "        ", OLED_8X16);
            OLED_ShowString(0, 50, "        ", OLED_8X16);
            OLED_ShowString(0, 40, "        ", OLED_8X16);
        }
        // 如果按键3被按下
        if (KeyNum == 3)
        {
            if (i < RenShu) 
			{ 
			ArrayWrite[0] = min;
            ArrayWrite[1] = sec;
            ArrayWrite[2] = minisec;//这里数组元素多的话，可以用循环，但这里没必要
			W25Q64_PageProgram(w, ArrayWrite, 3);
			w+=3;
			i++;
			}
        }

        // 如果按键4被按下
        if (KeyNum == 4)
        {
            // 如果n大于等于10，重置n
            if (n >= i) { n = 0; r = 0x000000;}
            // 根据n的值，选择不同的地址从闪存读取时间数据
			W25Q64_ReadData(r, ArrayRead, 3);
			r+=3;//本打算封装一下，但这里地址具体加多少要取决于ArrayRead数组中元素的个数，
            // n自增
            n++;
            // 如果n小于等于10，显示排名和对应的时间
            if (n <= RenShu)
            {
                OLED_ShowString(0, 20, "NO.", OLED_8X16);
                OLED_ShowNum(20, 20, n, 2, OLED_8X16);
            }
            OLED_ShowString(0, 50, "        ", OLED_8X16);
            OLED_ShowNum(0, 40, ArrayRead[0], 2, OLED_8X16);
			OLED_ShowString(15, 40, ":", OLED_8X16);
            OLED_ShowNum(22, 40, ArrayRead[1], 2, OLED_8X16);
			OLED_ShowString(37, 40, ":", OLED_8X16);
            OLED_ShowNum(44, 40, ArrayRead[2], 2, OLED_8X16);
            OLED_Update();
        }
        // 实时显示时间
			OLED_ShowNum(0, 0, min, 2, OLED_8X16);
			OLED_ShowString(15, 0, ":", OLED_8X16);
			OLED_ShowNum(22, 0, sec, 2, OLED_8X16);
			OLED_ShowString(37, 0, ":", OLED_8X16);
			OLED_ShowNum(44, 0, minisec, 2, OLED_8X16);
        
			// 显示计数次数
			OLED_ShowString(65, 0, "cnt:", OLED_8X16);
			OLED_ShowNum(100, 0, i, 2, OLED_8X16);
			OLED_Update();
    
    }
}

/**
 * @brief 本函数用于模拟秒表的秒数增加的逻辑
 * 
 * 该函数在运行标志为真时，按照模拟秒表计数的逻辑递增时间单位。
 * 时间单位包括秒（sec）、分钟（min），并使用一个额外的变量（minisec）
 * 来模拟秒内的更小单位。当minisec达到100时，秒数增加，以此类推。
 * 
 * 注意：此函数假定runflag、minisec、sec和min是全局变量或以某种方式在函数外部定义。
 */
void sec_loop(void)
{
    // 仅当runflag为真时执行以下代码，模拟秒表运行状态
    if(runflag)
    {
        // 增加minisec，模拟秒内的更小时间单位的增加
        minisec++;
        // 当minisec达到100，重置minisec并增加秒数
        if(minisec>=100)
        {
            minisec=0;
            // 秒数增加
            sec++;
            // 当秒数达到60，重置秒数并增加分钟数
            if(sec>=60)
            {
                sec=0;
                // 分钟数增加
                min++;
                // 当分钟数达到60，重置分钟数，准备下一个小时的计数
                if(min>=60)
                {
                    min=0;
                }
            }
        }
    }
}

/**
 * TIM2_IRQHandler函数是TIM2定时器的中断服务程序。
 * 它处理TIM2定时器产生的中断，并根据中断的不同来源执行相应的操作。
 * 其中包括处理定时器更新中断以及根据不同的计数条件调用相应的函数。
 */
void TIM2_IRQHandler(void)
{
    // 静态变量T0Count1和T0Count3用于计数中断发生的次数
    static uint8_t T0Count1, T0Count3;

    // 检查是否有更新中断发生
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        // 对不同的计数器进行加1操作
        T0Count1++;
        T0Count3++;

        // 清除更新中断的标志位
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
    
    // 当T0Count1达到20时，将其重置，并调用Key_Loop函数处理按键循环
    if (T0Count1 >= 20)
    {
        T0Count1 = 0;
        Key_Loop();
    }

    // 当T0Count3达到10时，将其重置，并调用sec_loop函数处理秒循环
    if (T0Count3 >= 10)
    {
        T0Count3 = 0;
        sec_loop();
    }
}

