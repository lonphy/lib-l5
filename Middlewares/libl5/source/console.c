//
// Created by lonphy on 2018/10/18.
//


#include "lib_l5.h"

#if defined(L5_USE_CONSOLE)

static UART_HandleTypeDef *consoleUart;
static osMutexId txMuxId;
#define consoleTxLock(t) osMutexWait(txMuxId, t)
#define consoleTxUnLock() osMutexRelease(txMuxId)

HAL_StatusTypeDef L5InitConsole(UART_HandleTypeDef *uart) {
    consoleUart = uart;
    osMutexDef(tx);
    txMuxId = osMutexCreate(osMutex(tx));
    return HAL_OK;
}

// 向控制台写入数据
HAL_StatusTypeDef L5ConsoleWrite(const char *buf, uint16_t bufSize) {
    HAL_StatusTypeDef result;
    consoleTxLock(osWaitForever);
    taskENTER_CRITICAL();
    result = HAL_UART_Transmit(consoleUart, (uint8_t *) buf, bufSize, 0xffff);
    taskEXIT_CRITICAL();
    consoleTxUnLock();
    return result;
}

// 从控制台读取数据
HAL_StatusTypeDef L5ConsoleRead(void *buf, uint bufSize) {
    return HAL_OK;
}

// 销毁
void L5DeInitConsole(void) {
    consoleUart = NULL;
    osMutexDelete(txMuxId);
}

#endif // defined(L5_USE_CONSOLE)
