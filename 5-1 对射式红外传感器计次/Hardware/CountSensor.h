/**
 * @file CountSensor.h
 * @brief 脉冲计数传感器驱动头文件（基于 STM32F10x EXTI 中断）
 * 
 * 本模块通过外部中断（EXTI）实现对 GPIO 引脚上脉冲信号的可靠计数，
 * 支持上升沿或下降沿触发，并包含软件消抖机制。
 * 
 * ⚠️【使用前必读】配置流程（三步）：
 * 1. 在下方 "用户配置区" 设置引脚、端口、触发方式；
 * 2. 根据所选引脚，取消对应 COUNT_SENSOR_IRQN 的注释；
 * 3. 在 CountSensor.c 中实现匹配的中断服务函数（如 EXTI2_IRQHandler）。
 * 
 * 📌 引脚与 IRQ 映射关系（STM32F1 系列）：
 *   - Pin_0  → EXTI0_IRQn      → ISR: EXTI0_IRQHandler
 *   - Pin_1  → EXTI1_IRQn      → ISR: EXTI1_IRQHandler
 *   - Pin_2  → EXTI2_IRQn      → ISR: EXTI2_IRQHandler
 *   - Pin_3  → EXTI3_IRQn      → ISR: EXTI3_IRQHandler
 *   - Pin_4  → EXTI4_IRQn      → ISR: EXTI4_IRQHandler
 *   - Pin_5~9 → EXTI9_5_IRQn   → ISR: EXTI9_5_IRQHandler
 *   - Pin_10~15→ EXTI15_10_IRQn→ ISR: EXTI15_10_IRQHandler
 * 
 * ✅ 示例：使用 PA2 下降沿触发
 *   #define COUNT_SENSOR_PORT_NUM    0
 *   #define COUNT_SENSOR_PIN         GPIO_Pin_2
 *   #define COUNT_SENSOR_PIN_SOURCE  GPIO_PinSource2
 *   #define COUNT_SENSOR_TRIGGER     EXTI_Trigger_Falling
 *   #define EXPECTED_LEVEL_AFTER_EDGE   (uint8_t)Bit_RESET
 *   #define COUNT_SENSOR_IRQN        EXTI2_IRQn   // 取消此行注释
 *   并在 .c 中实现 void EXTI2_IRQHandler(void)
 */

#ifndef COUNT_SENSOR_H
#define COUNT_SENSOR_H

#include "stm32f10x.h"

/* ==================== 用户配置区（请按实际硬件修改）==================== */

/**
 * @brief 指定传感器连接的 GPIO 端口号
 * 
 * 取值范围：
 *   0 → GPIOA
 *   1 → GPIOB
 *   2 → GPIOC
 *   3 → GPIOD
 * 
 * @note 不支持其他端口（如 GPIOE），如需扩展请修改下方推导逻辑。
 */
#define COUNT_SENSOR_PORT_NUM       0

/**
 * @brief 指定传感器连接的 GPIO 引脚（必须与 PIN_SOURCE 一致）
 * 
 * 示例：GPIO_Pin_2 表示第 2 号引脚（即 PA2 / PB2 等）
 * 
 * @warning 必须使用 stm32f10x.h 中定义的标准宏（如 GPIO_Pin_0 ~ GPIO_Pin_15）
 */
#define COUNT_SENSOR_PIN            GPIO_Pin_2

/**
 * @brief 指定引脚复用功能源（用于 EXTI 映射）
 * 
 * 必须与 COUNT_SENSOR_PIN 对应：
 *   GPIO_Pin_2    → GPIO_PinSource2
 *   GPIO_Pin_10   → GPIO_PinSource10
 * 
 * @note 此值用于 GPIO_EXTILineConfig() 函数
 */
#define COUNT_SENSOR_PIN_SOURCE     GPIO_PinSource2

