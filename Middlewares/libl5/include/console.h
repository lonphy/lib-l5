//
// Created by lonphy on 2018/10/18.
//
#ifndef __LIB_L5_CONSOLE__
#define __LIB_L5_CONSOLE__

#include "mcu.h"


HAL_StatusTypeDef L5InitConsole(UART_HandleTypeDef *uart);

// 向控制台写入数据
HAL_StatusTypeDef L5ConsoleWrite(const char *buf, uint16_t bufSize);

// 从控制台读取数据
HAL_StatusTypeDef L5ConsoleRead(void *buf, uint bufSize);

// 销毁
void L5DeInitConsole(void);

#endif // __LIB_L5_CONSOLE__
