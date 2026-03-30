#include "stm32f10x.h"                  // 设备头文件

/**
 * @brief 定时器初始化函数
 * @details 配置TIM2为内部时钟定时器模式，实现精确的定时中断功能
 */
void Timer_Init(void)
{
    /* 
     * ==================== 1. 时钟使能配置 ====================
     * 使能TIM2定时器所需的时钟源
     */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // TIM2挂载在APB1总线上
    
    /* 
     * ==================== 2. 定时器时钟源配置 ====================
     * 配置定时器的时钟来源
     */
    TIM_InternalClockConfig(TIM2);  // 配置TIM2使用内部时钟驱动（系统默认）
    
    /* 
     * ==================== 3. 定时器基本参数配置 ====================
     * 配置定时器的核心参数：预分频器(PSC)、自动重装载值(ARR)等
     */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    
    // 时钟分频配置 - 与时基单元滤波频率相关
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  
    
    // 重复计数器 - 仅高级定时器使用，通用定时器设为0
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    
    // 自动重装载值ARR - 决定计数周期
    TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;  // 减1补偿硬件自动加1
    
    // 计数模式 - 设置为向上计数
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    // 预分频系数PSC - 对主频进行分频
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;  // 72MHz/7200 = 10kHz
    
    /*
     * 参数配置说明：
     * 
     * 时基单元组成：PSC(预分频器)、ARR(自动重装器)、CNT(计数器)
     * 计数器溢出频率计算公式：
     * CK_CNT_OV = CK_CNT/(ARR+1) = CK_PSC/(PSC+1)/(ARR+1)
     * 
     * 本配置详解：
     * - 系统主频：72MHz
     * - 预分频器：7200分频 → 72MHz/7200 = 10kHz计数频率
     * - 自动重装值：10000 → 10kHz/10000 = 1Hz溢出频率
     * - 最终效果：每1秒产生一次更新事件和中断
     * 
     * 参数取值范围：0~65535
     * 配置原则：
     * - PSC小、ARR大：高频计多数
     * - PSC大、ARR小：低频计少数
     * - 结果相同，可根据应用需求选择
     */
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);  // 初始化TIM2时基单元
    
    /* 
     * ==================== 4. 中断标志位处理 ====================
     * 解决初始化时的更新事件副作用问题
     */
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    
    /*
     * 重要说明：
     * TIM_TimeBaseInit初始化会产生副作用：
     * - 更新事件和更新中断同时发生
     * - 初始化完成时会立即触发中断
     * - 导致系统复位后立即进入中断服务程序
     * 
     * 技术原理：
     * PSC和ARR都有缓冲寄存器，只有在更新事件时才会真正生效
     * 初始化时需要手动触发更新事件使配置立即生效
     * 但这样会产生不必要的中断，所以需要清除更新标志位
     * 
     * 相关概念：
     * - 更新中断：需要CPU处理的中断请求
     * - 更新事件：定时器内部事件，可触发其他操作
     * 两者通常同时发生，但处理方式不同
     */
    
    /* 
     * ==================== 5. 中断系统配置 ====================
     * 配置定时器中断使能和NVIC中断控制器
     */
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  // 使能TIM2更新中断
    
    /*
     * 中断配置说明：
     * 选择更新中断的原因：
     * - 需要定时器定期触发中断处理
     * - 更新中断在计数器溢出时产生
     * - 适合周期性任务处理
     */
    
    // NVIC优先级分组配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    /*
     * NVIC配置说明：
     * 所有中断都需要NVIC管理
     * 配置流程类似于GPIO初始化
     * 需要理解各参数的具体功能
     */
    
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 配置TIM2中断通道
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;                    // TIM2中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    // 使能中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;          // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                 // 响应优先级
    
    /*
     * 优先级配置说明：
     * 当前系统只有一个中断源，优先级配置影响不大
     * 在多中断系统中需要合理规划优先级
     */
    
    NVIC_Init(&NVIC_InitStructure);
    
    /* 
     * ==================== 6. 定时器启动 ====================
     * 最后一步：使能定时器开始工作
     */
    TIM_Cmd(TIM2, ENABLE);  // 启动TIM2定时器
}

/*
 * 中断服务函数示例（当前被注释掉）
 * 
 * void TIM2_IRQHandler(void)
 * {
 *     if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
 *     {
 *         Num++;                                    // 中断处理代码
 *         TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除中断标志
 *     }
 * }
 * 
 * 使用说明：
 * - 中断函数名必须与启动文件中定义的IRQn一致
 * - 需要在main函数中定义全局变量Num
 * - 每次定时器溢出时会自动执行此函数
 */