/**
 * @brief 指定 EXTI 触发边沿类型
 * 
 * 可选值（来自 stm32f10x_exti.h）：
 *   - EXTI_Trigger_Rising      : 上升沿触发
 *   - EXTI_Trigger_Falling     : 下降沿触发
 *   - EXTI_Trigger_Rising_Falling : 双边沿（本模块暂不支持）
 * 
 * @note 触发方式将影响 GPIO 输入模式（上拉/下拉）和消抖电平判断
 */
#define COUNT_SENSOR_TRIGGER        EXTI_Trigger_Falling  //Falling 对应RESET
#define EXPECTED_LEVEL_AFTER_EDGE   (uint8_t)Bit_RESET

/* ---------------------------------------------------------------------- */
/* ⚙️ 中断号配置（手动启用一项！）                                        */
/* ---------------------------------------------------------------------- */

// #define COUNT_SENSOR_IRQN EXTI0_IRQn
// #define COUNT_SENSOR_IRQN EXTI1_IRQn
#define COUNT_SENSOR_IRQN EXTI2_IRQn
// #define COUNT_SENSOR_IRQN EXTI3_IRQn
// #define COUNT_SENSOR_IRQN EXTI4_IRQn
// #define COUNT_SENSOR_IRQN EXTI9_5_IRQn
// #define COUNT_SENSOR_IRQN EXTI15_10_IRQn

/* ---------------------------------------------------------------------- */
/* 🔧 自动推导区（用户无需修改，由上述配置自动生成）                     */
/* ---------------------------------------------------------------------- */

/* 根据 PORT_NUM 推导实际端口、时钟和 AFIO 映射源 */
#if COUNT_SENSOR_PORT_NUM == 0
    #define COUNT_SENSOR_PORT               GPIOA
    #define COUNT_SENSOR_RCC_APB2_PERIPH    RCC_APB2Periph_GPIOA
    #define COUNT_SENSOR_GPIO_PORT_SOURCE   GPIO_PortSourceGPIOA
#elif COUNT_SENSOR_PORT_NUM == 1
    #define COUNT_SENSOR_PORT               GPIOB
    #define COUNT_SENSOR_RCC_APB2_PERIPH    RCC_APB2Periph_GPIOB
    #define COUNT_SENSOR_GPIO_PORT_SOURCE   GPIO_PortSourceGPIOB
#elif COUNT_SENSOR_PORT_NUM == 2
    #define COUNT_SENSOR_PORT               GPIOC
    #define COUNT_SENSOR_RCC_APB2_PERIPH    RCC_APB2Periph_GPIOC
    #define COUNT_SENSOR_GPIO_PORT_SOURCE   GPIO_PortSourceGPIOC
#elif COUNT_SENSOR_PORT_NUM == 3
    #define COUNT_SENSOR_PORT               GPIOD
    #define COUNT_SENSOR_RCC_APB2_PERIPH    RCC_APB2Periph_GPIOD
    #define COUNT_SENSOR_GPIO_PORT_SOURCE   GPIO_PortSourceGPIOD
#else
    #error "Unsupported port number! Please use 0 (GPIOA), 1 (GPIOB), 2 (GPIOC), or 3 (GPIOD)."
#endif

/**
 * @brief EXTI 中断线编号（由引脚自动转换）
 * 
 * STM32 中 EXTI_LineX 对应 GPIO_Pin_X，因此直接转换为 uint32_t 即可。
 */
#define COUNT_SENSOR_EXTI_LINE      ((uint32_t)(COUNT_SENSOR_PIN))

/* ==================== 公共函数声明 ==================== */

/**
 * @brief 初始化计数传感器（配置 GPIO、EXTI、NVIC）
 * 
 * 调用一次即可完成硬件初始化。
 * 
 * @note 必须在 main() 中调用，且早于任何中断发生。
 */
void CountSensor_Init(void);

/**
 * @brief 获取当前累计的有效脉冲计数值
 * 
 * 此函数会自动处理待定的中断事件（软件消抖），
 * 并返回最新计数值。建议在主循环中定期调用。
 * 
 * @return uint16_t 当前计数值（从 0 开始累加）
 */
uint16_t CountSensor_Get(void);

#endif /* COUNT_SENSOR_H */
