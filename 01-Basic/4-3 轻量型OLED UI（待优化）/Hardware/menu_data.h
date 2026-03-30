/**
 * @file menu_data.h
 * @brief OLED 菜单系统数据结构声明头文件
 *
 * 本文件定义并声明菜单系统所需的核心数据类型、结构体及全局变量。
 * 采用模块化设计，将数据结构与实现分离，便于扩展和跨文件引用。
 *
 * @author Eureka & Lingma
 * @date 2026-02-22
 */

#ifndef MENU_DATA_H
#define MENU_DATA_H

#include "stm32f10x.h"  // STM32F1 系列设备头文件
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 *                          宏定义
 *============================================================================*/

/** @brief 文本水平滚动动画的更新间隔（单位：毫秒） */
#define SET_SCROLL_DELAY    (16U)

/*============================================================================
 *                          类型定义
 *============================================================================*/

/** @brief 菜单 ID 类型（用于索引和计数） */
typedef uint16_t MyMenuID;

/*============================================================================
 *                          枚举定义
 *============================================================================*/

/**
 * @brief 菜单项功能类型
 */
typedef enum {
    MENU_ITEM_NORMAL = 0,   /**< 普通菜单项：可跳转子菜单或触发回调 */
    MENU_ITEM_BACK   = 1    /**< 返回项：返回上级菜单 */
} MenuItemType;

/*============================================================================
 *                          结构体定义
 *============================================================================*/

/**
 * @brief 可编辑数值项的配置参数
 *
 * 存储于 Flash，用于限制用户输入范围和步长。
 */
typedef struct {
    int16_t min;    /**< 允许的最小值 */
    int16_t max;    /**< 允许的最大值 */
    uint8_t step;   /**< 调节步长（必须为正整数） */
} MenuEditConfig;

/**
 * @brief 单个菜单项的数据结构
 *
 * 每个菜单项可具备以下功能之一：
 * - 跳转到子页面（submenu != NULL）
 * - 执行回调函数（callback != NULL）
 * - 动态显示/编辑关联变量（int16_Value != NULL）
 *
 * @note 字段初始化规则：
 *       - 未使用的指针应设为 NULL
 *       - 运行时状态字段（如 is_editing、scroll_offset 等）由菜单引擎维护
 */
typedef struct MyMenuItem {
    const char* text;               /**< 显示文本，支持 printf 风格格式（如 "Val:%d"） */
    void (*callback)(void);         /**< Enter 键确认时调用的函数（submenu 优先级更高） */
    struct MyMenuPage* submenu;     /**< 子菜单页面指针（非 NULL 时忽略 callback） */
    int16_t* int16_Value;           /**< 关联的 int16_t 变量地址（用于动态显示或编辑） */	
    MenuItemType item_type;         /**< 项类型（影响导航行为） */

    // 编辑相关
    MenuEditConfig* edit_config;    /**< 编辑配置（非 NULL 表示该项可编辑） */
    bool is_editing;                /**< 当前是否处于编辑模式（运行时状态） */

    // 水平滚动相关（由渲染逻辑自动维护）
    int16_t scroll_offset;          /**< 当前滚动偏移（像素） */
    int16_t text_width;             /**< 文本总宽度缓存（像素，首次渲染时计算） */
    bool is_scrolling;              /**< 是否需要滚动动画 */
    float* float_Value;                 /**< 关联的16位数值变量指针 */		
} MyMenuItem;

/**
 * @brief 菜单页面状态结构体
 *
 * 描述一个完整菜单页面的运行时状态与布局参数。
 */
typedef struct MyMenuPage {
    struct MyMenuPage* parent;      /**< 父页面指针（顶层页面为 NULL） */
    MyMenuItem* items;              /**< 指向菜单项数组（必须以 {0} 结尾） */
    MyMenuID active_id;             /**< 当前选中的菜单项索引 */
    MyMenuID visible_start;         /**< 可视区域起始项索引（用于垂直滚动） */
    MyMenuID max_visible;           /**< 屏幕最多可显示的项数（受 OLED 分辨率限制） */
    MyMenuID slot;                  /**< 光标在可视区中的槽位（0 ~ max_visible - 1） */
    MyMenuID ItemNum;               /**< 菜单项总数（不包含终止项） */

    // 滚动动画控制
    uint32_t last_scroll_time;      /**< 上次水平滚动更新的时间戳（ms） */
    uint16_t scroll_delay;          /**< 水平滚动间隔（ms），通常为 SET_SCROLL_DELAY */

    // 编码器加速控制（用于快速滚动）
    uint32_t last_encoder_time;     /**< 上次编码器操作时间戳（ms） */
    uint8_t  encoder_accel;         /**< 当前加速等级（1=慢速，值越大越快） */
} MyMenuPage;

/*============================================================================
 *                          全局变量声明
 *============================================================================*/

// 菜单项数组（定义在 menu_data.c）
extern MyMenuItem MainItems[];
extern MyMenuItem SettingsItems[];
extern MyMenuItem MonitorItems[];
extern MyMenuItem MoreItems[];

// 页面实例（定义在 menu_data.c）
extern MyMenuPage MainPage;
extern MyMenuPage SettingsPage;
extern MyMenuPage MonitorPage;
extern MyMenuPage MorePage;


#ifdef __cplusplus
}
#endif

#endif /* MENU_DATA_H */
