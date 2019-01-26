#include "main.h"
#include <lib_l5.h>

#include "hw_log_uart.h"
#include "hw_wifi_usart.h"
#include "hw_crc.h"
#include "hw_gpio.h"
#include "hw_i2c.h"
#include "hw_spi.h"

void task_lcd12864(void const *arg);

void task_parser(const void *arg);

void task_oled(void const *arg);

void task_wifi(const void *arg);

void task_led(const void *arg);

void task_24c02(const void *arg);

void task_w25q16(const void *arg);

void ll_init(void);

void system_clock_config(void);

void _error_handler(char *file, int line) {
    (void) (file);
    (void) (line);

    while (1) {
        LL_mDelay(1000);
        //l5_led_toggle(Led1);
    }
}

int main(void) {
    ll_init();

    {
        system_clock_config();
        hw_gpio_init();
        /* hw_crc_init(); */
        hw_spi_init();

#if defined(L5_USE_USART_CONSOLE)
        hw_log_usart_init();
#endif
    }
    /*
        osThreadDef(main, task_main, osPriorityNormal, 0, 1024);
        osThreadCreate(osThread(main), NULL);

        osThreadDef(parser, task_parser, osPriorityNormal, 1, 512);
        osThreadCreate(osThread(parser), NULL);
        */
#if defined(L5_USE_OLED)
    osThreadDef(oled, task_oled, osPriorityNormal, 0, 1024);
    osThreadCreate(osThread(oled), NULL);
#endif

#if defined(L5_USE_LED)
    {
        osThreadDef(led, task_led, osPriorityNormal, 1, 512);
        osThreadCreate(osThread(led), NULL);
    }
#endif

#if defined(L5_USE_ESP8266)
    {
        hw_wifi_usart_init();
        osThreadDef(wifi, task_wifi, osPriorityNormal, 1, 4096);
        osThreadCreate(osThread(wifi), NULL);
    }
#endif

#if defined(L5_USE_AT24CXX)
    {
        osThreadDef(at24cxx, task_24c02, osPriorityNormal, 1, 512);
        osThreadCreate(osThread(at24cxx), NULL);
    }
#endif
    osThreadDef(w25q16, task_w25q16, osPriorityNormal, 1, 512);
    osThreadCreate(osThread(w25q16), NULL);

    osKernelStart();
    return 0;
}


void ll_init(void) {
#if defined(STM32F1)
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
#endif

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    L5_NVIC_SetPriority(PendSV_IRQn, 15);
    L5_NVIC_SetPriority(PendSV_IRQn, 15);

#if defined(STM32F1)
    LL_GPIO_AF_Remap_SWJ_NOJTAG();
#endif
}

void SysTick_Handler(void) {
    osSystickHandler();
}

void system_clock_config(void) {

    /* Set FLASH latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    /* Enable HSE oscillator */
    LL_RCC_HSE_Enable();
    while (LL_RCC_HSE_IsReady() != 1);

    /* Main PLL configuration and activation */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
    LL_RCC_PLL_Enable();
    while (LL_RCC_PLL_IsReady() != 1);

    /* Sysclk activation on the main PLL */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    /* Set systick to 1ms in using frequency set to 72MHz */
    LL_Init1msTick(72000000);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(72000000);
}