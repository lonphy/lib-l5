//
// Created by lonphy on 2018/10/17.
//
#ifndef __LIB_L5_LED_H__
#define __LIB_L5_LED_H__

#include "mcu.h"

#if defined(L5_USE_LED)

#ifndef LED_GPIO
#error (LIBL5 LED GPIO not configed)
#endif

#if !(defined(LED_LED1_PIN) || defined(LED_LED2_PIN) || defined(LED_LED3_PIN))
#error (LIBL5 LED pin not configed)
#endif

typedef enum {
#ifdef LED_LED1_PIN
    Led1,
#endif
#ifdef LED_LED2_PIN
    Led2,
#endif
#ifdef LED_LED3_PIN
    Led3,
#endif
    LedN
} LedType;

void L5_LedInit(void);

void L5_LedOn(LedType led);

void L5_LedOff(LedType led);

void L5_LedToggle(LedType led);

#endif // L5_USE_LED

#endif // __LIB_L5_LED_H__
