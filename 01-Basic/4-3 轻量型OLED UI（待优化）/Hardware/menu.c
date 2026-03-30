/**
 * @file menu.c
 * @brief 轻量级 OLED 菜单 UI 框架实现文件
 * 
 * 本文件实现了基于 STM32 的轻量级 OLED 菜单系统，支持：
 * - 父子页面导航结构
 * - 按键上下滚动浏览
 * - 长文本水平滚动显示
 * - 稳定的帧率控制机制
 * 
 * @author Eureka & Lingma
 * @date 2026-02-22
 */

#include "menu.h"
#include "menu_data.h"
#include "OLED.h"                    
#include "OLED_Fonts.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/* ============================================================================ */
/*                              全局配置参数                                    */
/* ============================================================================ */

/** @brief 字体宽度 (像素) */
#define FONT_WIDTH                  (16U)

/** @brief 字体高度 (像素) */
#define FONT_HEIGHT                 (16U)

/** @brief 行间距 (像素) */
#define LINE_SPACING                (4U)

/** @brief 底部安全边距 (像素) */
#define SAFE_MARGIN_BOTTOM          (4U)

/** @brief 单行总高度 (字体高度 + 行间距) */
#define LINE_HEIGHT                 (FONT_HEIGHT + LINE_SPACING)

/** @brief 屏幕最大安全可见行数 */
#define MAX_SAFE_VISIBLE_ITEMS      ((OLED_HEIGHT - SAFE_MARGIN_BOTTOM) / LINE_HEIGHT)

/** @brief 菜单文本显示起始点 X 坐标 (像素) */
#define START_POINT_OF_TEXT_DISPLAY (20U)

/** @brief UI 主循环帧间隔 (毫秒) */
#define SET_FRAME_INTERVAL          (33U)

/* ============================================================================ */
/*                              按键输入配置                                    */
/* ============================================================================ */

/* ---------------- 硬件引脚定义 ---------------- */
#define KEY1_PIN                    GPIO_Pin_0     
#define KEY2_PIN                    GPIO_Pin_11     
#define KEY3_PIN                    GPIO_Pin_5     /*!< 按键 3 连接至 PB12 */
#define KEY4_PIN                    GPIO_Pin_4     /*!< 按键 4 连接至 PB15 */

/* ---------------- 按键状态读取宏 ---------------- */
#define READ_KEY1_STATE()           GPIO_ReadInputDataBit(GPIOB, KEY1_PIN)
#define READ_KEY2_STATE()           GPIO_ReadInputDataBit(GPIOA, KEY2_PIN)
#define READ_KEY3_STATE()           GPIO_ReadInputDataBit(GPIOA, KEY3_PIN)
#define READ_KEY4_STATE()           GPIO_ReadInputDataBit(GPIOA, KEY4_PIN)

/* ---------------- 时间参数（基于 20ms/tick 的定时器中断） ---------------- */
#define DEBOUNCE_TICKS              (1U)    /*!< 消抖时间：20 ms */
#define LONG_PRESS_TICKS            (25U)   /*!< 长按判定：500 ms */
#define DOUBLE_CLICK_TICKS          (10U)   /*!< 双击最大间隔：200 ms */
#define TRIPLE_CLICK_TICKS          (10U)   /*!< 三击最大间隔：200 ms */

/* 渐进式长按阶段配置 */
#define PHASE1_END_TICKS            (100U)  /*!< 阶段 1 结束：2000 ms */
#define PHASE2_END_TICKS            (200U)  /*!< 阶段 2 结束：4000 ms */
#define INTERVAL_PHASE1             (6U)    /*!< 阶段 1 触发间隔：120 ms */
#define INTERVAL_PHASE2             (3U)    /*!< 阶段 2 触发间隔：60 ms */
#define INTERVAL_PHASE3             (1U)    /*!< 阶段 3 触发间隔：20 ms */

#define MAX_KEYS_NUM                (4U)    /*!< 支持的最大按键数量 */

/* ============================================================================ */
/*                            旋转编码器输入配置                                */
/* ============================================================================ */

