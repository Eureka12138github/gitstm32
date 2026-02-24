#include "stm32f10x.h"                  // 设备头文件

/**
 * @brief 编码器初始化函数
 * @details 配置TIM3为编码器接口模式，用于测量旋转编码器的转速和方向
 * @note 使用PA6(TI3_CH1)和PA7(TI3_CH2)作为编码器信号输入
 */
void Encoder_Init(void)
{
    /* 
     * ==================== 1. 时钟配置 ====================
     * 使能定时器和GPIO所需时钟
     */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   // TIM3挂载在APB1总线上
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // PA6、PA7引脚需要GPIOA时钟
    
    /* 
     * ==================== 2. GPIO引脚配置 ====================
     * 配置编码器信号输入引脚
     */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     // 上拉输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // 使用PA6和PA7引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 输入速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /*
     * 引脚功能说明：
     * PA6：编码器A相信号（TI3_CH1）
     * PA7：编码器B相信号（TI3_CH2）
     * 上拉输入确保信号稳定，避免悬空抖动
     */
    
    /* 
     * ==================== 3. 定时器时基配置 ====================
     * 配置TIM3的基本工作参数
     */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    
    // 时钟分频配置
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 不分频
    
    // 重复计数器（仅高级定时器使用）
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    
    // 自动重装载值ARR - 设置为最大值防止溢出
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;  // 16位计数器最大值
    
    // 计数模式
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
    
    // 预分频系数PSC - 设置为0（不分频）
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;  // PSC=0，直接使用系统主频
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);  // 初始化时基单元
    
    /* 
     * ==================== 4. 编码器接口模式配置 ====================
     * 配置输入捕获单元并设置编码器模式
     */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);  // 初始化结构体默认值
    
    // 通道1配置（A相信号）
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;  // 最强滤波(15个采样周期)
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    // 通道2配置（B相信号）
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;  // 相同滤波设置
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    // 编码器接口模式配置
    TIM_EncoderInterfaceConfig(
        TIM3,                          // 定时器选择
        TIM_EncoderMode_TI12,         // 使用TI1和TI2两个通道
        TIM_ICPolarity_Rising,        // A相信号上升沿有效
        TIM_ICPolarity_Falling        // B相信号下降沿有效
    );
    
    /*
     * 编码器模式工作原理：
     * - TI12模式：同时使用通道1和通道2
     * - 根据A、B相信号的相位关系判断旋转方向
     * - 计数器自动增减：同向+1，反向-1
     * - 硬件自动处理编码器逻辑，无需软件干预
     */
    
    /* 
     * ==================== 5. 启动定时器 ====================
     */
    TIM_Cmd(TIM3, ENABLE);  // 启动TIM3定时器
}

/**
 * @brief 获取编码器计数值
 * @return int16_t 当前编码器计数值
 * @details 返回后清零计数器，便于下次测量
 * @note 计数值正负表示旋转方向，绝对值表示转速
 */
int16_t Encoder_Get(void)
{
    int16_t temp;
    
    temp = TIM_GetCounter(TIM3);  // 读取当前计数值
    TIM_SetCounter(TIM3, 0);      // 清零计数器，准备下一次测量
    
    return temp;
}
