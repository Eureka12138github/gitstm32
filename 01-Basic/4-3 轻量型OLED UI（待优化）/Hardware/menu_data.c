/**
 * @file menu_data.c
 * @brief OLED 菜单系统数据定义文件
 *
 * 本文件集中定义所有菜单页面与菜单项的数据结构，包括：
 * - 主菜单、子菜单的层级关系
 * - 菜单项的显示文本、回调函数、子页面指针、关联变量等
 * - 可编辑数值项的配置参数
 *
 * 所有菜单数据在此统一管理，便于扩展与调试。
 *
 * @author Eureka & Lingma
 * @date 2026-02-22
 */
 
/*============================================================================
 *                          使用说明摘要
 *============================================================================*/
/*
 * 🔹 菜单项 (MyMenuItem) 字段说明：
 *   - text         : 显示字符串（支持 %d 等格式）
 *   - callback     : Enter 键触发的回调函数
 *   - submenu      : 指向子页面（用于跳转）
 *   - int16_Value  : 关联的 int16_t 变量指针（用于动态显示/编辑）
 *   - item_type    : 类型（MENU_ITEM_NORMAL / MENU_ITEM_BACK）
 *   - edit_config  : 编辑配置（非 NULL 表示可编辑）
 *   - is_editing   : 当前是否处于编辑状态
 *   - scroll_offset/text_width/is_scrolling: 滚动动画相关（由渲染逻辑维护）
 *   - float_Value  : 关联的 float 变量指针（用于动态显示/编辑）
 *
 * 🔹 页面 (MyMenuPage) 字段说明：
 *   - parent       : 父页面（用于返回）
 *   - items        : 菜单项数组
 *   - active_id    : 当前选中项索引
 *   - visible_start: 可视区域起始项
 *   - max_visible  : 屏幕最多显示项数（受 OLED 分辨率限制）
 *   - slot         : 光标在可视区中的位置（0 ~ max_visible-1）
 *   - ItemNum      : 自动计算的总项数（不包含终止项）
 *   - scroll_delay : 水平滚动动画间隔（ms）
 *
 * 🔹 配置规范：
 *   - 每个菜单项数组必须以 {0} 结尾
 *   - 返回项必须设置 .item_type = MENU_ITEM_BACK
 *   - 可编辑项需提供有效的 .int16_Value 和 .edit_config
 *   - 所有未显式初始化的字段默认为 0（安全）
 */ 
 

#include "menu_data.h"
#include <stddef.h>

/*============================================================================
 *                          全局可编辑变量声明
 *============================================================================*/

// 示例：用于菜单中动态显示和编辑的全局变量
int16_t g_value1 = 0;
float g_value2 = 0;


/*============================================================================
 *                      可编辑项配置
 *============================================================================*/

/**
 * @brief RP 参数通用编辑配置（只读常量，存于 Flash）
 */
static const MenuEditConfig s_edit_config1 = {
    .min  = -50,   ///< 最小值
    .max  = 50,    ///< 最大值
    .step = 1      ///< 调节步长
};

/*============================================================================
 *                          回调函数实现
 *============================================================================*/

/**
 * @brief 测试回调函数：切换 LED 状态
 * @note 实际使用时需取消注释 Led_Turn() 调用
 */
static void Test_Callback_1(void) {
    // Led_Turn();
}

/*============================================================================
 *                          菜单项数组定义
 *============================================================================*/

/**
 * @brief 主菜单项数组
 * 
 * 包含导航项、功能项、长文本项等类型。
 * - 使用完整初始化的第一项作为结构体参考模板
 * - 数组末尾必须以 {0} 终止
 */
MyMenuItem MainItems[] = {
    // 【完整初始化示例】—— 明确所有字段含义
    {
        .text         = "监控",
        .callback     = NULL,
        .submenu      = &MonitorPage,
        .int16_Value  = NULL,
        .item_type    = MENU_ITEM_NORMAL,
        .edit_config  = NULL,
        .is_editing   = false,
        .scroll_offset= 0,
        .text_width   = 0,
        .is_scrolling = false
    },
    {"设置", NULL, &SettingsPage, NULL, MENU_ITEM_NORMAL},
    {"更多", NULL, &MorePage,    NULL, MENU_ITEM_NORMAL},
    {"TEST1", Test_Callback_1, NULL, NULL, MENU_ITEM_NORMAL},
    {"TEST2:ABCDIHOIAJFJLSAJDKFJAKLJDLFAJLD", NULL, NULL, NULL, MENU_ITEM_NORMAL},
    {0}  ///< 数组终止标记
};