/* ---------------- 硬件引脚定义 ---------------- */
#define ENCODER_GPIO_PORT           GPIOB                   /*!< 编码器端口 */
#define ENCODER_GPIO_CLK            RCC_APB2Periph_GPIOB    /*!< 对应的 GPIO 时钟 */
#define ENCODER_PIN_A               GPIO_Pin_0              /*!< A 相连接的引脚 */
#define ENCODER_PIN_B               GPIO_Pin_1              /*!< B 相连接的引脚 */
#define ENCODER_PIN_SRC_A           GPIO_PinSource0         /*!< A 相 EXTI 映射源 */
#define ENCODER_PIN_SRC_B           GPIO_PinSource1         /*!< B 相 EXTI 映射源 */
#define ENCODER_EXTI_PORTSOURCE_A   GPIO_PortSourceGPIOB    /*!< A 相端口 EXTI 映射源 */
#define ENCODER_EXTI_PORTSOURCE_B   GPIO_PortSourceGPIOB    /*!< B 相端口 EXTI 映射源 */
#define ENCODER_IRQ_A               EXTI0_IRQn              /*!< A 相中断通道 */
#define ENCODER_IRQ_B               EXTI1_IRQn              /*!< B 相中断通道 */

/* ---------------- 衍生宏定义 ---------------- */
#define ENCODER_EXTI_LINE           (ENCODER_PIN_A | ENCODER_PIN_B)
#define ENCODER_A_EXTI_LINE         ((uint32_t)(ENCODER_PIN_A))
#define ENCODER_B_EXTI_LINE         ((uint32_t)(ENCODER_PIN_B))

/* ============================================================================ */
/*                            中断优先级配置                                    */
/* ============================================================================ */

#define SUB_PRIO_UNUSED             (0U)
#define ENCODER_EXTIA_PRIO          (5U)    /*!< 编码器 A 相中断优先级 */
#define ENCODER_EXTIB_PRIO          (6U)    /*!< 编码器 B 相中断优先级 */
#define GENERAL_TASK_HANDLER_PRIO   (7U)    /*!< 通用任务调度定时器优先级 */
#define TIMER2_PRIO                 GENERAL_TASK_HANDLER_PRIO

/* ============================================================================ */
/*                              功能开关配置                                    */
/* ============================================================================ */

//#define USE_ENCODER_INPUT           /*!< 启用编码器输入（注释掉可禁用） */

/* ============================================================================ */
/*                              全局状态变量                                    */
/* ============================================================================ */

static MyMenuPage* g_current_page = NULL;

/* ============================================================================ */
/*                         按键系统类型与数据结构                               */
/* ============================================================================ */

typedef enum {
    KEY_STATE_IDLE          = 0,    /*!< 空闲状态 */
    KEY_STATE_DEBOUNCE      = 1,    /*!< 消抖检测 */
    KEY_STATE_PRESSED       = 2,    /*!< 持续按压 */
    KEY_STATE_WAIT_DOUBLE   = 3,    /*!< 等待双击 */
    KEY_STATE_WAIT_TRIPLE   = 4     /*!< 等待三击 */
} KeyState;

typedef enum {
    KEY_EVENT_NONE          = 0,    /*!< 无事件 */
    KEY_EVENT_CLICK         = 1,    /*!< 单击 */
    KEY_EVENT_DOUBLE_CLICK  = 2,    /*!< 双击 */
    KEY_EVENT_TRIPLE_CLICK  = 3,    /*!< 三击 */
    KEY_EVENT_LONG_PRESS    = 4     /*!< 长按 */
} KeyEventType;

typedef struct {
    KeyState current_state;     /*!< 当前状态 */
    uint16_t press_counter;     /*!< 按压计时（单位：20ms） */
    uint16_t release_counter;   /*!< 释放计时（单位：20ms） */
    KeyEventType event;         /*!< 触发事件类型 */
    uint8_t multi_click_step;   /*!< 连击计数（0-2 对应单击/双击/三击） */
} KeyStateMachine;

static KeyStateMachine key_states[MAX_KEYS_NUM] = {
    {KEY_STATE_IDLE, 0, 0, KEY_EVENT_NONE, 0},
    {KEY_STATE_IDLE, 0, 0, KEY_EVENT_NONE, 0},
    {KEY_STATE_IDLE, 0, 0, KEY_EVENT_NONE, 0},
    {KEY_STATE_IDLE, 0, 0, KEY_EVENT_NONE, 0}
};

/* ============================================================================ */
/*                         按键底层驱动函数                                     */
/* ============================================================================ */

/**
 * @brief 初始化按键 GPIO
 */
static void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = KEY2_PIN | KEY3_PIN | KEY4_PIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY1_PIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief 获取指定按键的物理状态
 * @param key_id 按键编号（1~4）
 * @return uint8_t 1=按下，0=释放
 */
static uint8_t GetKeyState(uint8_t key_id) 
{
    uint8_t key_state = 0;

    switch (key_id) {
        case 1: key_state = READ_KEY1_STATE(); break;
        case 2: key_state = READ_KEY2_STATE(); break;
        case 3: key_state = READ_KEY3_STATE(); break;
        case 4: key_state = READ_KEY4_STATE(); break;
        default: key_state = 1; break;
    }

    return !key_state; 
}

