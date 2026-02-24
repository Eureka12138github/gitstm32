#include "stm32f10x.h"                  // 设备头文件

/**
 * @brief PWM初始化函数
 * @details 配置TIM2为PWM输出模式，通过PA0引脚输出PWM波形控制LED亮度
 */
void PWM_Init(void)
{
    /* 
     * ==================== 1. 时钟与GPIO配置 ====================
     * 使能所需外设时钟并配置GPIO引脚
     */
    
    // 使能定时器和GPIO时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   // TIM2挂载在APB1总线上
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // PA0引脚需要GPIOA时钟
    
    // 引脚重映射相关配置（当前被注释掉）
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);  // 部分重映射PA0→PA15
    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // 解除调试端口默认功能
    
    /*
     * 引脚重映射说明：
     * 
     * 三种常见重映射场景：
     * 1. 调试端口转普通IO口：
     *    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
     *    GPIO_PinRemapConfig(X, ENABLE);
     * 
     * 2. 定时器引脚重映射：
     *    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
     *    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
     * 
     * 3. 重映射调试端口时需组合使用上述两种方法
     * 
     * 注意事项：
     * - GPIO_Remap_SWJ_Disable参数慎用，可能导致无法烧录程序
     * - 详细映射关系请参考手册8.3表35和表43
     */
    
    // 配置PWM输出引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    // 复用推挽输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;          // 使用PA0引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 输出速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /*
     * GPIO模式说明：
     * GPIO_Mode_AF_PP = 复用推挽输出
     * 作用：将引脚控制权交给外设（TIM2），使PWM波形能够正常输出
     * 类比：就像把方向盘交给自动驾驶系统，让专业的系统来控制
     */
    
    /* 
     * ==================== 2. 定时器基本参数配置 ====================
     * 配置TIM2的时基单元参数
     */
    
    TIM_InternalClockConfig(TIM2);  // 配置TIM2使用内部时钟
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    
    // 时钟分频配置
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 不分频
    
    // 重复计数器（仅高级定时器使用）
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    
    // 自动重装载值ARR - 决定PWM周期
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;  // PWM周期100个计数单位
    
    // 计数模式
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
    
    // 预分频系数PSC - 决定计数频率
    TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;  // 72MHz/720 = 100kHz
    
    /*
     * PWM参数配置详解：
     * 
     * 时基单元组成：PSC(预分频器)、ARR(自动重装器)、CNT(计数器)
     * 
     * 本配置产生的PWM特性：
     * - 系统主频：72MHz
     * - 预分频后频率：72MHz/720 = 100kHz（计数频率）
     * - PWM频率：100kHz/100 = 1kHz（1毫秒周期）
     * - 分辨率：1%（因为ARR=100）
     * 
     * 计算公式：
     * PWM频率 = 计数频率/(ARR+1) = 100kHz/100 = 1kHz
     * 占空比 = CCR/(ARR+1) × 100%
     * 
     * 参数取值范围：0~65535
     * 配置原则：
     * - PSC小、ARR大：高频计多数，分辨率高
     * - PSC大、ARR小：低频计少数，分辨率低
     */
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);  // 初始化时基单元
    
    /* 
     * ==================== 3. PWM输出通道配置 ====================
     * 配置输出比较通道实现PWM功能
     */
    
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);  // 初始化结构体默认值
    
    // 输出比较模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;      // PWM模式1
    
    // 输出极性
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  // 高电平有效
    
    // 输出使能
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  // 使能输出
    
    // 脉冲宽度（占空比控制）
    TIM_OCInitStructure.TIM_Pulse = 0;  // 初始占空比0%（CCR值）
    
    /*
     * PWM配置说明：
     * 
     * 当前配置效果：
     * - PWM频率：1kHz
     * - 初始占空比：0%
     * - 分辨率：1%
     * - 最大占空比：100%
     * 
     * 占空比调节原理：
     * 占空比 = CCR/(ARR+1) × 100%
     * 在本配置中：占空比 = CCR/100 × 100% = CCR%
     * 所以直接修改CCR值就可以改变占空比百分比
     */
    
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);  // 初始化通道1
    
    /* 
     * ==================== 4. 启动定时器 ====================
     * 最后一步：使能定时器开始PWM输出
     */
    TIM_Cmd(TIM2, ENABLE);  // 启动TIM2定时器
}

/**
 * @brief 设置PWM占空比
 * @param Compare 要设置的比较值（0-100对应0%-100%占空比）
 * @details 通过修改CCR寄存器值来调节PWM占空比
 * @note 在当前配置下，参数值直接等于占空比百分比
 */
void PWM_SetCompare1(uint16_t Compare)
{
    /*
     * 占空比调节说明：
     * 
     * 根据PWM配置：
     * - ARR+1 = 100（自动重装载值）
     * - 占空比公式：CCR/(ARR+1) × 100%
     * - 即：占空比 = CCR/100 × 100% = CCR%
     * 
     * 使用方法：
     * PWM_SetCompare1(50);  // 设置50%占空比
     * PWM_SetCompare1(0);   // 设置0%占空比（全暗）
     * PWM_SetCompare1(100); // 设置100%占空比（全亮）
     */
    
    TIM_SetCompare1(TIM2, Compare);  // 设置通道1的比较值
}
