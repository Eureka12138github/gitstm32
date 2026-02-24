#include "stm32f10x.h"                  // 设备头文件

/**
 * @brief 输入捕获初始化函数（PWMI模式）
 * @details 配置TIM3为PWMI模式，同时测量信号频率和占空比
 * @note 使用两个通道捕获同一信号：通道1测周期，通道2测高电平时间
 */
void IC_Init(void)
{
    /* 
     * ==================== 1. 时钟配置 ====================
     * 使能定时器和GPIO所需时钟
     */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   // TIM3挂载在APB1总线上
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // PA6引脚需要GPIOA时钟
    
    /* 
     * ==================== 2. GPIO引脚配置 ====================
     * 配置输入捕获引脚
     */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     // 上拉输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;         // 使用PA6引脚(TI3_CH1)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 输入速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 
     * ==================== 3. 定时器时基配置 ====================
     * 配置TIM3的基本工作参数
     */
    TIM_InternalClockConfig(TIM3);  // 配置TIM3使用内部时钟
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    
    // 时钟分频配置
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 不分频
    
    // 重复计数器（仅高级定时器使用）
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    
    // 自动重装载值ARR - 设置为最大值防止溢出
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;  // 16位计数器最大值
    
    // 计数模式
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
    
    // 预分频系数PSC - 决定计数频率
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;  // 72MHz/72 = 1MHz
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);  // 初始化时基单元
    
    /* 
     * ==================== 4. PWMI模式配置 ====================
     * 配置输入捕获单元实现PWMI功能
     */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    
    // 通道选择
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;        // 使用通道1
    
    // 极性选择
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  // 捕获上升沿
    
    // 输入选择
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  // 直连输入
    
    // 输入分频
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;   // 不分频
    
    // 数字滤波
    TIM_ICInitStructure.TIM_ICFilter = 0xF;  // 最强滤波(15个采样周期)
    
    // PWMI模式配置：同时配置通道1和通道2
    TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);
    
    /*
     * PWMI模式原理：
     * - 通道1：捕获上升沿，测量完整周期
     * - 通道2：捕获下降沿，测量高电平时间
     * - 占空比 = 高电平时间 / 周期时间
     */
    
    /* 
     * ==================== 5. 主从模式配置 ====================
     * 配置触发和复位机制
     */
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);  // 选择TI1FP1作为触发源
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);  // 复位模式：捕获时自动清零CNT
    
    /* 
     * ==================== 6. 启动定时器 ====================
     */
    TIM_Cmd(TIM3, ENABLE);  // 启动TIM3定时器
}

/**
 * @brief 获取测量频率值
 * @return uint32_t 测量到的频率值(Hz)
 * @details 使用测周法计算频率：fx = fc/N
 * @note fc = 1MHz, N为周期计数值
 */
uint32_t IC_GetFreq(void)
{
    // fx = fc/N = 1000000/(周期计数值+1)
    return 1000000 / (TIM_GetCapture1(TIM3) + 1);
}

/**
 * @brief 获取测量占空比
 * @return uint32_t 占空比百分比值(0-100)
 * @details 占空比 = (高电平时间/周期时间) × 100%
 */
uint32_t IC_GetDuty(void)
{
    /*
     * 占空比计算公式：
     * Duty% = (CCR2/CCR1) × 100%
     * 
     * 其中：
     * CCR1 = 周期计数值（通道1捕获）
     * CCR2 = 高电平时间计数值（通道2捕获）
     * 
     * +1修正：补偿硬件计数从0开始的偏差
     * ×100：转换为百分比形式
     */
    
    uint32_t period = TIM_GetCapture1(TIM3) + 1;   // 周期计数值
    uint32_t high_time = TIM_GetCapture2(TIM3) + 1; // 高电平时间计数值
    
    return (high_time * 100) / period;
}
