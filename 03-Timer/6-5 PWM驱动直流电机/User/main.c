/**
 ******************************************************************************
 * @file    main.c
 * @author  
 * @version V1.0
 * @date    2024年
 * @brief   PWM驱动直流电机控制系统
 ******************************************************************************
 * @details 
 * 功能说明：
 * 本程序通过按键控制直流电机的转速和方向
 * - 按键1：速度递增20单位
 * - 速度范围：-100(反转) 到 +100(正转)
 * - 超过上限时自动切换到反向最大速度
 * 
 * 硬件连接：
 * - 电机驱动电路连接到PA4(正转)、PA5(反转)、PWM引脚
 * - 按键用于速度控制
 * - OLED显示当前速度值
 * 
 * 技术要点：
 * - 采用循环速度控制策略
 * - 实现正反转无缝切换
 * - PWM频率可调以减少电机噪音
 ******************************************************************************
 */

#include "stm32f10x.h"                 // STM32F10x系列设备头文件
#include "Delay.h"                     // 延时函数库
#include "LED.h"                       // LED驱动库
#include "Key.h"                       // 按键驱动库
#include "OLED.h"                      // OLED显示驱动库
#include "Motor.h"                     // 电机控制库

/* 私有宏定义 */
#define SPEED_STEP          20         // 速度递增步长
#define MAX_SPEED           100        // 最大速度值
#define MIN_SPEED           -100       // 最小速度值

/* 私有函数声明 */
static void System_Init(void);
static int8_t Handle_Speed_Control(int8_t current_speed);
static void Display_Speed(int8_t speed);

/**
 * @brief 主函数
 * @details 程序入口点，实现电机速度控制主循环
 */
int main(void)
{
    uint8_t key_value;
    int8_t motor_speed = 0;  // 初始化电机速度为0
    
    // 系统初始化
    System_Init();
    
    // 主控制循环
    while(1)
    {
        key_value = Key_GetNum();  // 获取按键值
        
        if(key_value == 1)  // 按键1被按下
        {
            motor_speed = Handle_Speed_Control(motor_speed);
            Motor_SetSpeed(motor_speed);
        }
        
        Display_Speed(motor_speed);  // 更新OLED显示
    }
}

/**
 * @brief 系统初始化函数
 * @details 初始化所有外设模块
 */
static void System_Init(void)
{
    OLED_Init();    // 初始化OLED显示屏
    Motor_Init();   // 初始化电机驱动
    Key_Init();     // 初始化按键
}

/**
 * @brief 速度控制处理函数
 * @param current_speed 当前速度值
 * @return 处理后的速度值
 * @details 实现速度递增和循环控制逻辑
 */
static int8_t Handle_Speed_Control(int8_t current_speed)
{
    int8_t new_speed = current_speed + SPEED_STEP;
    
    // 速度越界处理：超过最大值时切换到反向最大速度
    if(new_speed > MAX_SPEED)
    {
        new_speed = MIN_SPEED;
    }
    
    return new_speed;
}

/**
 * @brief 显示速度值
 * @param speed 要显示的速度值
 * @details 在OLED屏幕上显示当前电机速度
 */
static void Display_Speed(int8_t speed)
{
    OLED_ShowString(1, 1, "Speed:");      // 显示速度标签
    OLED_ShowSignedNum(1, 7, speed, 3);   // 显示速度数值（带符号）
}

/*
 * 电机控制说明：
 * 
 * 速度控制逻辑：
 * 0 → 20 → 40 → 60 → 80 → 100 → -100 → -80 → -60 → -40 → -20 → 0
 * 
 * PWM频率优化：
 * - 默认频率可能产生可闻噪音
 * - 可通过修改PWM配置中的PSC值来调整频率
 * - 建议设置为20kHz以上以消除蜂鸣声
 * - 调整方法：增大PSC值可降低频率，减小PSC值可提高频率
 * 
 * 使用示例：
 * Motor_SetSpeed(50);   // 正转50%速度
 * Motor_SetSpeed(-30);  // 反转30%速度
 * Motor_SetSpeed(0);    // 停止电机
 * 
 */