/**
 * @brief 按键扫描状态机（定时器中断中调用，每 20ms）
 */
void Key_Scan(void)
{
    static uint8_t last_key_state[MAX_KEYS_NUM] = {0};

    for (uint8_t i = 0; i < MAX_KEYS_NUM; i++) {
        uint8_t current_state = GetKeyState(i + 1);
        
        switch (key_states[i].current_state) {
            /* 空闲状态：等待按键按下 */
            case KEY_STATE_IDLE:
                if (current_state && !last_key_state[i]) {
                    key_states[i].current_state = KEY_STATE_DEBOUNCE;
                    key_states[i].press_counter = 0;
                }
                break;

            /* 消抖状态：确认有效按下 */
            case KEY_STATE_DEBOUNCE:
                key_states[i].press_counter++;
                if (key_states[i].press_counter >= DEBOUNCE_TICKS) {
                    if (current_state) {
                        key_states[i].current_state = KEY_STATE_PRESSED;
                        key_states[i].press_counter = 0;
                    } else {
                        key_states[i].current_state = KEY_STATE_IDLE;
                    }
                }
                break;

            /* 按压保持状态：处理长按与释放 */
            case KEY_STATE_PRESSED:
                if (!current_state) {
                    /* 情况 1：按键释放 */
                    if (key_states[i].press_counter < LONG_PRESS_TICKS) {
                        key_states[i].multi_click_step++;
                        
                        if (key_states[i].multi_click_step >= 3) {
                            key_states[i].event = KEY_EVENT_TRIPLE_CLICK;
                            key_states[i].current_state = KEY_STATE_IDLE;
                            key_states[i].multi_click_step = 0;
                        } else {
                            key_states[i].current_state = (key_states[i].multi_click_step == 1)
                                ? KEY_STATE_WAIT_DOUBLE
                                : KEY_STATE_WAIT_TRIPLE;
                            key_states[i].release_counter = 0;
                        }
                    } else {
                        key_states[i].current_state = KEY_STATE_IDLE;
                        key_states[i].multi_click_step = 0;
                    }
                    key_states[i].press_counter = 0;
                } else {
                    /* 情况 2：持续按压（处理长按事件） */
                    key_states[i].press_counter++;
                    if (key_states[i].press_counter >= LONG_PRESS_TICKS) {
                        uint32_t current_time = key_states[i].press_counter;
                        uint32_t interval = 0;
                        uint32_t phase_start = 0;

                        /* 渐进式间隔策略 */
                        if (current_time < PHASE1_END_TICKS) {
                            phase_start = LONG_PRESS_TICKS;
                            interval = INTERVAL_PHASE1;
                        } else if (current_time < PHASE2_END_TICKS) {
                            phase_start = PHASE1_END_TICKS;
                            interval = INTERVAL_PHASE2;
                        } else {
                            phase_start = PHASE2_END_TICKS;
                            interval = INTERVAL_PHASE3;
                        }

                        uint32_t time_in_phase = current_time - phase_start;
                        if (time_in_phase == 0 || (time_in_phase % interval == 0)) {
                            key_states[i].event = KEY_EVENT_LONG_PRESS;
                        }
                    }
                }
                break;

            /* 等待双击状态：检测第二次按下 */
            case KEY_STATE_WAIT_DOUBLE:
                key_states[i].release_counter++;
                if (key_states[i].release_counter >= DOUBLE_CLICK_TICKS) {
                    key_states[i].event = KEY_EVENT_CLICK;
                    key_states[i].current_state = KEY_STATE_IDLE;
                    key_states[i].multi_click_step = 0;
                } else if (current_state && !last_key_state[i]) {
                    key_states[i].current_state = KEY_STATE_DEBOUNCE;
                    key_states[i].press_counter = 0;
                }
                break;

            /* 等待三击状态：检测第三次按下 */
            case KEY_STATE_WAIT_TRIPLE:
                key_states[i].release_counter++;
                if (key_states[i].release_counter >= TRIPLE_CLICK_TICKS) {
                    key_states[i].event = KEY_EVENT_DOUBLE_CLICK;
                    key_states[i].current_state = KEY_STATE_IDLE;
                    key_states[i].multi_click_step = 0;
                } else if (current_state && !last_key_state[i]) {
                    key_states[i].current_state = KEY_STATE_DEBOUNCE;
                    key_states[i].press_counter = 0;
                }
                break;

            default:
                break;
        }
        
        last_key_state[i] = current_state;
    }
}

