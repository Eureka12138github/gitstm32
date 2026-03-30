#include "stm32f10x.h"
#include "menu.h"

int main(void)
{

    // 1. 启动菜单系统
    MyOLED_UI_Init(&MainPage);  // MainPage 定义于 menu_data.c

    // 2. 主循环
    while (1) {
        MyOLED_UI_MainLoop();   // 必须持续调用
    }
}

// 示例：定时器中断中调用按键扫描（每 20ms）
void TIM2_IRQHandler(void)
{
    static uint8_t tick = 0;
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        if (++tick >= 20) {  // 假设定时器 1ms 中断
            tick = 0;
            Key_Scan();      // 触发按键处理
        }
    }
}
