#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"      // STM32标准库
#include "stddef.h"         // 标准定义

/*---------------- 硬件引脚配置（修改此处调整按键引脚） ----------------*/
#define Key1_PIN        GPIO_Pin_6     // 按键1 -> PA6
#define Key2_PIN        GPIO_Pin_7     // 按键2 -> PA7
#define Key3_PIN        GPIO_Pin_0     // 按键3 -> PB0
#define Key4_PIN        GPIO_Pin_1     // 按键4 -> PB1

/* 按键状态读取宏（0=按下，1=释放） */
#define Read_Key1_State()   GPIO_ReadInputDataBit(GPIOA, Key1_PIN)
#define Read_Key2_State()   GPIO_ReadInputDataBit(GPIOA, Key2_PIN)
#define Read_Key3_State()   GPIO_ReadInputDataBit(GPIOB, Key3_PIN)
#define Read_Key4_State()   GPIO_ReadInputDataBit(GPIOB, Key4_PIN)

/*---------------- 时间参数配置（基于20ms系统时钟） -------------------*/
#define DEBOUNCE_TICKS      1       // 消抖时间20ms 
#define LONG_PRESS_TICKS    25      // 长按判定500ms
#define DOUBLE_CLICK_TICKS  10      // 双击间隔200ms
#define TRIPLE_CLICK_TICKS  10      // 三击间隔200ms 

/* 渐进长按阶段配置 */
#define PHASE1_END_TICKS    100     // 阶段1结束2000ms
#define PHASE2_END_TICKS    200     // 阶段2结束4000ms  
#define INTERVAL_PHASE1     10      // 阶段1触发间隔200ms
#define INTERVAL_PHASE2     5       // 阶段2触发间隔100ms
#define INTERVAL_PHASE3     1       // 阶段3触发间隔20ms

#define MAX_KEYS_NUM       4       // 最大按键数量


/*--------------------- 状态与事件类型定义 --------------------------*/
typedef enum {
    KEY_STATE_IDLE,          // 空闲状态
    KEY_STATE_DEBOUNCE,      // 消抖检测
    KEY_STATE_PRESSED,       // 持续按压
    KEY_STATE_WAIT_DOUBLE,   // 等待双击
    KEY_STATE_WAIT_TRIPLE    // 等待三击
} KeyState;

typedef enum {
    KEY_EVENT_NONE,          // 无事件
    KEY_EVENT_CLICK,         // 单击
    KEY_EVENT_DOUBLE_CLICK,  // 双击
    KEY_EVENT_TRIPLE_CLICK,  // 三击
    KEY_EVENT_LONG_PRESS     // 长按
} KeyEventType;

/* 按键状态机结构体（每个按键独立维护） */
typedef struct {
    KeyState current_state;   // 当前状态
    uint16_t press_counter;   // 按压计时（单位：20ms）
    uint16_t release_counter; // 释放计时（单位：20ms）
    KeyEventType event;       // 触发事件类型
    uint8_t multi_click_step; // 连击计数（0-2对应单击/双击/三击） 
} KeyStateMachine;

/*---------------------- 函数声明 --------------------------------*/
void Key_Init(void);    // 初始化按键GPIO
void Key_Scan(void);    // 按键状态扫描（需20ms周期调用）
void Key_GetEvent(KeyEventType* Key_Event, uint8_t Event_Num); // 获取按键事件

#endif 
