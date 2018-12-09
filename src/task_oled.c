/* *
 * OLED example
 * @author lonphy
 * @date 2018-11-10
 */
#include <lib_l5.h>

#ifdef L5_USE_OLED

void task_oled(__unused void const *arg) {
    printf("task_oled start\n");

    l5_oled_init();
    l5_oled_clear_screen();

    l5_oled_show_string(16, 0, "OLED SSD1306", FONT_8x16);
    l5_oled_show_string(0, 2, "STM32's IIC example", FONT_6x8);
    l5_oled_show_string(0, 4, "UART I2S ISA", FONT_6x8);
    l5_oled_show_string(0, 7, "SDIO CRC AES GPIO etc", FONT_6x8);

    char buf[21] = {0};
    uint8_t count = 0;
    while (1) {
        osDelay(100);
        sprintf(buf, " [%03d] -> Hex[0x%02x] ", count, count);
        count++;
        l5_oled_show_string(0, 4, buf, FONT_6x8);
    }
}

#endif