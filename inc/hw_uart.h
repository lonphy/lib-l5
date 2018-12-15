//
// Created by lonphy on 2018/10/19.
//

#ifndef RTOS_UART_H
#define RTOS_UART_H

#include "mcu.h"
#include "main.h"

extern UART_HandleTypeDef hUart1;

void hw_uart_init();

// uart Rx complete callback
void L5_UART1_RxCpltCallback(UART_HandleTypeDef *hUart);

// uart Err callback
void L5_UART1_ErrorCallback(UART_HandleTypeDef *hUart);

#endif //RTOS_UART_H