/**
 * @brief 获取按键事件并清除标志
 * @param Key_Event 事件数组输出
 * @param Event_Num 事件数量
 */
static void Key_GetEvent(KeyEventType* Key_Event, uint8_t Event_Num)
{
    if (Key_Event == NULL || Event_Num > MAX_KEYS_NUM) {
        return;
    }

    for (uint8_t i = 0; i < Event_Num; i++) {
        Key_Event[i] = key_states[i].event;
        key_states[i].event = KEY_EVENT_NONE;
    }
}

/* ============================================================================ */
/*                         旋转编码器驱动                                       */
/* ============================================================================ */

static volatile int16_t Encoder_Count = 0;

/**
 * @brief 初始化旋转编码器 GPIO 和 EXTI
 */
static void Encoder_Init(void)
{
    RCC_APB2PeriphClockCmd(ENCODER_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = ENCODER_PIN_A | ENCODER_PIN_B;
    GPIO_Init(ENCODER_GPIO_PORT, &GPIO_InitStructure);

    GPIO_EXTILineConfig(ENCODER_EXTI_PORTSOURCE_A, ENCODER_PIN_SRC_A);
    GPIO_EXTILineConfig(ENCODER_EXTI_PORTSOURCE_B, ENCODER_PIN_SRC_B);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = ENCODER_EXTI_LINE;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = ENCODER_IRQ_A;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ENCODER_EXTIA_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SUB_PRIO_UNUSED;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = ENCODER_IRQ_B;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ENCODER_EXTIB_PRIO;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief 获取编码器增量值并清零
 * @return int16_t 计数变化（+1/-1）
 */
static int16_t Encoder_Get(void)
{
    int16_t temp = Encoder_Count;
    Encoder_Count = 0;
    return temp;
}

/* 中断服务函数（根据实际引脚选择启用） */
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(ENCODER_A_EXTI_LINE) != RESET) {
        if (GPIO_ReadInputDataBit(ENCODER_GPIO_PORT, ENCODER_PIN_B) == Bit_RESET) {
            Encoder_Count--;
        }
        EXTI_ClearITPendingBit(ENCODER_A_EXTI_LINE);
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(ENCODER_B_EXTI_LINE) != RESET) {
        if (GPIO_ReadInputDataBit(ENCODER_GPIO_PORT, ENCODER_PIN_A) == Bit_RESET) {
            Encoder_Count++;
        }
        EXTI_ClearITPendingBit(ENCODER_B_EXTI_LINE);
    }
}

/* ============================================================================ */
/*                         系统时基与定时器                                     */
/* ============================================================================ */

static volatile uint32_t s_tick = 0;

void SysTick_Handler(void)
{
    s_tick++;
}

/**
 * @brief 初始化 SysTick 为 1ms 周期
 */
static void SysTick_Init(void)
{
    if (SysTick_Config(SystemCoreClock / 1000)) {
        while (1);
    }
}

/**
 * @brief 获取系统运行时间（ms）
 * @return uint32_t 自启动以来的毫秒数
 */
static uint32_t SysTick_Get(void)
{
    return s_tick;
}

/**
 * @brief 初始化 TIM2 产生 1ms 定时中断
 */
static void Timer2_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_InternalClockConfig(TIM2);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIMER2_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SUB_PRIO_UNUSED;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM2, ENABLE);
}

/* ============================================================================ */
/*                         静态函数声明                                         */
/* ============================================================================ */

/* 安全检查 */
static bool IsCurrentPageValid(void);
static void HandleInvalidPage(void);

/* 工具函数 */
static MyMenuID GetMaxVisibleItems(void);
static int16_t CalcStringWidth(int16_t ChineseFont, int16_t ASCIIFont, const char *format, ...);

/* 滚动管理 */
static void InitScrollState(MyMenuItem* item);
static void UpdateVerticalScrollPosition(bool move_down);
static void UpdateHorizontalTextScroll(MyMenuPage* page);
static void SyncSlotWithActiveId(void);

/* 显示处理 */
static void DisplayScrollingText(uint8_t x, uint8_t y, const MyMenuItem* item);
static void DisplayMenuItem(uint8_t x, uint8_t y, MyMenuItem* item, bool is_active);

/* 按键处理 */
static void MyOLED_UI_Enter(void);
static void MyOLED_UI_Back(void);
static void MyOLED_UI_MoveUp(void);
static void MyOLED_UI_MoveDown(void);

/* ============================================================================ */
/*                         安全检查函数实现                                     */
/* ============================================================================ */

/**
 * @brief 检查当前页面是否有效
 * @return bool true=有效，false=无效
 */
