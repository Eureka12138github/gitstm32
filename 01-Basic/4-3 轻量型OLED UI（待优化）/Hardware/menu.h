/**
 * @file menu.h
 * @brief 轻量级 OLED 菜单 UI 框架接口头文件
 *
 * 本文件定义了 OLED 菜单系统的核心对外 API，提供基础的用户界面交互能力。
 * 基于 menu_data.h 中定义的菜单数据结构，实现导航、滚动、显示与按键响应。
 *
 * 主要特性：
 * - 支持多级菜单页面跳转
 * - 自动垂直滚动（当菜单项超出屏幕时）
 * - 长文本水平滚动动画
 * - 可编辑数值项支持（增/减）
 *
 * @author Eureka & Lingma
 * @date 2026-02-22
 */

#ifndef MENU_H
#define MENU_H

#include "menu_data.h"  // 引入菜单数据结构定义

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 *                          核心接口函数声明
 *============================================================================*/

/**
 * @brief 初始化 OLED 菜单系统
 */
void MyOLED_UI_Init(MyMenuPage* page);

/**
 * @brief 菜单系统主循环处理函数
 */
void MyOLED_UI_MainLoop(void);

/**
 * @brief 按键扫描与事件分发函数
 */
void Key_Scan(void);

#ifdef __cplusplus
}
#endif

#endif /*MENU_H */


/*============================================================================
 *                          使用示例
 *============================================================================*/
/*
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
*/
