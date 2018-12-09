//
// Created by lonphy on 2018/11/10.
//

#include "lib_l5.h"
#include "hw_i2c.h"
#include "oled_ssd1306_font.h"

#ifdef L5_USE_OLED

#define OLED_ADDRESS 0x78

static inline void i2c_write(uint8_t address, uint8_t data) {
    HAL_I2C_Mem_Write(&hI2C1, OLED_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xffff);
}

static inline void write_cmd(uint8_t I2C_Command) {
    i2c_write(0x00, I2C_Command);
}

static inline void write_data(uint8_t I2C_Data) {
    i2c_write(0x40, I2C_Data);
}

void l5_oled_init(void) {
    HAL_Delay(2000);

    write_cmd(0xAE); /* display off */
    write_cmd(0x20); /* Set Memory Addressing Mode */
    write_cmd(
            0x10);    //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    write_cmd(0xb0);    //Set Page Start Address for Page Addressing Mode,0-7
    write_cmd(0xc8);    //Set COM Output Scan Direction
    write_cmd(0x00); //---set low column address
    write_cmd(0x10); //---set high column address
    write_cmd(0x40); //--set start line address
    write_cmd(0x81); //--set contrast control register
    write_cmd(0xff); //¡¡∂»µ˜Ω⁄ 0x00~0xff
    write_cmd(0xa1); //--set segment re-map 0 to 127
    write_cmd(0xa6); //--set normal display
    write_cmd(0xa8); //--set multiplex ratio(1 to 64)
    write_cmd(0x3F); //
    write_cmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    write_cmd(0xd3); //-set display offset
    write_cmd(0x00); //-not offset
    write_cmd(0xd5); //--set display clock divide ratio/oscillator frequency
    write_cmd(0xf0); //--set divide ratio
    write_cmd(0xd9); //--set pre-charge period
    write_cmd(0x22); //
    write_cmd(0xda); //--set com pins hardware configuration
    write_cmd(0x12);
    write_cmd(0xdb); //--set vcomh
    write_cmd(0x20); //0x20,0.77xVcc
    write_cmd(0x8d); //--set DC-DC enable
    write_cmd(0x14); //
    write_cmd(0xaf); //--turn on oled panel
}

void OLED_SetPos(uint8_t x, uint8_t y) {
    write_cmd(0xb0 + y);
    write_cmd(((x & 0xf0) >> 4) | 0x10);
    write_cmd((x & 0x0f) | 0x01);
}

void OLED_Fill(uint8_t fill_Data) {
    uint8_t m, n;
    for (m = 0; m < 8; m++) {
        write_cmd((uint8_t) 0xb0 + m);        //page0-page1
        write_cmd(0x00);        //low column start address
        write_cmd(0x10);        //high column start address
        for (n = 0; n < 128; n++) {
            write_data(fill_Data);
        }
    }
}

void l5_oled_clear_screen(void) {
    OLED_Fill(0x00);
}

void OLED_ON(void) {
    write_cmd(0X8D);
    write_cmd(0X14);
    write_cmd(0XAF);
}

void OLED_OFF(void) {
    write_cmd(0X8D);
    write_cmd(0X10);
    write_cmd(0XAE);
}

void l5_oled_show_string(uint8_t x, uint8_t y, char ch[], oled_font_t font) {
    uint8_t c = 0, i = 0, j = 0;
    switch (font) {
        case FONT_6x8: {
            while (ch[j] != '\0') {
                c = (uint8_t)(ch[j] - 32);
                if (x > 126) {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x, y);
                for (i = 0; i < 6; i++) {
                    write_data(F6x8[c * 6 + i]);
                }
                x += 6;
                j++;
            }
        }
            break;
        case FONT_8x16: {
            while (ch[j] != '\0') {
                c = (uint8_t)(ch[j] - 32);
                if (x > 120) {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x, y);
                for (i = 0; i < 8; i++) {
                    write_data(F8X16[c * 16 + i]);
                }
                OLED_SetPos(x, y + (uint8_t)1);
                for (i = 0; i < 8; i++) {
                    write_data(F8X16[c * 16 + i + 8]);
                }
                x += 8;
                j++;
            }
        }
            break;
    }
}

#endif