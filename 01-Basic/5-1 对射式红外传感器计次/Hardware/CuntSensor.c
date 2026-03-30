/**
 * @file CountSensor.c
 * @brief 脉冲计数传感器驱动（基于 EXTI 中断 + 软件消抖）
 * 
 * 本模块通过外部中断（EXTI）检测 GPIO 引脚上的边沿信号，
 * 实现对旋转编码器、霍尔传感器、光电开关等脉冲信号的计数。
 * 
 * 使用说明：
 * 1. 在 CountSensor.h 中配置引脚、端口、触发方式；
 * 2. 手动启用对应的 COUNT_SENSOR_IRQN 宏；
 * 3. 在本文件中实现对应名称的中断服务函数（如 EXTI2_IRQHandler）；
 * 4. 主循环中定期调用 CountSensor_Get() 获取计数值并完成消抖处理。
 * 
 * 注意：中断服务函数仅记录时间戳和置位标志，不进行延时或复杂操作，
 *       消抖逻辑在主循环中异步完成，确保系统实时性。
 */

#include "CountSensor.h"
#include "bsp_delay.h"  // 提供 SysTick_Get() 函数，返回毫秒级时间戳

/* ------------------------ 全局变量 ------------------------ */

/** 当前累计的有效脉冲计数值 */
volatile uint16_t CountSensor_Count = 0;

/** 上次中断发生的时间戳（单位：ms） */
volatile uint32_t g_LastIntTime = 0;

/** 中断待处理标志：1=有待处理事件，0=无事件 */
volatile uint8_t g_ExtiPending = 0;

/** 软件消抖时间阈值（单位：ms） */
#define DEBOUNCE_TIME_MS    (20U)

/* ------------------------ 函数实现 ------------------------ */

/**
 * @brief 初始化计数传感器（配置 GPIO + EXTI + NVIC）
 * 
 * 根据 CountSensor.h 中的用户配置，完成以下初始化：
 * - 使能 GPIO 和 AFIO 时钟
 * - 配置 GPIO 浮空输入
 * - 将指定引脚映射到 EXTI 线
 * - 配置 EXTI 触发模式（上升沿/下降沿）
 * - 配置 NVIC 中断优先级
 */
void CountSensor_Init(void)
{
    /* 1. 使能 GPIO 和 AFIO 时钟 */
    RCC_APB2PeriphClockCmd(COUNT_SENSOR_RCC_APB2_PERIPH | RCC_APB2Periph_AFIO, ENABLE);

    /* 2. 配置 GPIO 为输入模式 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = COUNT_SENSOR_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(COUNT_SENSOR_PORT, &GPIO_InitStructure);
	
    /* 3. 将 GPIO 引脚映射到 EXTI 线 */
    GPIO_EXTILineConfig(COUNT_SENSOR_GPIO_PORT_SOURCE, COUNT_SENSOR_PIN_SOURCE);

    /* 4. 配置 EXTI 中断线 */
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line    = COUNT_SENSOR_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = COUNT_SENSOR_TRIGGER;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* 5. 配置 NVIC 中断优先级 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // 设置优先级分组
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                    = COUNT_SENSOR_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 1;  // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 1;  // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief EXTI15_10 中断服务函数（需根据实际引脚修改函数名）
 * 
 * 仅执行最简操作：
 * - 检查中断标志
 * - 清除中断挂起位（必须！）
 * - 记录当前时间戳
 * - 置位待处理标志
 * 
 * @note 此函数名称必须与所用 EXTI 线匹配（如 Pin_13 → EXTI15_10_IRQHandler）
 */
void EXTI2_IRQHandler(void)
{
    if (EXTI_GetITStatus(COUNT_SENSOR_EXTI_LINE) == SET)
    {
        EXTI_ClearITPendingBit(COUNT_SENSOR_EXTI_LINE);  // 清除中断标志（关键！）
        g_LastIntTime = SysTick_Get();                   // 记录中断时间
        g_ExtiPending = 1;                               // 标记有待处理事件
    }
}

/**
 * @brief 软件消抖与有效信号确认（由 CountSensor_Get() 调用）
 * 
 * 在主循环中调用，完成以下逻辑：
 * 1. 检查是否有待处理中断事件；
 * 2. 若距离上次中断已超过消抖时间，则读取当前引脚电平；
 * 3. 若电平符合预期（如下降沿后应为低），则视为有效脉冲，计数+1；
 * 4. 清除待处理标志，防止重复处理。
 * 
 * @note 此函数不应在中断中调用，避免阻塞。
 */
static void CountSensor_ProcessDebounce(void)
{
    if (g_ExtiPending)
    {
        /* 检查是否已过消抖时间 */
        if ((SysTick_Get() - g_LastIntTime) >= DEBOUNCE_TIME_MS) {
							
				/* 读取当前引脚电平，并与预期值比较 */
			if (GPIO_ReadInputDataBit(COUNT_SENSOR_PORT, COUNT_SENSOR_PIN) == EXPECTED_LEVEL_AFTER_EDGE) {
					CountSensor_Count++;  // 有效脉冲，计数增加
			}
			g_ExtiPending = 0;        // 清除标志，完成本次处理
        }
        /* 否则：等待下一次轮询（不阻塞主循环） */
    }
}

/**
 * @brief 获取当前传感器计数值（线程安全，主循环调用）
 * 
 * 此函数会先尝试处理待定的中断事件（消抖），然后返回最新计数值。
 * 建议在主循环中定期调用，以及时更新计数结果。
 * 
 * @return uint16_t 当前累计的有效脉冲次数
 */
uint16_t CountSensor_Get(void)
{
    CountSensor_ProcessDebounce();  // 处理可能的待定事件
    return CountSensor_Count;
}