static bool IsCurrentPageValid(void) 
{
    if (g_current_page == NULL || 
        g_current_page->items == NULL || 
        g_current_page->ItemNum == 0) {
        return false;
    }
    return true;
}

/**
 * @brief 处理无效页面状态
 */
static void HandleInvalidPage(void) 
{
    OLED_Clear();
    if (g_current_page == NULL || g_current_page->items == NULL) {
        OLED_ShowString(0, 0, "NO PAGE", OLED_8X16_HALF);
    } else {
        OLED_ShowString(0, 0, "EMPTY", OLED_8X16_HALF);
    }
    OLED_Update();
}

/* ============================================================================ */
/*                         工具函数实现                                         */
/* ============================================================================ */

/**
 * @brief 获取最大可见菜单项数量
 * @return MyMenuID 最大可见项数
 */
static MyMenuID GetMaxVisibleItems(void) 
{
    return MAX_SAFE_VISIBLE_ITEMS;
}

/**
 * @brief 计算字符串显示宽度
 * @param ChineseFont 中文字体宽度（像素）
 * @param ASCIIFont ASCII 字体宽度（像素）
 * @param format 格式化字符串
 * @param ... 可变参数列表
 * @return int16_t 字符串总宽度（像素）
 */
static int16_t CalcStringWidth(int16_t ChineseFont, int16_t ASCIIFont, const char *format, ...) 
{
    int16_t StringLength = 0;
    char String[MAX_STRING_LENGTH];

    va_list args;
    va_start(args, format);
    vsnprintf(String, sizeof(String), format, args);
    va_end(args);

    char *ptr = String;
    if (OLED_CHN_CHAR_WIDTH == 2) {
        while (*ptr != '\0') {
            if ((unsigned char)*ptr & 0x80) {
                StringLength += ChineseFont;
                ptr += 2;
            } else {
                StringLength += ASCIIFont;
                ptr++;
            }
        }
    } else if (OLED_CHN_CHAR_WIDTH == 3) {
        while (*ptr != '\0') {
            uint8_t c = (uint8_t)*ptr;
            if ((c & 0xE0) == 0xE0) {
                StringLength += ChineseFont;
                ptr += 3;
            } else if ((c & 0xC0) == 0xC0) {
                ptr += 2;
            } else {
                StringLength += ASCIIFont;
                ptr++;
            }
        }   
    }
    return StringLength;
}

/* ============================================================================ */
/*                         滚动状态管理函数                                     */
/* ============================================================================ */

/**
 * @brief 初始化菜单项滚动状态
 * @param item 指向要初始化的菜单项
 */
static void InitScrollState(MyMenuItem* item) 
{
    if (item == NULL) return;
    
    if (item->int16_Value != NULL) {
        item->text_width = CalcStringWidth(OLED_16X16_FULL, OLED_8X16_HALF, item->text, *item->int16_Value);
    }
    if (item->float_Value != NULL) {
        item->text_width = CalcStringWidth(OLED_16X16_FULL, OLED_8X16_HALF, item->text, *item->float_Value);
    } else {
        item->text_width = CalcStringWidth(OLED_16X16_FULL, OLED_8X16_HALF, item->text);
    }

    int16_t available_width = OLED_WIDTH - START_POINT_OF_TEXT_DISPLAY;
    
    if (item->text_width > available_width) {
        item->is_scrolling = true;
        item->scroll_offset = 0;
    } else {
        item->is_scrolling = false;
        item->scroll_offset = 0;
    }
}

/**
 * @brief 更新垂直滚动位置
 * @param move_down true=向下，false=向上
 */
static void UpdateVerticalScrollPosition(bool move_down) 
{
    if (!IsCurrentPageValid()) return;
    
    MyMenuID max_visible = GetMaxVisibleItems();
    MyMenuID max_slot = max_visible - 1;
    
    if (move_down) {
        if (g_current_page->slot < max_slot) {
            g_current_page->slot++;
        } else {
            MyMenuID max_visible_start = (g_current_page->ItemNum > max_visible) ? 
                                       (g_current_page->ItemNum - max_visible) : 0;
            if (g_current_page->visible_start < max_visible_start) {
                g_current_page->visible_start++;
            }
        }
    } else {
        if (g_current_page->slot > 0) {
            g_current_page->slot--;
        } else {
            if (g_current_page->visible_start > 0) {
                g_current_page->visible_start--;
            }
        }
    }
}

/**
 * @brief 更新水平文本滚动动画
 * @param page 指向当前页面
 */
