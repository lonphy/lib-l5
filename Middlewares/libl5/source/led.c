//
// Created by lonphy on 2018/10/17.
//
#include "lib_l5.h"

#if defined(L5_USE_LED)

const uint32_t led_pin[LedN] = {
#if defined(LED_LED1_PIN)
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

#ifdef LED_LED1_PIN
    LL_GPIO_SetPinMode(LED_GPIO, LED_LED1_PIN, LL_GPIO_MODE_OUTPUT);
#endif
#ifdef LED_LED2_PIN
    LL_GPIO_SetPinMode(LED_GPIO, LED_LED2_PIN, LL_GPIO_MODE_OUTPUT);
#endif
#ifdef LED_LED3_PIN
    LL_GPIO_SetPinMode(LED_GPIO, LED_LED2_PIN, LL_GPIO_MODE_OUTPUT);
#endif

#ifdef LED_LED1_PIN
    l5_led_off(Led1);
#endif
}

void l5_led_on(LedType led) {
    LL_GPIO_SetOutputPin(LED_GPIO, led_pin[led]);
}

void l5_led_off(LedType led) {
    LL_GPIO_ResetOutputPin(LED_GPIO, led_pin[led]);
}

void l5_led_toggle(LedType led) {
    LL_GPIO_TogglePin(LED_GPIO, led_pin[led]);
}

#endif // defined(L5_USE_LED)
