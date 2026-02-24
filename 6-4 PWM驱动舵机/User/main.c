/**
 ******************************************************************************
 * @file    main.c
 * @author  
 * @version V1.0
 * @date    2024年
 * @brief   舵机角度控制
 ******************************************************************************
 * @details 
 * 功能说明：
 * 本程序通过按键控制舵机在0°-180°范围内转动
 * - 按键1：角度递增30°
 * - 角度范围：-5°(显示为0°) 到 180°
 * - 超过180°自动回到起始位置
 * 
 * 硬件连接：
 * - 舵机连接到TIM2_CH2(PA1)引脚
 * - 按键(PB13 - KEY1 -> GND)用于角度控制
 * - OLED显示当前角度值
 * 
 * 技术要点：
 * - 采用角度偏移补偿硬件误差
 * - 实现循环角度控制
 * - 模块化设计便于扩展
 ******************************************************************************
 */

#include "stm32f10x.h"                 // STM32F10x系列设备头文件
#include "Delay.h"                     // 延时函数库
#include "Key.h"                       // 按键驱动库
#include "OLED.h"                      // OLED显示驱动库
#include "Servo.h"                     // 舵机控制库

/* 私有宏定义 */
#define ANGLE_STEP          30         // 角度递增步长
#define ANGLE_OFFSET        5          // 角度显示偏移量
#define MIN_DISPLAY_ANGLE   0          // 最小显示角度
#define MAX_DISPLAY_ANGLE   180        // 最大显示角度
#define MIN_ACTUAL_ANGLE    -5         // 最小实际角度
#define MAX_ACTUAL_ANGLE    180        // 最大实际角度

/* 私有函数声明 */
static void System_Init(void);
static void Display_Angle(int16_t actual_angle);
static int16_t Handle_Angle_Control(int16_t current_angle, uint8_t key_press);

/**
 * @brief 主函数
 * @details 程序入口点，实现舵机角度控制主循环
 */
int main(void)
{
    uint8_t key_value;
    int16_t servo_angle = MIN_ACTUAL_ANGLE;  // 初始化舵机角度
    
    // 系统初始化
    System_Init();
    
    // 显示初始角度
    Display_Angle(servo_angle);
    Servo_SetAngle(servo_angle);
    
    // 主控制循环
    while(1)
    {
        key_value = Key_GetNum();  // 获取按键值
        
        if(key_value == 1)  // 按键1被按下
        {
            servo_angle = Handle_Angle_Control(servo_angle, key_value);
            Display_Angle(servo_angle);
            Servo_SetAngle(servo_angle);
        }
    }
}

/**
 * @brief 系统初始化函数
 * @details 初始化所有外设模块
 */
static void System_Init(void)
{
    Key_Init();     // 初始化按键
    Servo_Init();   // 初始化舵机
    OLED_Init();    // 初始化OLED显示屏
    OLED_ShowString(1, 1, "Angle:");  // 显示角度标签
}

/**
 * @brief 显示角度值
 * @param actual_angle 实际角度值
 * @details 将实际角度转换为显示角度并更新OLED显示
 */
static void Display_Angle(int16_t actual_angle)
{
    uint16_t display_angle = actual_angle + ANGLE_OFFSET;  // 转换为显示角度
    OLED_ShowNum(1, 7, display_angle, 3);  // 在OLED上显示角度
}

/**
 * @brief 角度控制处理函数
 * @param current_angle 当前角度值
 * @param key_press 按键值
 * @return 处理后的角度值
 * @details 实现角度递增和循环控制逻辑
 */
static int16_t Handle_Angle_Control(int16_t current_angle, uint8_t key_press)
{
    int16_t new_angle = current_angle;
    
    // 特殊处理：从起始位置开始时使用较小步长
    if(current_angle == MIN_ACTUAL_ANGLE)
    {
        new_angle += (ANGLE_STEP - ANGLE_OFFSET);  // 25°步长
    }
    else
    {
        new_angle += ANGLE_STEP;  // 30°步长
    }
    
    // 角度越界处理：超过最大值回到起始位置
    if(new_angle > MAX_ACTUAL_ANGLE)
    {
        new_angle = MIN_ACTUAL_ANGLE;
    }
    
    return new_angle;
}
/*
 * 硬件补偿说明：
 * 
 * 舵机硬件存在机械误差：
 * - 实际0°位置对应程序中的-5°
 * - 显示时通过OFFSET补偿，显示为0°
 * - 这种设计保持了用户界面的直观性
 * 
 * 角度对应关系：
 * 程序角度 | 显示角度 | 实际舵机角度
 *   -5°    |   0°    |   硬件0°位置
 *   25°    |   30°   |   硬件30°位置
 *   85°    |   90°   |   硬件90°位置
 *   175°   |   180°  |   硬件180°位置
 * 
 * 设计优势：
 * 1. 用户看到的是标准0-180°范围
 * 2. 程序内部处理硬件偏移
 * 3. 便于后期校准和维护
 */
