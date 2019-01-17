//
// Created by lonphy on 2018/10/17.
//
#ifndef __LIB_L5_LED_H__
#define __LIB_L5_LED_H__

#include "mcu.h"

#if defined(L5_USE_LED)

#if !(\
    (defined(LED_LED1_GPIO) && defined(LED_LED1_PIN)) || \
    (defined(LED_LED2_GPIO) && defined(LED_LED2_PIN)) || \
    (defined(LED_LED3_GPIO) && defined(LED_LED3_PIN))    \
)
#error (LIBL5 LED GPIO not configed)
#endif

typedef enum {
#if  defined(LED_LED1_PIN)
    Led1,
#endif
#if defined(LED_LED2_PIN)
    Led2,
#endif
#if defined(LED_LED3_PIN)
    Led3,
#endif
    LedN
} LedType;

void l5_led_init(void);

void l5_led_on(LedType led);

void l5_led_off(LedType led);

void l5_led_toggle(LedType led);

#endif // L5_USE_LED

#endif // __LIB_L5_LED_H__
