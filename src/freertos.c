#include <FreeRTOS.h>
#include <cmsis_os.h>
#include <task.h>
#include <string.h>
#include <lib_l5.h>

#include "main.h"
#include "uart.h"

osThreadId ledHandler,
        lcd1602Handler,
        lcd12864Handler;


void taskLed(void const *arg);

void taskLcd1602(void const *arg);

void taskLcd12864(void const *arg);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

void MX_FREERTOS_Init(void) {

#ifdef L5_USE_LED
    osThreadDef(led, taskLed, osPriorityNormal, 0, 64);
    ledHandler = osThreadCreate(osThread(led), NULL);
#endif

#ifdef L5_USE_LCD1602
    osThreadDef(lcd1602, taskLcd1602, osPriorityNormal, 0, 512);
    lcd1602Handler = osThreadCreate(osThread(lcd1602), NULL);
#endif

#ifdef L5_USE_LCD12864
    osThreadDef(lcd12864, taskLcd12864, osPriorityNormal, 0, 512);
    lcd12864Handler = osThreadCreate(osThread(lcd12864), NULL);
#endif

}

#ifdef L5_USE_LED

void taskLed(void const *arg) {
    L5_LedInit();
    for (;;) {
        LedToggle(Led1);
        osDelay(1000);
    }
}

#endif

#ifdef L5_USE_LCD1602
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
#endif

#ifdef L5_USE_LCD12864

void taskLcd12864(void const *arg) {
    L5_Lcd12864Init();

    // my lcd12864's screen is reverse.
    L5_Lcd12864SetPageFromBottom2Top();
    L5_Lcd12864ClearScreen();

    uint8_t i, j;
    for (;;) {
        // loop 8 page, max 128/7=18 char pre line

        L5_Lcd12864WriteString(0, 0, "0123456789+-*/!@$%");
        L5_Lcd12864WriteString(0, 1, "^&*()_=[]{}:\";'<>,");
        L5_Lcd12864WriteString(0, 2, ".?/\\| abcdefghijkl");
        L5_Lcd12864WriteString(0, 3, "mnopqrstuvwxyzABCD");
        L5_Lcd12864WriteString(0, 4, "EFGHIJKLMNOPQRSTUV");

        for (i = 5; i < 8; i++) {
            for (j = 0; j < 5; ++j) {
                L5_Lcd12864WriteString(j*4, i, "WXYZ");
            }
        }

        osDelay(1000);
        L5_Lcd12864SetPageFromTop2Bottom();
        osDelay(3000);
        L5_Lcd12864ClearScreen();
        L5_Lcd12864SetPageFromBottom2Top();


        for ( i = 0; i < 8; ++i) {
            L5_Lcd12864SetPage(i);
            L5_Lcd12864SetColumn(0);
            for ( j = 0; j < 64; ++j) {
                L5_Lcd12864WriteData(0x55);
                L5_Lcd12864WriteData(0xaa);
                osDelay(20);
            }
        }
        osDelay(800);
    }
}

#endif