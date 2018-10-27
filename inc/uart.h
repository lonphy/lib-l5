//
// Created by lonphy on 2018/10/19.
//

#ifndef RTOS_UART_H
#define RTOS_UART_H

#include "stm32f1xx_hal.h"
#include "main.h"

extern UART_HandleTypeDef hUart1;
void UartInit();

// uart Rx complete callback
void L5_UART1_RxCpltCallback(UART_HandleTypeDef *hUart);
void L5_UART2_RxCpltCallback(UART_HandleTypeDef *hUart);
void L5_UART3_RxCpltCallback(UART_HandleTypeDef *hUart);

// uart Err callback
void L5_UART1_ErrorCallback(UART_HandleTypeDef *hUart);
void L5_UART2_ErrorCallback(UART_HandleTypeDef *hUart);
void L5_UART3_ErrorCallback(UART_HandleTypeDef *hUart);

#endif //RTOS_UART_H
