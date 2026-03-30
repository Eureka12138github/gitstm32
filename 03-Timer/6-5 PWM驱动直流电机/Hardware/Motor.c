#include "stm32f10x.h"                  // 设备头文件
#include "PWM.h"                        // PWM控制库

/**
 * @brief 电机初始化函数
 * @details 初始化PWM模块和电机控制引脚
 * @note 电机控制需要3个引脚：2个方向控制引脚(PA4,PA5) + 1个PWM调速引脚
 */
void Motor_Init(void)
{
    /* 
     * ==================== PWM模块初始化 ====================
     * 初始化定时器PWM输出功能
     */
    PWM_Init();  // 初始化PWM输出（当前使用TIM2_CH3）
    
    /* 
     * ==================== 电机控制引脚配置 ====================
     * 配置电机方向控制引脚
     */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能GPIOA时钟
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       // 推挽输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; // 使用PA4和PA5引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 输出速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /*
     * 引脚功能说明：
     * PA4: 电机正转控制引脚
     * PA5: 电机反转控制引脚
     * PWM引脚(通常是PA2): 电机速度控制引脚
     * 
     * 控制逻辑：
     * PA4=1, PA5=0 → 电机正转
     * PA4=0, PA5=1 → 电机反转
     * PA4=0, PA5=0 → 电机停止
     */
}

/**
 * @brief 设置电机转速和方向
 * @param Speed 速度值：正值为正转，负值为反转，绝对值表示转速大小
 * @details 通过PWM占空比控制电机转速，通过GPIO电平控制转动方向
 * @note 速度范围通常为-100到+100，具体范围取决于PWM配置
 */
void Motor_SetSpeed(int8_t Speed)
{
    /*
     * 电机控制原理说明：
     * 
     * 直流电机控制需要两个要素：
     * 1. 方向控制：通过两个引脚的高低电平组合实现
     * 2. 速度控制：通过PWM占空比调节电机电压
     * 
     * 类比理解：
     * 就像开车一样：
     * - 挂前进档/倒车档 = 方向控制（GPIO）
     * - 踩油门深度 = 速度控制（PWM占空比）
     */
    
    if(Speed >= 0)
    {
        // 正转模式：PA4=1, PA5=0
        GPIO_SetBits(GPIOA, GPIO_Pin_4);     // 设置正转引脚为高电平
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);   // 设置反转引脚为低电平
        PWM_SetCompare3(Speed);              // 设置正转速度（PWM占空比）
    }
    else
    {
        // 反转模式：PA4=0, PA5=1
        GPIO_SetBits(GPIOA, GPIO_Pin_5);     // 设置反转引脚为高电平
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);   // 设置正转引脚为低电平
        PWM_SetCompare3(-Speed);             // 设置反转速度（取绝对值）
    }
    
    /*
     * 使用示例：
     * Motor_SetSpeed(50);   // 正转，50%速度
     * Motor_SetSpeed(-30);  // 反转，30%速度
     * Motor_SetSpeed(0);    // 停止
     * 
     * 注意事项：
     * - 负数参数会被转换为正数传递给PWM函数
     * - 实际转速还受到电机特性和电源电压影响
     * - 建议在使用前测试电机的最大安全转速
     */
}