static void UpdateHorizontalTextScroll(MyMenuPage* page) 
{
    if (page == NULL) return;
    
    uint32_t current_time = SysTick_Get();
    
    if (page->last_scroll_time == 0) {
        page->last_scroll_time = current_time;
    }
    
    if ((current_time - page->last_scroll_time) >= page->scroll_delay) {
        page->last_scroll_time = current_time;
        
        MyMenuID start_idx = page->visible_start;
        MyMenuID end_idx = start_idx + GetMaxVisibleItems();
        if (end_idx > page->ItemNum) {
            end_idx = page->ItemNum;
        }
        
        for (MyMenuID i = start_idx; i < end_idx; i++) {
            MyMenuItem* item = &page->items[i];
            if (item->is_scrolling) {
                item->scroll_offset--;
                if (item->scroll_offset < -item->text_width) {
                    item->scroll_offset = OLED_WIDTH - START_POINT_OF_TEXT_DISPLAY;
                }
            }
        }
    }
}

/**
 * @brief 同步槽位与活动 ID
 */
static void SyncSlotWithActiveId(void) 
{
    if (!IsCurrentPageValid()) return;
    
    if (g_current_page->active_id >= g_current_page->ItemNum) {
        g_current_page->active_id = g_current_page->ItemNum - 1;
    }
    
    MyMenuID offset = g_current_page->active_id - g_current_page->visible_start;
    MyMenuID actual_max_visible = GetMaxVisibleItems();
    
    if (offset < actual_max_visible) {
        g_current_page->slot = offset;
    } else {
        g_current_page->visible_start = g_current_page->active_id - (actual_max_visible - 1);
        g_current_page->slot = actual_max_visible - 1;
    }
    
    if (g_current_page->slot >= actual_max_visible) {
        g_current_page->slot = actual_max_visible - 1;
    }
}

/* ============================================================================ */
/*                         显示处理函数                                         */
/* ============================================================================ */

/**
 * @brief 显示滚动的文本
 * @param x X 坐标
 * @param y Y 坐标
 * @param item 菜单项指针
 */
static void DisplayScrollingText(uint8_t x, uint8_t y, const MyMenuItem* item) 
{
    if (item == NULL || item->text_width == 0) return;
    
    int16_t display_x = x + item->scroll_offset;
    int16_t text_right_edge = display_x + item->text_width;
    
    if (display_x < OLED_WIDTH && text_right_edge > 0) {
        if (item->int16_Value != NULL) {
            OLED_PrintfMixArea(display_x, y, item->text_width, FONT_HEIGHT, 
                              display_x, y, OLED_16X16_FULL, OLED_8X16_HALF, 
                              item->text, *item->int16_Value);
        } else if (item->float_Value != NULL) {
            OLED_PrintfMixArea(display_x, y, item->text_width, FONT_HEIGHT, 
                              display_x, y, OLED_16X16_FULL, OLED_8X16_HALF, 
                              item->text, *item->float_Value);
        } else {
            OLED_ShowMixStringArea(display_x, y, item->text_width, FONT_HEIGHT, 
                                  display_x, y, item->text, 
                                  OLED_16X16_FULL, OLED_8X16_HALF);
        }
    }
}

/**
 * @brief 显示单个菜单项
 * @param x X 坐标
 * @param y Y 坐标
 * @param item 菜单项指针
 * @param is_active 是否为活动项
 */
static void DisplayMenuItem(uint8_t x, uint8_t y, MyMenuItem* item, bool is_active) 
{
    char prefix[4] = {0};
    bool is_dynamic_content = false;
    uint8_t text_x = x + START_POINT_OF_TEXT_DISPLAY;

    if (item->int16_Value != NULL || item->float_Value != NULL) {
        is_dynamic_content = true;
    }

    if (is_dynamic_content) {
        int16_t new_width;
        if (item->int16_Value != NULL) {
            new_width = CalcStringWidth(OLED_16X16_FULL, OLED_8X16_HALF, item->text, *item->int16_Value);
        } else {
            new_width = CalcStringWidth(OLED_16X16_FULL, OLED_8X16_HALF, item->text, *item->float_Value);
        }

        if (abs(new_width - item->text_width) >= 8) {
            item->text_width = new_width;
            bool should_scroll = (item->text_width > (OLED_WIDTH - START_POINT_OF_TEXT_DISPLAY));
            if (should_scroll != item->is_scrolling) {
                item->is_scrolling = should_scroll;
                if (!should_scroll) {
                    item->scroll_offset = 0;
                }
            }
        }
    }

    if (item->is_scrolling) {
        DisplayScrollingText(text_x, y, item);
    } else {
        if (item->int16_Value != NULL) {
            OLED_PrintfMix(text_x, y, OLED_16X16_FULL, OLED_8X16_HALF, item->text, *item->int16_Value);
        } else if (item->float_Value != NULL) {
            OLED_PrintfMix(text_x, y, OLED_16X16_FULL, OLED_8X16_HALF, item->text, *item->float_Value);
        } else {
            OLED_ShowMixString(text_x, y, item->text, OLED_16X16_FULL, OLED_8X16_HALF);
        }
    }

    if (is_active) {
        prefix[0] = item->is_editing ? '=' : '>';
    } else {
        prefix[0] = ' ';
    }

    switch (item->item_type) {
        case MENU_ITEM_BACK:
            prefix[1] = '~';
            break;
        default:
            if (item->submenu != NULL) {
                prefix[1] = '>';
            } else if (item->callback != NULL) {
                prefix[1] = '+';
            } else if (item->edit_config != NULL) {
                prefix[1] = '=';
            } else {
                prefix[1] = '-';
            }
            break;
    }

    OLED_ShowString(x, y, prefix, OLED_8X16_HALF);
}

