//
// Created by lonphy on 2018/10/20.
//

#ifndef RTOS_LIB_L5_CONF_H
#define RTOS_LIB_L5_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* #define L5_USE_CONSOLE */
#define L5_USE_LED
#define L5_USE_ESP8266
/* #define L5_USE_LCD1602 */
#define L5_USE_LCD12864

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

/*------------ LCD12864 config -------------- */
#ifdef L5_USE_LCD12864
#define LCD12864_RW_PIN      GPIO_PIN_0  // PB0
#define LCD12864_RS_PIN      GPIO_PIN_1  // PB1
#define LCD12864_RD_PIN      GPIO_PIN_2  // PB2
#define LCD12864_CS_PIN      GPIO_PIN_8  // PA8
#define LCD12864_RESET_PIN   GPIO_PIN_11 // PA11

#define LCD12864_DAT_PINS     GPIO_PIN_All&0xFF00 // PB8~PB15
#define LCD12864_DAT_GPIO     GPIOB

#endif
/*------------ LCD12864 config -------------- */

/*------------ LED config -------------- */
#ifdef L5_USE_LED
#define LED_GPIO GPIOB
#define LED_LED1_PIN GPIO_PIN_4 // PB3

#if 0
// TODO: Just for testing.
#define LED_GPIO GPIOA
#define LED_LED1_PIN GPIO_PIN_8  // PA8
#define LED_LED2_PIN GPIO_PIN_11  // PA11
#endif

#endif
/*------------ LED config -------------- */

#ifdef __cplusplus
}
#endif

#endif //RTOS_LIB_L5_CONF_H
