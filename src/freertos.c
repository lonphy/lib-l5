#include <FreeRTOS.h>
#include <cmsis_os.h>
#include <task.h>
#include <string.h>
#include <lib_l5.h>

#include "main.h"

void ExpUartTask(void const *arg);
void TaskParser(const void *arg);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

void MX_FREERTOS_Init(void) {

    osThreadDef(uartQueue, ExpUartTask, osPriorityNormal, 0, 1024);
    osThreadCreate(osThread(uartQueue), NULL);

    osThreadDef(parser, TaskParser, osPriorityNormal, 1, 512);
    osThreadCreate(osThread(parser), NULL);
}
