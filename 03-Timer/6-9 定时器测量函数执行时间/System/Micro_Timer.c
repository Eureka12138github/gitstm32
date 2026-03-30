#include "Micro_Timer.h"
#include "stm32f10x.h"                  // Device header

// 全局变量，用于存储最后一次测量的耗时 (单位：us)
volatile uint32_t g_last_exec_time_us = 0;
// 报警标志
volatile uint8_t g_timer_overflow_flag = 0;

void MicroTimer_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 72MHz / 72 = 1MHz (1us per tick)
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;      
    // 0xFFFF = 65535 us (~65ms)
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;         
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    // 【重要】先清零计数器，再开启，确保从 0 开始
    TIM_SetCounter(TIM3, 0); 
    TIM_Cmd(TIM3, ENABLE);
}


uint32_t Get_Micros(void) {
    return TIM3->CNT;
}

/**
 * @brief 安全计算耗时
 * @param start 开始时间
 * @param end 结束时间
 * @return 耗时微秒数。如果检测到异常超长(>60ms)，返回 0xFFFF 并置位标志
 */
uint32_t Calculate_Elapsed_Time(uint32_t start, uint32_t end) {
    // 技巧：在无符号整数运算中，(end - start) 即使发生溢出(跨零点)，
    // 只要实际差值 < 65536，结果依然是正确的！
    // 例如：start=65530, end=10. 
    // (uint16_t)(10 - 65530) = (uint16_t)(-65520) = 10 (正确!)
    // 但因为我们用的是 uint32_t 接收 TIM3->CNT (高16位是0)，需要强制转换一下确保逻辑
    
    uint32_t delta = (uint16_t)(end - start); 
    
    // 安全检查：如果耗时超过 60ms (60000us)，认为异常或溢出多次
    if (delta > 60000) {
        g_timer_overflow_flag = 1; // 标记异常
        return 65535; // 返回最大值表示溢出
    }
    
    g_timer_overflow_flag = 0;
    return delta;
}

