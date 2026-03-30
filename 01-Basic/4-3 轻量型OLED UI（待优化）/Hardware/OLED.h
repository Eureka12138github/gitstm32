/**
 * @file OLED.h
 * @brief OLED显示驱动软件层头文件
 * @author 江协科技 & 上nm网课呢
 * @date 2026-02-22
 */

#ifndef __OLED_H
#define __OLED_H

#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f10x.h"                  // Device header
#include "stdbool.h"
	
	
// 字体大小定义
#define OLED_6X8_HALF       (6)     // ASCII字符 6x8
#define OLED_8X16_HALF      (8)     // ASCII字符 8x16
#define OLED_7X12_HALF      (7)     // ASCII字符 7x12

#define OLED_8X8_FULL       (8)     // 中文字符 8x8
#define OLED_12X12_FULL     (12)    // 中文字符 12x12
#define OLED_16X16_FULL     (16)    // 中文字符 16x16
#define OLED_20X20_FULL     (20)    // 中文字符 20x20

// 填充模式定义
#define OLED_UNFILLED       (0)
#define OLED_FILLED         (1)

// 字符串最大长度
#define MAX_STRING_LENGTH   (128)

	
/* ============================================================================ */
/*                            OLED 显示屏配置                                   */
/* ============================================================================ */
/** @defgroup OLED_Config OLED 引脚与通信参数 */
/** @{ */
#define OLED_SCL_PIN        GPIO_Pin_8          /*!< I2C 时钟线 SCL 引脚 (PB8) */
#define OLED_SDA_PIN        GPIO_Pin_9          /*!< I2C 数据线 SDA 引脚 (PB9) */
#define OLED_GPIO_PORT      GPIOB               /*!< SCL/SDA 所属 GPIO 端口 */
#define OLED_GPIO_CLK       RCC_APB2Periph_GPIOB/*!< 对应 GPIO 时钟使能位 */

#define SCL_SDA_DELAY_US    (0U)                /*!< 软件 I2C 延时（μs），0 表示无延时 */
#define OLED_I2C_ADDR       (0x78U)             /*!< OLED 模块 I2C 地址（7位左移后为 0x78） */


#define OLED_WIDTH     128											/*!< OLED屏幕宽度(像素) */																							
#define OLED_HEIGHT    64												/*!< OLED屏幕高度(像素) */
#define OLED_PAGES    (OLED_HEIGHT / 8)

/** @} */	


/*============================================================================
 *                          对外接口函数声明
 *============================================================================*/


// I2C通信函数 
void OLED_WriteCommand(uint8_t Command);
void OLED_WriteData(uint8_t *Data, uint8_t Count);

// OLED 初始化函数 
void OLED_Init(void);	

// 显示控制函数
void OLED_Update(void);
void OLED_UpdateArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height);
void OLED_Clear(void);
void OLED_ClearArea(int16_t X, int16_t Y, int16_t Width, int16_t Height);
void OLED_Reverse(void);
void OLED_ReverseArea(int16_t X, int16_t Y, int16_t Width, int16_t Height);

// 图像显示函数
void OLED_ShowImage(int16_t X, int16_t Y, uint16_t Width, uint16_t Height, const uint8_t *Image);

// 文本显示函数
void OLED_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize);
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize);
void OLED_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize);
void OLED_ShowMixString(int16_t X, int16_t Y, const char *String, uint8_t ChineseFontSize, uint8_t ASCIIFontSize);
void OLED_ShowChinese(int16_t X, int16_t Y, const char *Chinese, uint8_t FontSize);
void OLED_Printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...);
void OLED_PrintfMix(int16_t X, int16_t Y, uint8_t ChineseFontSize, uint8_t ASCIIFontSize, const char *format, ...);

// 区域显示函数
void OLED_ShowImageArea(int16_t X_Pic, int16_t Y_Pic, int16_t PictureWidth, int16_t PictureHeight, 
                       int16_t X_Area, int16_t Y_Area, int16_t AreaWidth, int16_t AreaHeight, const uint8_t *Image);
void OLED_ShowCharArea(int16_t RangeX, int16_t RangeY, int16_t RangeWidth, int16_t RangeHeight, 
                      int16_t X, int16_t Y, char Char, uint8_t FontSize);
void OLED_ShowStringArea(int16_t RangeX, int16_t RangeY, int16_t RangeWidth, int16_t RangeHeight, 
                        int16_t X, int16_t Y, char *String, uint8_t FontSize);
void OLED_ShowChineseArea(int16_t RangeX, int16_t RangeY, int16_t RangeWidth, int16_t RangeHeight, 
                         int16_t X, int16_t Y, char *Chinese, uint8_t FontSize);
void OLED_PrintfArea(int16_t RangeX, int16_t RangeY, int16_t RangeWidth, int16_t RangeHeight, 
                    int16_t X, int16_t Y, uint8_t FontSize, char *format, ...);
void OLED_ShowMixStringArea(int16_t RangeX, int16_t RangeY, int16_t RangeWidth, int16_t RangeHeight,
                           int16_t X, int16_t Y, const char *String, uint8_t ChineseFontSize, uint8_t ASCIIFontSize);
void OLED_PrintfMixArea(int16_t RangeX, int16_t RangeY, int16_t RangeWidth, int16_t RangeHeight,
                       int16_t X, int16_t Y, uint8_t ChineseFontSize, uint8_t ASCIIFontSize, const char *format, ...);

// 绘图函数
void OLED_DrawPoint(int16_t X, int16_t Y);
uint8_t OLED_GetPoint(uint8_t X, uint8_t Y);
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1);
void OLED_DrawRectangle(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint8_t IsFilled);
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled);
void OLED_DrawCircle(int16_t X, int16_t Y, int16_t Radius, uint8_t IsFilled);
void OLED_DrawEllipse(int16_t X, int16_t Y, int16_t A, int16_t B, uint8_t IsFilled);
void OLED_DrawArc(int16_t X, int16_t Y, int16_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled);
void OLED_DrawRoundedRectangle(int16_t X, int16_t Y, int16_t Width, int16_t Height, int16_t Radius, uint8_t IsFilled);





#ifdef __cplusplus
}
#endif

#endif