/* ============================================================================ */
/*                         按键事件处理函数                                     */
/* ============================================================================ */

/**
 * @brief 处理 Enter 按键事件
 */
static void MyOLED_UI_Enter(void) 
{
    if (!IsCurrentPageValid()) return;
    
    MyMenuItem* item = &g_current_page->items[g_current_page->active_id];

    if (item->item_type == MENU_ITEM_BACK) {
        if (g_current_page->parent != NULL) {
            g_current_page = g_current_page->parent;
        }
        return;
    }

    if (item->submenu != NULL) {
        g_current_page = item->submenu;
        g_current_page->active_id = 0;
        g_current_page->visible_start = 0;
        g_current_page->slot = 0;
        return;
    }

    if (item->edit_config != NULL) {
        item->is_editing = !item->is_editing;
        return;
    }

    if (item->callback != NULL) {
        item->callback();
    }
}

/**
 * @brief 处理 Back 按键事件
 */
static void MyOLED_UI_Back(void) 
{
    if (!IsCurrentPageValid()) return;
    
    MyMenuItem* item = &g_current_page->items[g_current_page->active_id];
    if (item->is_editing) {
        item->is_editing = false;
        return;
    }

    if (g_current_page->parent != NULL) {
        g_current_page = g_current_page->parent;
    }
}

/**
 * @brief 处理向上导航
 */
static void MyOLED_UI_MoveUp(void) 
{
    if (!IsCurrentPageValid()) return;
    if (g_current_page->active_id == 0) return;
    g_current_page->active_id--;
    UpdateVerticalScrollPosition(false);
}

/**
 * @brief 处理向下导航
 */
static void MyOLED_UI_MoveDown(void) 
{
    if (!IsCurrentPageValid()) return;
    if (g_current_page->active_id >= g_current_page->ItemNum - 1) return;
    g_current_page->active_id++;
    UpdateVerticalScrollPosition(true);
}

/* ============================================================================ */
/*                         对外接口函数实现                                     */
/* ============================================================================ */

/**
 * @brief 初始化 OLED 菜单系统
 * @param page 初始页面指针
 */
void MyOLED_UI_Init(MyMenuPage* page) 
{
    SysTick_Init();
    OLED_Init();
    Timer2_Init();
    Key_Init();
    
#ifdef USE_ENCODER_INPUT
    Encoder_Init();
#endif
    
    if (page == NULL) {
        OLED_Clear();
        OLED_ShowString(0, 0, "PAGE NULL!", OLED_8X16_HALF);
        OLED_Update();
        while (1);
    }
    
    g_current_page = page;
}

/**
 * @brief 主循环函数
 * @note 应在 main 函数中循环调用
 */
