//
// Created by lonphy on 2019-01-27.
//

#ifndef RTOS_HW_H
#define RTOS_HW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mcu.h>

void _error_handler(char *, int);

#define Error_Handler() _error_handler(__FILE__, __LINE__)

/* wifi uart config */
#define WIFI_USART          UART4
#define WIFI_BAUD_RATE      2250000
#define WIFI_GPIO           GPIOC
#define WIFI_Tx             LL_GPIO_PIN_10
#define WIFI_Rx             LL_GPIO_PIN_11

#define WIFI_TX_TIMEOUT   1000
#define WIFI_RX_TIMEOUT   3000
#define AP_SSID           "ChinaNet-l5-client"
#define AP_PWD            "lonphy0814"
#define TCP_SERVER_IP     "192.168.1.17"
#define TCP_SERVER_PORT   8899
/* wifi uart config */

void hw_crc_init(void);

void hw_gpio_init(void);

void hw_i2c_init(I2C_TypeDef *i2c, uint8_t addr1);

void hw_spi_init(void);

void hw_log_usart_init();

void hw_dac_init(void);

void hw_dac_start_dma(void *buf, uint32_t len);
void hw_dac_stop();

void hw_wifi_usart_init();

void hw_usart_start_dma_rx(void *buf, uint32_t len);

void hw_usart_start_dma_tx(void *buf, uint32_t len);

uint32_t hw_usart_get_dma_rx_length();

uint32_t hw_crc32(const uint8_t *buf, uint32_t size);


#ifdef __cplusplus
}
#endif
#endif //RTOS_HW_H
