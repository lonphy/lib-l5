//
// Created by lonphy on 2018/10/25.
//
#include <lib_l5.h>
#include "hw.h"

#if defined(L5_USE_LCD12864)

#include <string.h>
#include "uart.h"
#include "ring_queue.h"


typedef struct {
    uint8_t cmd;
    uint8_t rawSize;
    uint8_t *raw;
} Command_t;

osPoolDef(cmdPool, 3, Command_t);
osPoolId cmdPool;
static osMessageQId uartQid = 0;
static uint8_t rxBuf = 0;

static L5_RingQueue_t *uart1BufferQueue = 0;

void initL5Lib() {
    L5_LedInit();
    L5_Lcd12864Init();
}

void task_lcd12864(void const *arg) {
    (void) arg;


    initL5Lib();

    osEvent evt;
    Command_t *cmd;

    L5_Lcd12864SetPageFromBottom2Top();     // my lcd12864's screen is reverse.
    L5_Lcd12864ClearScreen();

    // define
    osMessageQDef(uart1TxQ, 32, Command_t*);

    uartQid = osMessageCreate(osMessageQ(uart1TxQ), NULL);
    if (uartQid == 0) {
        L5_Lcd12864WriteString(0, 0, "queue create err");
        return;
    }

    // open rx it success or go die.
    HAL_UART_Receive_IT(&hUart1, &rxBuf, 1);

    while (1) {
        evt = osMessageGet(uartQid, osWaitForever);
        if (evt.status == osEventMessage) {
            cmd = evt.value.p;
            switch (cmd->cmd) {
                case 0x01:
                    L5_LedToggle(Led1);
                    break;
                case 0x02: {
                    L5_Lcd12864WriteString(0, 5, (char *) cmd->raw);
                    vPortFree(cmd->raw);
                }
                    break;
                case 0x03: {

                }
                    break;
                default: {
                    L5_Lcd12864WriteString(0, 1, "unknown cmd");
                }
            }
            osPoolFree(cmdPool, cmd);
        } else {
            L5_Lcd12864WriteString(0, 3, "error~");
        }
    }

    // delete, mark
    // osMessageDelete(uartQid);
}


void task_parser(const void *arg) {
    UNUSED(arg);

    uart1BufferQueue = L5_RingQueueInit(128);

    cmdPool = osPoolCreate(osPool(cmdPool));
    Command_t *cmd = NULL;
    uint8_t b1, b2, i;

    while (1) {
        LOOP:
        b1 = 0;
        b2 = 0;
        while (osOK != L5_RingQueueRead(uart1BufferQueue, &b1)) {
            osThreadYield();
        }

        if (!(b1 == 1 || b1 == 2 || b1 == 3)) {
            continue;
        }

        while (osOK != L5_RingQueueRead(uart1BufferQueue, &b2)) {
            osThreadYield();
        }

        cmd = osPoolAlloc(cmdPool);
        switch (cmd->cmd = b1) {
            case 1:
                cmd->rawSize = 0;
                break;
            case 2: {
                cmd->rawSize = b2;
                if (b2 > 0 && b2 <= 18) {
                    cmd->raw = pvPortMalloc(18);
                    i = 0;
                    while (i < b2) {
                        if (osOK == L5_RingQueueRead(uart1BufferQueue, &(cmd->raw[i]))) {
                            ++i;
                        }
                        osThreadYield();
                    }

                    // Just for debug
                    __asm volatile("nop");
                } else {
                    osPoolFree(cmdPool, cmd);
                    goto LOOP;
                }
            }
                break;
            default:
                (void) 0;
        }

        osMessagePut(uartQid, (uint32_t) cmd, osWaitForever);
    }
}

// uart1's rx complete callback
void L5_UART1_RxCpltCallback(UART_HandleTypeDef *hUart) {
    L5_RingQueueWrite(uart1BufferQueue, rxBuf);
    // must success or go die.
    while (HAL_UART_Receive_IT(hUart, &rxBuf, 1) != HAL_OK);
}

#endif