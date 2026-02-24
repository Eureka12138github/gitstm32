#include "stm32f10x.h"                  // 设备头文件

// 外部变量声明（当前被注释掉）
// extern uint16_t Num;

/**
 * @brief 定时器初始化函数
 * @details 配置TIM2为外部时钟模式，用于计数外部脉冲信号
 * @note 实际计数机制：每50个外部上升沿使Num加1
 */
void Timer_Init(void)
{
    // 1. 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // TIM2挂在APB1总线上
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // PA0引脚需要GPIOA时钟
    
    // 2. 配置GPIO引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         // 上拉输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;             // 使用PA0引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 输出速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 
     * 说明：为什么选择上拉输入而不是浮空输入？
     * - 浮空输入容易受干扰，电平不稳定会频繁跳变
     * - 上拉输入相对稳定，适合大多数应用场景
     * - 浮空输入适用于：外部信号很微弱，内部上拉电阻可能影响信号的情况
     */
    
    // 3. 配置外部时钟模式
    TIM_ETRClockMode2Config(TIM2,                    // 选择TIM2定时器
                           TIM_ExtTRGPSC_OFF,        // 外部信号不分频（保持原始频率）
                           TIM_ExtTRGPolarity_NonInverted, // 上升沿有效（不反相）
                           0x04);                    // 数字滤波器设置（0x00~0x0F）
    
    /*
     * TIM_ETRClockMode2Config参数详解：
     * 参数1：定时器外设选择
     * 参数2：是否对外部信号分频（OFF表示不分频）
     * 参数3：信号极性（NonInverted表示上升沿触发，Inverted表示下降沿触发）
     * 参数4：数字滤波强度（数值越大滤波越强，但会有延迟）
     *
     * 关于滤波器：STM32内部硬件滤波，可以有效消除信号抖动
     * 关于模式选择：Mode2比Mode1更灵活，可与其他触发源配合使用
     */
    
    // 4. 配置定时器基本参数
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // 时钟分割1
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;             // 重复计数器（仅高级定时器使用）
    TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;                   // 自动重装载值ARR=9（计数0-9共10个数）
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_Prescaler = 5 - 1;                 // 预分频系数PSC=4（每5个脉冲计数器+1）
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    
    /*
     * 核心计数机制说明：
     * - 外部脉冲通过PA0输入到TIM2
     * - 预分频器设置：每5个外部上升沿，计数器才加1
     * - 计数范围：0到9（共10个数）循环计数
     * - 溢出条件：当计数达到9后再次加1时产生更新事件
     * - 中断触发：每次溢出时触发更新中断
     * 
     * 实际效果：每50个外部上升沿（5×10=50）→ Num变量加1
     * 显示效果：CNT在0-9间循环，每循环一次Num加1
     */
    
    // 5. 清除更新标志位
   TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    
    /*
     * 为什么要清除标志位？
     * - TIM_TimeBaseInit初始化完成后会立即产生更新事件
     * - 如果不清除，系统启动时就会进入中断
     * - 这样做确保计数从0开始
     */
    
    // 6. 配置中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);           // 使能更新中断
    
    // 7. 配置NVIC中断控制器
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);      // 设置优先级分组
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;                      // TIM2中断通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                      // 使能中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;            // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                   // 响应优先级
    NVIC_Init(&NVIC_InitStructure);
    
    // 8. 启动定时器
    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief 获取当前计数值
 * @return 当前计数器值（0-9循环）
 * @note 返回值在0-9之间循环变化，每5个外部脉冲增加1
 */
uint16_t Timer_GetCounter(void)
{
    return TIM_GetCounter(TIM2);
}

/**
 * @brief 获取预分频系数
 * @return 实际预分频值（返回值已加1）
 * @note 预分频器硬件存储值比实际值小1，所以需要加1
 */
uint16_t Timer_GetPrescaler(void)
{
    return TIM_GetPrescaler(TIM2) + 1;
}

/*
 * 示例中断服务函数（当前被注释掉）
 * 
 * void TIM2_IRQHandler(void)
 * {
 *     if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
 *     {
 *         Num++;                                    // 溢出计数器加1（每50个外部脉冲）
 *         TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除中断标志
 *     }
 * }
 * 
 * 使用说明：
 * - 在main函数中定义全局变量Num
 * - 取消注释extern声明
 * - 取消注释中断服务函数
 * - 每50个外部上升沿Num会自动加1
 */