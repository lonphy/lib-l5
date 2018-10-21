//
// Created by lonphy on 2018/10/20.
//

#ifndef RTOS_LIB_L5_CONF_H
#define RTOS_LIB_L5_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

//#define L5_USE_CONSOLE
#define L5_USE_LED
//#define L5_USE_ESP8266
#define L5_USE_LCD1602


/*------------ LCD1602 config -------------- */
#ifdef L5_USE_LCD1602
#define LCD1602_DAT_GPIO GPIOB
#define LCD1602_CTL_GPIO GPIOB

#define LCD1602_DAT_PINS     GPIO_PIN_All&0xFF00 // PB8~PB15

#define LCD1602_EN_PIN       GPIO_PIN_0 // PB0
#define LCD1602_RS_PIN       GPIO_PIN_1 // PB1
#define LCD1602_RW_PIN       GPIO_PIN_2 // PB2

#endif
/*------------ LCD1602 config -------------- */

#ifdef L5_USE_LED
#define LED_GPIO GPIOB
#define LED_LED1_PIN GPIO_PIN_3 // PB3
#endif

#ifdef __cplusplus
}
#endif

#endif //RTOS_LIB_L5_CONF_H
