/**
 ******************************************************************************
 * @file    main.c
 * @author  
 * @version V1.0
 * @date    2024年
 * @brief   PWM驱动LED呼吸灯效果演示程序
 ******************************************************************************
 * @details 
 * 功能说明：
 * 本程序通过PWM控制LED实现呼吸灯效果，亮度从0%逐渐增加到100%，
 * 然后从100%逐渐降低到0%，循环往复。
 * 
 * 硬件连接：
 * - LED连接到TIM2_CH1(PA0)引脚
 * - 通过PWM占空比控制LED亮度
 * 
 * 技术要点：
 * - 使用有符号整型变量避免循环条件错误
 * - 实现平滑的亮度渐变效果
 * - 采用模块化设计便于扩展
 ******************************************************************************
 */

#include "stm32f10x.h"                 // STM32F10x系列设备头文件
#include "Delay.h"                     // 延时函数库
#include "LED.h"                       // LED驱动库
#include "OLED.h"                      // OLED显示驱动库
#include "PWM.h"                       // PWM配置库

/* 私有宏定义 */
#define BRIGHTNESS_STEP     1          // 亮度调节步长
#define INCREASE_DELAY_MS   50         // 亮度增加延时(ms)
#define DECREASE_DELAY_MS   10         // 亮度减少延时(ms)
#define MIN_BRIGHTNESS      0          // 最小亮度(0%)
#define MAX_BRIGHTNESS      100        // 最大亮度(100%)

/* 私有函数声明 */
static void System_Init(void);
static void Breathing_Light_Effect(void);

/**
 * @brief 主函数
 * @details 程序入口点，负责系统初始化和主循环处理
 */
int main(void)
{
    // 系统初始化
    System_Init();
    
    // 主循环 - 实现呼吸灯效果
    while(1)
    {
        Breathing_Light_Effect();
    }
}

/**
 * @brief 系统初始化函数
 * @details 初始化所有外设模块
 */
static void System_Init(void)
{
    OLED_Init();    // 初始化OLED显示屏
    PWM_Init();     // 初始化PWM模块
}

/**
 * @brief 呼吸灯效果实现函数
 * @details 控制LED从暗到亮再到暗的循环变化
 * @note 使用有符号整型确保循环条件正确
 */
static void Breathing_Light_Effect(void)
{
    int8_t brightness;  // 使用有符号整型避免循环错误
    
    // 亮度逐渐增加：0% → 100%
    for(brightness = MIN_BRIGHTNESS; brightness <= MAX_BRIGHTNESS; brightness += BRIGHTNESS_STEP)
    {
        PWM_SetCompare1(brightness);           // 设置PWM占空比
        Delay_ms(INCREASE_DELAY_MS);           // 延时产生渐变效果
    }
    
    // 亮度逐渐减少：100% → 0%
    for(brightness = MAX_BRIGHTNESS; brightness >= MIN_BRIGHTNESS; brightness -= BRIGHTNESS_STEP)
    {
        PWM_SetCompare1(brightness);           // 设置PWM占空比
        
        // 在最低亮度时稍作停顿增强视觉效果
        if(brightness == MIN_BRIGHTNESS)
        {
            Delay_ms(DECREASE_DELAY_MS * 2);   // 延长最低亮度停留时间
        }
        else
        {
            Delay_ms(DECREASE_DELAY_MS);       // 正常延时
        }
    }
}
