//
// Created by lonphy on 2018/10/18.
//
#ifndef __LIB_L5_CONSOLE__
#define __LIB_L5_CONSOLE__

#include "mcu.h"


HAL_StatusTypeDef L5InitConsole(UART_HandleTypeDef *uart);

HAL_StatusTypeDef L5ConsoleWrite(const char *buf, uint16_t bufSize);

// TODO
HAL_StatusTypeDef L5ConsoleRead(void *buf, uint bufSize);

void L5DeInitConsole(void);

#endif // __LIB_L5_CONSOLE__
