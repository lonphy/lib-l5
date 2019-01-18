//
// Created by lonphy on 2018/11/10.
//

#ifndef __LIB_L5_OLED_SSD1306_H__
#define __LIB_L5_OLED_SSD1306_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu.h"

typedef enum {
    FONT_6x8,
    FONT_8x16
} oled_font_t;

extern void oled_i2c_write(I2C_TypeDef *i2c, uint8_t dev_address, uint8_t mem_address, uint8_t dat);

void l5_oled_init(void);

void OLED_SetPos(uint8_t x, uint8_t y);

void OLED_Fill(uint8_t fill_Data);

void l5_oled_clear_screen(void);

void OLED_ON(void);

void OLED_OFF(void);

void l5_oled_show_string(uint8_t x, uint8_t y, char ch[], oled_font_t font);

#ifdef __cplusplus
}
#endif

#endif //__LIB_L5_OLED_SSD1306_H__
