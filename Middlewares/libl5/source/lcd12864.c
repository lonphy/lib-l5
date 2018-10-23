//
// Created by lonphy on 2018/10/21.
//

#include "lib_l5.h"
#include "lcd12864_font.h"

#ifdef L5_USE_LCD12864

// TODO: gpio config extract

/* /CS pin */
#define LCD12864_CS(n)    HAL_GPIO_WritePin(GPIOA, LCD12864_CS_PIN,    n)
#define LCD12864_RD(n)    HAL_GPIO_WritePin(GPIOB, LCD12864_RD_PIN,    n)
/* A0 pin */
#define LCD12864_RS(n)    HAL_GPIO_WritePin(GPIOB, LCD12864_RS_PIN,    n)
/* /WR pin */
#define LCD12864_RW(n)    HAL_GPIO_WritePin(GPIOB, LCD12864_RW_PIN,    n)
/* /RES pin */
#define LCD12864_RESET(n) HAL_GPIO_WritePin(GPIOA, LCD12864_RESET_PIN, n)

#define lcd12864_max_page 8
#define lcd12864_max_col  131


void L5_Lcd12864IsBusy(void) {
    LCD12864_DAT_GPIO->BSRR = 0x0000ff00; // PB8~PB15 置1
    LCD12864_CS(0);
    LCD12864_RD(0);
    LCD12864_RS(0);
    LCD12864_RW(1);
    while (LCD12864_DAT_GPIO->IDR >> 15);
}

void L5_Lcd12864Init(void) {

    GPIO_InitTypeDef gpioOpt;
    gpioOpt.Pin = LCD12864_DAT_PINS;
    gpioOpt.Mode = GPIO_MODE_OUTPUT_OD;
    gpioOpt.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LCD12864_DAT_GPIO, &gpioOpt);

    gpioOpt.Pin = (LCD12864_RS_PIN | LCD12864_RW_PIN | LCD12864_RD_PIN);
    gpioOpt.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOB, &gpioOpt);

    gpioOpt.Pin = (LCD12864_CS_PIN | LCD12864_RESET_PIN);
    gpioOpt.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &gpioOpt);

    osDelay(1);

    // enter reset mode
    LCD12864_RESET(0);
    osDelay(1);
    LCD12864_CS(1);
    LCD12864_RESET(1);

    //----------------Star Initial Sequence-------//
    L5_Lcd12864WriteCmd(cmd14_reset);
    osDelay(1);

    L5_Lcd12864WriteCmd(cmd11_bias_1_9);

    // Booster Ratio Select Mode (2 Byte Command)
    L5_Lcd12864WriteCmd(0xF8);  // fixed
    L5_Lcd12864WriteCmd(0x01);  // 0 <= value(default 0x00) <= 0x03

    // Enter Electronic Volume Mode (2 Byte Command)
    L5_Lcd12864WriteCmd(0x81); // fixed
    L5_Lcd12864WriteCmd(0x23); // 0x01 <= value(default 0x20) <= 0x2f

    L5_Lcd12864WriteCmd(0x25);  //Ra/Rb set
    L5_Lcd12864WriteCmd(cmd16_power_ctrl(1, 1, 1));

    L5_Lcd12864SetColFromLeftToRight();
    L5_Lcd12864SetPageFromTop2Bottom();

    osDelay(1);
    L5_Lcd12864SetStartLine(0);
    L5_Lcd12864CDisplayON();
    osDelay(1);
}

void L5_Lcd12864WriteCmd(uint8_t cmd) {
    L5_Lcd12864IsBusy();
    LCD12864_CS(0);
    LCD12864_RD(1);
    LCD12864_RS(0);
    LCD12864_RW(0);
    LCD12864_DAT_GPIO->BSRR = ((uint32_t) cmd << 8) & 0xff00;
    LCD12864_DAT_GPIO->BRR = (((uint32_t) (~cmd)) << 8) & 0xff00;
    LCD12864_RW(1);
}

void L5_Lcd12864WriteData(uint8_t dat) {
    L5_Lcd12864IsBusy();
    LCD12864_CS(0);
    LCD12864_RD(1);
    LCD12864_RS(1);
    LCD12864_RW(0);
    LCD12864_DAT_GPIO->BSRR = ((uint32_t) dat << 8) & 0xff00;
    LCD12864_DAT_GPIO->BRR = ((uint32_t) (~dat) << 8) & 0xff00;
    LCD12864_RW(1);
}

void L5_Lcd12864ClearScreen(void) {
    uint8_t page, col;
    for (page = 0; page < lcd12864_max_page; page++) {
        L5_Lcd12864SetPage(page);
        L5_Lcd12864SetColumn(0);
        for (col = 0; col < lcd12864_max_col; col++) {
            L5_Lcd12864WriteData(0x00);
        }
    }
}


void L5_Lcd12864WriteString(uint8_t x, uint8_t y, const char *str) {
    L5_Lcd12864SetColumn(x * font5x8_display_width);
    L5_Lcd12864SetPage(y);

    uint16_t charStart, i;
    do {
        charStart = (uint16_t) ((*str - 0x20) * font5x8_width);
        if (charStart > font5x8_last_offset) {
            charStart = font5x8_unknow;
        }
        L5_Lcd12864WriteData(0);
        for (i = 0; i < font5x8_width; ++i) {
            L5_Lcd12864WriteData(font5x8[charStart + i]);
        }
        L5_Lcd12864WriteData(0);
    } while (*(++str));
}

#endif
