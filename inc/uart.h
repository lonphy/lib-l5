//
// Created by lonphy on 2018/10/19.
//

#ifndef RTOS_UART_H
#define RTOS_UART_H

#include "stm32f1xx_hal.h"
#include "main.h"

extern UART_HandleTypeDef hUart1;
void UartInit();

#endif //RTOS_UART_H
