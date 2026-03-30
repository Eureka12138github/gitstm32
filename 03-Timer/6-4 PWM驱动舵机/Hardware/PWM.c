#include "stm32f10x.h"                  // 设备头文件

/**
 * @brief PWM初始化函数
 * @details 配置TIM2为PWM输出模式，用于舵机控制
 * @note 舵机控制需要50Hz的PWM频率，通过PA1引脚输出
 */
void PWM_Init(void)
{
    /* 
     * ==================== 时钟配置 ====================
     * 使能定时器和GPIO所需时钟
     */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   // TIM2挂载在APB1总线上
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // PA1引脚需要GPIOA时钟
    
    /* 
     * ==================== GPIO引脚配置 ====================
     * 配置PWM输出引脚
     */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    // 复用推挽输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;          // 使用PA1引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 输出速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /*
     * GPIO模式说明：
     * GPIO_Mode_AF_PP = 复用推挽输出
     * 作用：将引脚控制权交给外设(TIM2)，使PWM波形能够正常输出
     * 就像把汽车方向盘交给自动驾驶系统一样
     */
    
    /* 
     * ==================== 定时器时钟源配置 ====================
     */
    TIM_InternalClockConfig(TIM2);  // 配置TIM2使用内部时钟（系统默认）
    
    /* 
     * ==================== 定时器基本参数配置 ====================
     * 配置TIM2的时基单元参数
     */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    
    // 时钟分频配置
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 不分频
    
    // 重复计数器（仅高级定时器使用）
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    
    // 自动重装载值ARR - 决定PWM周期
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;  // 20ms周期（50Hz）
    
    // 计数模式
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
    
    // 预分频系数PSC - 决定计数频率
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;  // 72MHz/72 = 1MHz
    
    /*
     * PWM参数配置详解：
     * 
     * 时基单元组成：PSC(预分频器)、ARR(自动重装器)、CNT(计数器)
     * 计数器溢出频率计算：CK_CNT_OV = CK_PSC/(PSC+1)/(ARR+1)
     * 
     * 舵机控制配置：
     * - 系统主频：72MHz
     * - 预分频后频率：72MHz/72 = 1MHz（计数频率）
     * - PWM周期：1MHz/20000 = 50Hz（20ms周期）
     * - 舵机控制范围：0.5ms-2.5ms对应0°-180°
     * 
     * 参数说明：
     * - PSC和ARR都有缓冲寄存器，用于安全的参数更新
     * - 参数取值范围：0~65535
     * - 硬件会自动对设置值加1，所以需要减1补偿
     */
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);  // 初始化时基单元
    
    /* 
     * ==================== PWM输出通道配置 ====================
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
    
    // 脉冲宽度（初始占空比0%）
    TIM_OCInitStructure.TIM_Pulse = 0;  // CCR初始值为0
    
    /*
     * 舵机控制说明：
     * 当前配置产生50Hz的PWM信号
     * 通过改变CCR值来控制舵机角度：
     * - 0.5ms脉宽对应0°（CCR ≈ 500）
     * - 1.5ms脉宽对应90°（CCR ≈ 1500）
     * - 2.5ms脉宽对应180°（CCR ≈ 2500）
     * 
     * 多通道扩展：
     * 同一定时器的不同通道可以独立控制多个舵机
     * 相位和频率相同，但占空比可各自调节
     */
    
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);  // 初始化通道2（PA1）
    
    /* 
     * ==================== 启动定时器 ====================
     */
    TIM_Cmd(TIM2, ENABLE);  // 启动TIM2定时器
}

/**
 * @brief 设置PWM通道2占空比
 * @param Compare 要设置的比较值
 * @details 通过修改CCR寄存器值来调节PWM占空比
 * @note 对于舵机控制，参数值直接影响脉冲宽度
 */
void PWM_SetCompare2(uint16_t Compare)
{
    /*
     * 舵机控制参数说明：
     * 
     * 舵机角度与CCR值对应关系：
     * - CCR = 500  → 0.5ms脉宽 → 0°
     * - CCR = 1500 → 1.5ms脉宽 → 90°（中位）
     * - CCR = 2500 → 2.5ms脉宽 → 180°
     * 
     * 使用示例：
     * PWM_SetCompare2(1500);  // 舵机转到90°位置
     * PWM_SetCompare2(500);   // 舵机转到0°位置
     * PWM_SetCompare2(2500);  // 舵机转到180°位置
     */
    
    TIM_SetCompare2(TIM2, Compare);  // 设置通道2的比较值
}

/**
 * @brief 设置PWM预分频系数
 * @param Prescaler 预分频系数值
 * @details 通过修改PSC值来改变PWM频率
 * @note 舵机控制通常不需要改变频率，主要用于特殊应用场合
 */
void PWM_SetPrescaler(uint16_t Prescaler)
{
    /*
     * 频率调节说明：
     * 
     * 为什么通常不改变ARR而改变PSC：
     * - ARR值与占空比计算直接相关
     * - 改变ARR会影响现有的占空比设置
     * - 改变PSC只影响频率，不影响占空比
     * 
     * 重载模式选择：
     * TIM_PSCReloadMode_Immediate：立即更新PSC值
     * - 优点：响应迅速
     * - 缺点：产生不完整的PWM周期
     * 
     * TIM_PSCReloadMode_Update：更新事件时更新PSC值
     * - 优点：保证PWM周期完整性
     * - 缺点：有一定延迟
     * 
     * 对于舵机控制，建议使用Update模式保证信号稳定性
     */
    
    TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Update);  // 设置PSC值
}
