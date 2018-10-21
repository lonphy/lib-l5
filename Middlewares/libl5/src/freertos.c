#include <FreeRTOS.h>
#include <cmsis_os.h>
#include <task.h>
#include <string.h>
#include <lib_l5.h>

#include "main.h"
#include "uart.h"

osThreadId ledTaskHandle[2];


void taskLed(void const *argument);

void taskLcd1602(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

void MX_FREERTOS_Init(void) {
    osThreadDef(led1, taskLed, osPriorityNormal, 0, 64);
    ledTaskHandle[0] = osThreadCreate(osThread(led1), NULL);

    // lcd1602
    osThreadDef(lcd, taskLcd1602, osPriorityNormal, 0, 512);
    ledTaskHandle[1] = osThreadCreate(osThread(lcd), NULL);

}


void taskLed(void const *argument) {
    L5_LedInit();
    for (;;) {
        LedToggle(Led1);
        osDelay(1000);
    }
}

void taskLcd1602(void const *_) {
    L5_Lcd1602_Init();
    char dis1[] = {"*Hi, I'm lonphy, hello*"};

    for (;;) {
        L5_Lcd1602_WriteString(0, 0, dis1);
        osDelay(1000);

        L5_Lcd1602_WriteString(0, 1, dis1);
        osDelay(2000);

        L5_Lcd1602_ClearScreen();
        osDelay(1000);

    }
}
