/**
 ******************************************************************************
 * @file    main.c
 * @author  
 * @version V1.0
 * @date    2024年
 * @brief   编码器测速系统（定时器中断方式）
 ******************************************************************************
 * @details 
 * 功能概述：
 * 本程序使用TIM3编码器接口模式测量旋转编码器转速，
 * 通过TIM2定时器中断定期读取编码器计数值，
 * 并在OLED上显示当前转速（带方向信息）。
 * 
 * 硬件连接：
 * - PA6、PA7：编码器A、B相信号输入
 * - TIM3：编码器接口模式
 * - TIM2：定时中断用于周期性读取
 * - OLED：显示转速信息
 * 
 * 技术特点：
 * - 硬件编码器接口：自动处理相位检测和方向判断
 * - 定时器中断读取：避免主循环阻塞，提高实时性
 * - 带符号显示：正数表示正转，负数表示反转
 ******************************************************************************
 */

#include "stm32f10x.h"                 // STM32F10x系列设备头文件
#include "Delay.h"                     // 延时函数库
#include "LED.h"                       // LED驱动库
#include "OLED.h"                      // OLED显示驱动库
#include "Timer.h"                     // 定时器配置库
#include "Encoder.h"                   // 编码器测速库

/* 全局变量 */
int16_t Speed;  // 存储编码器计数值（带符号，正负表示方向）

/**
 * @brief 主函数
 * @details 程序入口点
 */
int main(void)
{
    // 系统初始化
    OLED_Init();        // 初始化OLED显示屏
    Encoder_Init();     // 初始化编码器接口（TIM3）
    Timer_Init();       // 初始化定时器（TIM2，1秒周期中断）
    
    // 显示界面初始化
    OLED_ShowString(1, 1, "CNT:");  // 显示计数标签
    
    // 主循环 - 实时刷新显示
    while(1)
    {
        OLED_ShowSignedNum(1, 7, Speed, 5);  // 显示带符号的计数值
        // 注意：无需Delay，因为读取由中断完成，主循环非阻塞
    }
}

/**
 * @brief TIM2定时器更新中断服务函数
 * @details 每1秒执行一次，读取编码器计数值
 * @note 中断服务函数必须放在main.c中，不能放在头文件里
 */
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        // 读取编码器计数值并清零
        Speed = Encoder_Get();  // 获取当前计数值（已清零）
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // 清除中断标志
    }
}
