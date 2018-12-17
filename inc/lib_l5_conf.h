//
// Created by lonphy on 2018/10/20.
//

#ifndef RTOS_LIB_L5_CONF_H
#define RTOS_LIB_L5_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* #define L5_USE_ITM_CONSOLE */
#define L5_USE_LED
#define L5_USE_ESP8266
/* #define L5_USE_LCD1602 */
/* #define L5_USE_LCD12864 */
/* #define L5_USE_OLED */

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
/*------------ ESP8266 config -------------- */
#ifdef L5_USE_ESP8266
#define ESP8266_USART     UART4_BASE
//#define ESP8266_BAUD_RATE 115200
#define ESP8266_BAUD_RATE 460800
//#define ESP8266_RUN_BAUD_RATE 192000
#define ESP8266_RUN_BAUD_RATE 460800
#define ESP8266_GPIO      GPIOC
#define ESP8266_Tx        GPIO_PIN_10
#define ESP8266_Rx        GPIO_PIN_11
#define AP_SSID           "ChinaNet-l5-client"
#define AP_PWD            "lonphy0814"
#endif
/*------------ ESP8266 config -------------- */

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
#define LED_GPIO GPIOC
#define LED_LED1_PIN GPIO_PIN_13 // PC13

#endif
/*------------ LED config -------------- */

#ifdef __cplusplus
}
#endif

#endif //RTOS_LIB_L5_CONF_H
