// system_config.h
#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H
#include "stm32f10x.h"                  // Device header
#include "stdbool.h" 

/*============================================================================
 *                          OLED MENU 配置
 *============================================================================*/
/** @defgroup OLED_MENU_Config */
/** @{ */
/** @brief OLED屏幕宽度(像素) */
#define OLED_WIDTH     128

/** @brief OLED屏幕高度(像素) */
#define OLED_HEIGHT    64

/** @brief 字体宽度(像素) */
#define FONT_WIDTH            16

/** @brief 字体高度(像素) */
#define FONT_HEIGHT           16

/** @brief 行间距(像素) */
#define LINE_SPACING          4

/** @brief 底部安全边距(像素) */
#define SAFE_MARGIN_BOTTOM    4

/** @brief 单行总高度(字体高度+行间距) */
#define LINE_HEIGHT           (FONT_HEIGHT + LINE_SPACING)

/** @brief 屏幕最大安全可见行数 */
#define MAX_SAFE_VISIBLE_ITEMS ((OLED_HEIGHT - SAFE_MARGIN_BOTTOM) / LINE_HEIGHT)

/** @brief 菜单文本显示起始点X坐标(像素) */
#define START_POINT_OF_TEXT_DISPLAY 20

/** @brief 文本滚动动画更新间隔(毫秒) */
#define SET_SCROLL_DELAY 16  

/** @brief UI主循环帧间隔(毫秒) */
#define SET_FRAME_INTERVAL 33

/*
 * 滚动速度配置参考方案：
 * 
 * 方案1：高精度滚动（推荐）
 * page->scroll_delay = 16;   // 60FPS滚动更新
 * frame_interval = 33;       // 30FPS显示刷新
 * 优点：滚动平滑，显示负载适中
 *
 * 方案2：同步更新（最稳定）
 * page->scroll_delay = 33;   // 与显示同步
 * frame_interval = 33;       // 完全同步
 * 优点：绝对稳定，但滚动颗粒度较大
 *
 * 方案3：节能模式
 * page->scroll_delay = 66;   // 降低滚动频率
 * frame_interval = 66;       // 降低整体刷新率
 * 优点：省电，适合电池供电设备
 */
 
/** @} */



#define alarm_off						false

extern const char* ESP8266_SNTP_CONFIG;

extern u16 Reset_Count;


#endif
