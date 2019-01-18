//
// Created by lonphy on 2018/10/19.
//

#ifndef RTOS_UART_H
#define RTOS_UART_H

#include "mcu.h"
#include "main.h"

void hw_wifi_usart_init();

void hw_usart_start_dma_rx(void * buf, uint32_t len);
void hw_usart_start_dma_tx(void * buf, uint32_t len);
uint32_t hw_usart_get_dma_rx_length();

#endif //RTOS_UART_H
