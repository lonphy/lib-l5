//
// Created by lonphy on 2018/10/17.
//
#include "lib_l5.h"

#if defined(L5_USE_LED)

void l5_led_init(void) {

#if defined(LED_LED1_PIN)
    LL_GPIO_SetPinMode(LED_LED1_GPIO, LED_LED1_PIN, LL_GPIO_MODE_OUTPUT);
#endif
#if defined(LED_LED2_PIN)
    LL_GPIO_SetPinMode(LED_LED2_GPIO, LED_LED2_PIN, LL_GPIO_MODE_OUTPUT);
#endif
#if defined(LED_LED3_PIN)
    LL_GPIO_SetPinMode(LED_LED3_GPIO, LED_LED3_PIN, LL_GPIO_MODE_OUTPUT);
#endif
}

void l5_led_on(LedType led) {
#if  defined(LED_LED1_PIN)
    if (led == Led1) {
        LL_GPIO_SetOutputPin(LED_LED1_GPIO, LED_LED1_PIN);
    }
#endif

#if  defined(LED_LED2_PIN)
    if (led == Led2) {
        LL_GPIO_SetOutputPin(LED_LED2_GPIO, LED_LED2_PIN);
    }
#endif

#if  defined(LED_LED3_PIN)
    if (led == Led3) {
        LL_GPIO_SetOutputPin(LED_LED3_GPIO, LED_LED3_PIN);
    }
#endif
}

void l5_led_off(LedType led) {
#if  defined(LED_LED1_PIN)
    if (led == Led1) {
        LL_GPIO_ResetOutputPin(LED_LED1_GPIO, LED_LED1_PIN);
    }
#endif

#if  defined(LED_LED2_PIN)
    if (led == Led2) {
        LL_GPIO_ResetOutputPin(LED_LED2_GPIO, LED_LED2_PIN);
    }
#endif

#if  defined(LED_LED3_PIN)
    if (led == Led3) {
        LL_GPIO_ResetOutputPin(LED_LED3_GPIO, LED_LED3_PIN);
    }
#endif
}

void l5_led_toggle(LedType led) {
#if  defined(LED_LED1_PIN)
    if (led == Led1) {
        LL_GPIO_TogglePin(LED_LED1_GPIO, LED_LED1_PIN);
    }
#endif

#if  defined(LED_LED2_PIN)
    if (led == Led2) {
        LL_GPIO_TogglePin(LED_LED2_GPIO, LED_LED2_PIN);
    }
#endif

#if  defined(LED_LED3_PIN)
    if (led == Led3) {
        LL_GPIO_TogglePin(LED_LED3_GPIO, LED_LED3_PIN);
    }
#endif
}

#endif // defined(L5_USE_LED)