/**
 * @brief 设置子菜单项数组
 */
MyMenuItem SettingsItems[] = {
    {"Setting_1", NULL, NULL, NULL, MENU_ITEM_NORMAL},
    {"Setting_2", NULL, NULL, NULL, MENU_ITEM_NORMAL},
    {"Setting_3", NULL, NULL, NULL, MENU_ITEM_NORMAL},
    {"[返回]", NULL, NULL, NULL, MENU_ITEM_BACK},  ///< 返回项
    {0}
};

/**
 * @brief 监控子菜单项数组（支持数值显示与编辑）
 */
MyMenuItem MonitorItems[] = {
    {
        .text        = "Value1:%d",
        .callback    = NULL,
        .submenu     = NULL,
        .int16_Value = &g_value1,
        .float_Value = NULL,			
        .item_type   = MENU_ITEM_NORMAL,
        .edit_config = (MenuEditConfig*)&s_edit_config1,
        .is_editing  = false,
        .scroll_offset = 0,
        .text_width  = 0,
        .is_scrolling = false
    },

    {
        .text        = "Value2:%.2f",
        .callback    = NULL,
        .submenu     = NULL,
        .int16_Value = NULL,
        .float_Value = &g_value2,				
        .item_type   = MENU_ITEM_NORMAL,
        .edit_config = (MenuEditConfig*)&s_edit_config1,
        .is_editing  = false,
        .scroll_offset = 0,
        .text_width  = 0,
        .is_scrolling = false
    },

    {"[返回]", NULL, NULL, NULL, MENU_ITEM_BACK},
    {0}
};

/**
 * @brief 更多子菜单项数组
 */
MyMenuItem MoreItems[] = {
    {"About", NULL, NULL, NULL, MENU_ITEM_NORMAL},
    {"Help",  NULL, NULL, NULL, MENU_ITEM_NORMAL},
    {"[返回]", NULL, NULL, NULL, MENU_ITEM_BACK},
    {0}
};

/*============================================================================
 *                          页面结构定义
 *============================================================================*/

/**
 * @brief 主页面配置（顶层页面）
 */
MyMenuPage MainPage = {
    .parent           = NULL,
    .items            = MainItems,
    .active_id        = 0,
    .visible_start    = 0,
    .max_visible      = 4,
    .slot             = 0,
    .ItemNum          = (sizeof(MainItems) / sizeof(MyMenuItem)) - 1,
    .last_scroll_time = 0,
    .scroll_delay     = SET_SCROLL_DELAY,
    .last_encoder_time= 0,
    .encoder_accel    = 1
};

/**
 * @brief 设置页面配置
 */
MyMenuPage SettingsPage = {
    .parent           = &MainPage,
    .items            = SettingsItems,
    .active_id        = 0,
    .visible_start    = 0,
    .max_visible      = 4,
    .slot             = 0,
    .ItemNum          = (sizeof(SettingsItems) / sizeof(MyMenuItem)) - 1,
    .last_scroll_time = 0,
    .scroll_delay     = SET_SCROLL_DELAY,
    .last_encoder_time= 0,
    .encoder_accel    = 1
};

/**
 * @brief 监控页面配置（支持数值编辑）
 */
MyMenuPage MonitorPage = {
    .parent           = &MainPage,
    .items            = MonitorItems,
    .active_id        = 0,
    .visible_start    = 0,
    .max_visible      = 4,
    .slot             = 0,
    .ItemNum          = (sizeof(MonitorItems) / sizeof(MyMenuItem)) - 1,
    .last_scroll_time = 0,
    .scroll_delay     = SET_SCROLL_DELAY,
    .last_encoder_time= 0,
    .encoder_accel    = 1
};

/**
 * @brief 更多页面配置
 */
MyMenuPage MorePage = {
    .parent           = &MainPage,
    .items            = MoreItems,
    .active_id        = 0,
    .visible_start    = 0,
    .max_visible      = 4,
    .slot             = 0,
    .ItemNum          = (sizeof(MoreItems) / sizeof(MyMenuItem)) - 1,
    .last_scroll_time = 0,
    .scroll_delay     = SET_SCROLL_DELAY,
    .last_encoder_time= 0,
    .encoder_accel    = 1
};
