//
// Created by lonphy on 2018/10/17.
//
#include "lib_l5.h"

#if defined(L5_USE_LED)

const uint16_t led_pin[LedN] = {
#ifdef LED_LED1_PIN
        LED_LED1_PIN,
#endif
#ifdef LED_LED2_PIN
        LED_LED2_PIN,
#endif
#ifdef LED_LED3_PIN
        LED_LED3_PIN,
#endif
};

void l5_led_init(void) {
    GPIO_InitTypeDef gpioOpt = {0};
    gpioOpt.Pin = 0;
#ifdef LED_LED1_PIN
    gpioOpt.Pin |= LED_LED1_PIN;
#endif
#ifdef LED_LED2_PIN
    gpioOpt.Pin |= LED_LED2_PIN;
#endif
#ifdef LED_LED3_PIN
    gpioOpt.Pin |= LED_LED3_PIN;
#endif
    gpioOpt.Mode = GPIO_MODE_OUTPUT_PP;
    gpioOpt.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO, &gpioOpt);
#ifdef LED_LED1_PIN
    l5_led_off(Led1);
#endif
}

void l5_led_on(LedType led) {
    HAL_GPIO_WritePin(LED_GPIO, led_pin[led], GPIO_PIN_SET);
}

void l5_led_off(LedType led) {
    HAL_GPIO_WritePin(LED_GPIO, led_pin[led], GPIO_PIN_RESET);
}

void l5_led_toggle(LedType led) {
    HAL_GPIO_TogglePin(LED_GPIO, led_pin[led]);
}

#endif // defined(L5_USE_LED)
