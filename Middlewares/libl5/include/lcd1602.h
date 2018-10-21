//
// Created by lonphy on 2018/10/20.
//

#ifndef __LIB_L5_LCD1602_H__
#define __LIB_L5_LCD1602_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu.h"

/********************************硬件接口定义*********************************/
#ifndef LCD1602_RS_PIN
#error LCD1602 RS PIN not configed
#endif

#ifndef LCD1602_RW_PIN
#error LCD1602 RS PIN not configed
#endif

#ifndef LCD1602_EN_PIN
#error LCD1602 EN pin not configed
#endif

#ifndef LCD1602_CTL_GPIO
#error LCD1602 control gpio not configed
#endif

#ifndef LCD1602_DAT_GPIO
#error LCD1602 data gpio not configed
#endif

#ifndef LCD1602_DAT_PINS
#error LCD1602 data pins not configed
#endif

/* command defines */
#define __CMD_CLEAR      0x01   /* clear screen */
#define __CMD_POS_RESET  0x02   /* set cursor to home */

/* 输入方式设置 */
#define __AC_INC         0x02   /* AC inc 1 after r/w */
#define __AC_DEC         0x00   /* AC dec 1 after r/w */
#define __SCROLL_ENABLE  0x01   /* screen scroll after r/w */
#define __SCROLL_DISABLE 0x00   /* screen fixed after r/w */

#define __INPUT_CMD(__AC__, __SCROLL__) (0x04 |__AC__|__SCROLL__)

/* 显示开关控制 */
#define __SCREEN_ON      0x04
#define __SCREEN_OFF     0x00
#define __CURSOR_ON      0x02
#define __CURSOR_OFF     0x00
#define __BLINK_ON       0x01
#define __BLINK_OFF      0x00

#define __DISPLAY_SWITCH_CMD(__SCREEN__, __CURSOR__, __BLINK__) (0x08| __SCREEN__ | __CURSOR__ | __BLINK__)


/* 光标、画面位移 */
#define __MOV_SCREEN     0x08 /*移动画面*/
#define __MOV_CURSOR     0x00 /*移动光标*/
#define __MOV_RIGHT      0x04 /*向右移动*/
#define __MOV_LEFT       0x00 /*向左移动*/

#define __MOV_CMD(__S__, __DIR__) (0x10|__S__|__DIR__)

/* 功能设置 */
#define __FUNC_8Bit       0x10
#define __FUNC_4Bit       0x00
#define __FUNC_2_ROW      0x08
#define __FUNC_1_ROW      0x00
#define __FUNC_DOT_5x10   0x04
#define __FUNC_DOT_5x7    0x00

#define __FUNC_CMD(__DW__, __ROW__, __DOT__) (0x20|__DW__|__ROW__|__DOT__)

/* CGRAM地址 [0x00, 0x3f] */
#define _CGRAM_CMD(__ADDR__) (0x40|__ADDR__)

/* DDRAM地址
    单行: [0x00, 0x4f]
    两行:
        首行 [0x00, 0x2f]
        次行 [0x40, 0x67]
 */
#define _DDRAM_CMD(__ADDR__) (0x80|__ADDR__)

/* 读忙、AC值 */
#define __LCD_BUSY(n)  ((n>>7) & 0x01)
#define __LCD_AC(n)    (n&0x7f)


#define L5_Lcd1602_ClearScreen() L5_Lcd1602_WriteCmd(__CMD_CLEAR)
#define L5_Lcd1602_PosReset()    L5_Lcd1602_WriteCmd(__CMD_POS_RESET)

typedef enum {
    CursorLeft,
    CursorRight,
} LcdCursorDir;

void L5_Lcd1602_Init();

void L5_Lcd1602_SetPos(uint8_t x, uint8_t y);

// 设置光标移动方向
void L5_Lcd1602_SetCursorDir(LcdCursorDir dir);

void L5_Lcd1602_WriteData(uint16_t dat);

void L5_Lcd1602_WriteCmd(uint16_t cmd);

void L5_Lcd1602_WriteString(uint8_t x, uint8_t y, char *str);

#ifdef __cplusplus
}
#endif

#endif //__LIB_L5_LCD1602_H__
