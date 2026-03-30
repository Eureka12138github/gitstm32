#include "stm32f10x.h"                  // 设备头文件

/**
 * @brief ADC初始化函数
 * @details 配置ADC1为单通道连续转换模式，用于采集PA0引脚的模拟信号
 * @note 采用右对齐数据格式，12位分辨率
 */
void AD_Init(void)
{
    /* 
     * ==================== 1. 时钟配置 ====================
     * 使能ADC和GPIO所需时钟
     */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   // 使能ADC1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能GPIOA时钟
    
    // 配置ADC时钟分频：PCLK2/6 = 72MHz/6 = 12MHz
    // ADC最大工作频率为14MHz，12MHz在安全范围内
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    /* 
     * ==================== 2. GPIO引脚配置 ====================
     * 配置ADC输入引脚
     */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;      // 模拟输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;          // 使用PA0引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 输入速度50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /*
     * AIN模式说明：
     * - 断开GPIO数字电路，避免数字噪声干扰模拟信号
     * - 专为ADC设计的输入模式
     * - 确保模拟信号完整性
     */
    
    /* 
     * ==================== 3. ADC参数配置 ====================
     * 配置ADC基本工作参数
     */
    ADC_InitTypeDef ADC_InitStructure;
    
    // 工作模式：独立模式（单ADC）
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    
    // 扫描模式：单通道（不扫描）
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    
    // 转换模式：连续转换（自动重复转换）
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    
    // 触发源：软件触发（无需外部触发）
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    
    // 数据对齐：右对齐（低12位有效）
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    
    // 通道数量：1个通道
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    
    ADC_Init(ADC1, &ADC_InitStructure);  // 初始化ADC
    
    /* 
     * ==================== 4. 通道配置 ====================
     * 配置具体采样通道
     */
    // 配置通道0（PA0），采样时间55.5个周期
    // 采样时间越长，精度越高，但转换速度越慢
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    
    /* 
     * ==================== 5. ADC启动与校准 ====================
     */
    ADC_Cmd(ADC1, ENABLE);  // 开启ADC电源
    
    // ADC校准流程
    ADC_ResetCalibration(ADC1);  // 复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1) == SET);  // 等待复位完成
    
    ADC_StartCalibration(ADC1);  // 开始校准
    while(ADC_GetCalibrationStatus(ADC1) == SET);  // 等待校准完成
    
    // 启动连续转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
 * @brief 获取ADC转换值
 * @return uint16_t 12位ADC转换结果（0-4095）
 * @details 在连续转换模式下，直接读取最新转换结果
 * @note 返回值范围：0（0V）到4095（参考电压）
 */
uint16_t AD_GetValue(void)
{
    // 连续转换模式下，数据寄存器持续更新
    // 无需等待转换完成标志，直接读取最新值
    return ADC_GetConversionValue(ADC1);
}
