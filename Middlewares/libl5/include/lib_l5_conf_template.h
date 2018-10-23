//
// Created by lonphy on 2018/10/20.
//

#ifndef __LIB_L5_CONF_H__
#define __LIB_L5_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define L5_USE_CONSOLE
#define L5_USE_LED
#define L5_USE_ESP8266
#define L5_USE_LCD1602
#define L5_USE_LCD12864

/*------------ LCD1602 config -------------- */
#ifdef L5_USE_LCD1602
#define LCD1602_DAT_GPIO GPIOE
#define LCD1602_CTL_GPIO GPIOD

#define LCD1602_DAT_PINS     GPIO_PIN_All&0xFF00 // PE8~PE15

#define LCD1602_RS_PIN       GPIO_PIN_1 // PD13
#define LCD1602_RW_PIN       GPIO_PIN_2 // PD12
#define LCD1602_EN_PIN       GPIO_PIN_0 // PD11

#endif
/*------------ LCD1602 config -------------- */

/*------------ LCD12864 config -------------- */
#ifdef L5_USE_LCD12864

#endif
/*------------ LCD12864 config -------------- */

#ifdef __cplusplus
}
#endif

#endif //__LIB_L5_CONF_H__