void MyOLED_UI_MainLoop(void) 
{
    static KeyEventType s_key_events[MAX_KEYS_NUM] = {KEY_EVENT_NONE};
    Key_GetEvent(s_key_events, MAX_KEYS_NUM);

#ifdef USE_ENCODER_INPUT
    int16_t enc = Encoder_Get();
#endif

    /* 处理按键事件 */
    if (s_key_events[0] == KEY_EVENT_CLICK) {
        MyOLED_UI_Enter();
    }
    if (s_key_events[1] == KEY_EVENT_CLICK) {
        MyOLED_UI_Back();
    }
    if (s_key_events[2] == KEY_EVENT_CLICK || s_key_events[2] == KEY_EVENT_LONG_PRESS) {
        MyOLED_UI_MoveUp();
    }
    if (s_key_events[3] == KEY_EVENT_CLICK || s_key_events[3] == KEY_EVENT_LONG_PRESS) {
        MyOLED_UI_MoveDown();
    }

#ifdef USE_ENCODER_INPUT
    /* 处理编码器输入 */
    if (enc != 0) {
        if (IsCurrentPageValid()) {
            MyMenuItem* active_item = &g_current_page->items[g_current_page->active_id];
            
            if (active_item->is_editing && active_item->edit_config && 
                (active_item->int16_Value || active_item->float_Value)) {
                
                uint32_t now = SysTick_Get();
                
                if (g_current_page->last_encoder_time == 0) {
                    g_current_page->last_encoder_time = now;
                    g_current_page->encoder_accel = 1;
                }
                
                uint32_t dt = now - g_current_page->last_encoder_time;
                
                if (dt < 100 && dt > 0) {
                    g_current_page->encoder_accel++;
                    if (g_current_page->encoder_accel > 4) {
                        g_current_page->encoder_accel = 4;
                    }
                } else {
                    g_current_page->encoder_accel = 1;
                }
                g_current_page->last_encoder_time = now;

                int32_t base_step = active_item->edit_config->step;
                if (base_step <= 0) base_step = 1;

                const uint8_t accel_table[5] = {1, 1, 5, 10, 20};
                const uint8_t accel_table2[5] = {1, 1, 30, 40, 50};
                int32_t actual_step = base_step * accel_table2[g_current_page->encoder_accel];

                if (active_item->int16_Value) {
                    int32_t current_val = (int32_t)(*active_item->int16_Value);
                    int32_t delta = (enc >= 1) ? actual_step : -actual_step;
                    int32_t new_val = current_val + delta;
                    
                    if (new_val < active_item->edit_config->min) {
                        new_val = active_item->edit_config->min;
                    } else if (new_val > active_item->edit_config->max) {
                        new_val = active_item->edit_config->max;
                    }
                    
                    *active_item->int16_Value = (uint16_t)new_val;
                } else if (active_item->float_Value) {
                    float base_step_f = (float)base_step * 0.001f;
                    float actual_step_f = base_step_f * (float)accel_table[g_current_page->encoder_accel];
                    
                    float current_val = *active_item->float_Value;
                    float delta = (enc >= 1) ? actual_step_f : -actual_step_f;
                    float new_val = current_val + delta;

                    float f_min = (float)active_item->edit_config->min;
                    float f_max = (float)active_item->edit_config->max;

                    if (new_val < f_min) {
                        new_val = f_min;
                    } else if (new_val > f_max) {
                        new_val = f_max;
                    }

                    *active_item->float_Value = new_val;
                }
            } else {
                if (enc == 1) {
                    MyOLED_UI_MoveDown();
                } else if (enc == -1) {
                    MyOLED_UI_MoveUp();
                }
                g_current_page->encoder_accel = 1;
                g_current_page->last_encoder_time = 0;
            }
        }
    }
#endif

    /* 安全检查 */
    if (!IsCurrentPageValid()) {
        HandleInvalidPage();
        return;
    }

    SyncSlotWithActiveId();
    UpdateHorizontalTextScroll(g_current_page);

    MyMenuID actual_displayable = GetMaxVisibleItems();
    MyMenuID max_visible_start = (g_current_page->ItemNum > actual_displayable) ? 
                                 (g_current_page->ItemNum - actual_displayable) : 0;
    if (g_current_page->visible_start > max_visible_start) {
        g_current_page->visible_start = max_visible_start;
    }

    /* 非阻塞帧率控制 */
    static uint32_t last_render_time = 0;
    uint32_t current_time = SysTick_Get();
    
    if ((current_time - last_render_time) < SET_FRAME_INTERVAL) {
        return;
    }
    last_render_time = current_time;

    OLED_Clear();

    MyMenuID start_idx = g_current_page->visible_start;
    MyMenuID end_idx = start_idx + actual_displayable;
    if (end_idx > g_current_page->ItemNum) {
        end_idx = g_current_page->ItemNum;
    }

    MyMenuID display_index = 0;
    for (MyMenuID i = start_idx; i < end_idx; i++) {
        uint8_t line_y = display_index * LINE_HEIGHT;
        
        if (line_y > (OLED_HEIGHT - LINE_HEIGHT)) {
            break;
        }
        
        display_index++;
        
        MyMenuItem* item = &g_current_page->items[i];
        if (item->text == NULL) continue;

        if (item->text_width == 0) {
            InitScrollState(item);
        }

        bool is_active = (i == g_current_page->active_id);
        DisplayMenuItem(0, line_y, item, is_active);
    }

    OLED_Update();
}
