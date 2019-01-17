//
// Created by lonphy on 2019/1/13.
//
#include <lib_l5.h>
#include <stdio.h>

#define led_log(f, ...) printf("[task_led] " f "\r\n", ##__VA_ARGS__)

void task_led(const void *arg) {
    l5_led_init();

    l5_led_off(Led1);
    l5_led_on(Led2);
    led_log("started~");

    while(1) {

        osDelay(1000);
        l5_led_toggle(Led1);
        l5_led_toggle(Led2);
        led_log("toggle");
    }
}