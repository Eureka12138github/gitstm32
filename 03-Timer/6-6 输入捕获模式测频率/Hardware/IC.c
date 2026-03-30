#include "stm32f10x.h"                  // 设备头文件

/**
 * @brief 输入捕获初始化函数
 * @details 配置TIM3为输入捕获模式，用于测量外部信号频率
 * @note 使用测周法：通过测量信号周期来计算频率
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
     * GPIO模式选择说明：
     * GPIO_Mode_IPU = 上拉输入
     * 优点：提高信号稳定性，减少干扰
     * 适用：大多数数字信号输入场景
     */
    
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
    
    /*
     * 测频参数说明：
     * 
     * 计数标准频率 fc = 系统主频/PSC = 72MHz/72 = 1MHz
     * 
     * PSC值选择原则：
     * - 根据待测信号频率范围选择
     * - 一般选择使计数频率略高于信号频率
     * - 72的选择便于计算（1MHz计数频率）
     * 
     * ARR设置说明：
     * - 设置为最大值避免计数溢出
     * - 确保能测量较长周期的信号
     */
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);  // 初始化时基单元
    
    /* 
     * ==================== 4. 输入捕获单元配置 ====================
     * 配置输入捕获功能参数
     */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    
    // 通道选择
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;        // 使用通道1(PA6)
    
    // 极性选择
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  // 捕获上升沿
    
    // 输入选择
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  // 直连输入
    
    // 输入分频
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;   // 不分频，保持原始信号
    
    // 数字滤波
    TIM_ICInitStructure.TIM_ICFilter = 0xF;  // 最强滤波(15个采样周期)
    
    TIM_ICInit(TIM3, &TIM_ICInitStructure);  // 初始化输入捕获单元
    
    /* 
     * ==================== 5. 主从模式配置 ====================
     * 配置定时器的触发和复位机制
     */
    // 配置触发输入源
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);  // 选择TI1FP1作为触发源
    
    // 配置从模式
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);  // 复位模式：捕获时自动清零CNT
    
    /*
     * 主从模式工作原理：
     * 1. 无信号时：CNT在内部时钟驱动下持续自增
     * 2. 信号到达时：捕获上升沿，CNT自动清零
     * 3. 下一个上升沿：捕获新的CNT值，即为周期计数值
     * 
     * 优势：自动复位机制简化了软件处理
     */
    
    /* 
     * ==================== 6. 启动定时器 ====================
     */
    TIM_Cmd(TIM3, ENABLE);  // 启动TIM3定时器
}

/**
 * @brief 获取测量频率值
 * @return uint32_t 测量到的频率值(Hz)
 * @details 使用测周法计算频率：fx = fc/N
 * @note fc = 1MHz, N为捕获到的周期计数值
 */
uint32_t IC_GetFreq(void)
{
    /*
     * 测周法频率计算公式：
     * fx = fc/N
     * 
     * 其中：
     * fx = 待测信号频率
     * fc = 计数标准频率 = 1MHz
     * N = 周期计数值 = CCR1 + 1
     * 
     * 为什么加1：
     * 硬件计数从0开始，所以实际周期数需要加1
     * 
     * 精度分析：
     * 分辨率 = fc/(2^16) = 1MHz/65536 ≈ 15.26Hz
     * 测量误差来源：
     * 1. 信号边沿抖动
     * 2. 计数器量化误差
     * 3. 系统时钟精度
     * 
     * 适用范围：
     * 最高可测频率：fc/2 = 500kHz
     * 最低可测频率：fc/65536 ≈ 15Hz
     */
    
    return 1000000 / (TIM_GetCapture1(TIM3) + 1);
}
