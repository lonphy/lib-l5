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
#define L5_USE_USART_CONSOLE
#define L5_USE_LED
/* #define L5_USE_AT24CXX */
/* #define L5_USE_ESP8266 */
/* #define L5_USE_LCD1602 */
/* #define L5_USE_LCD12864 */
/* #define L5_USE_OLED */
#define L5_USE_W25QXX

/*------------ LCD1602 config -------------- */
#if defined(L5_USE_LCD1602)
#define LCD1602_DAT_GPIO GPIOB
#define LCD1602_CTL_GPIO GPIOB

#define LCD1602_DAT_PINS     GPIO_PIN_All&0xFF00 // PB8~PB15

#define LCD1602_EN_PIN       GPIO_PIN_0 // PB0
#define LCD1602_RS_PIN       GPIO_PIN_1 // PB1
#define LCD1602_RW_PIN       GPIO_PIN_2 // PB2
#endif
/*------------ LCD1602 config -------------- */
/*------------ ESP8266 config -------------- */
#if defined(L5_USE_ESP8266)
#define WIFI_USART        UART4
#define WIFI_BAUD_RATE    115200
#define WIFI_WORK_BAUD_RATE 2250000 /* worker run baud rate */
#define WIFI_GPIO         GPIOC
#define WIFI_Tx           LL_GPIO_PIN_10
#define WIFI_Rx           LL_GPIO_PIN_11
#define AP_SSID           "ChinaNet-l5-client"
#define AP_PWD            "lonphy0814"
#define WIFI_ENABLE_GPIO  GPIOC
#define WIFI_ENABLE_PIN   LL_GPIO_PIN_6 /* for stm32f103rct6 */
#endif
/*------------ ESP8266 config -------------- */

/*------------ LCD12864 config -------------- */
#if defined(L5_USE_LCD12864)
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
#if defined(L5_USE_LED)
#define LED_LED1_GPIO GPIOA
#define LED_LED1_PIN  LL_GPIO_PIN_1 // PA1

#define LED_LED2_GPIO GPIOC
#define LED_LED2_PIN  LL_GPIO_PIN_13 // PC13

#endif
/*------------ LED config -------------- */

#if defined(L5_USE_AT24CXX)
// SCL->PB6, SDA->PB7
#define AT24CXX_I2C     I2C1
#define AT24CXX_ADDRESS 0xA0
#endif

#if defined(L5_USE_OLED)
// SCL->PB10, SDA->PB11
#define OLED_I2C        I2C2
#define OLED_ADDRESS    0x78
#endif

#ifdef __cplusplus
}
#endif

#endif //RTOS_LIB_L5_CONF_H
