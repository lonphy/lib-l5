#include "main.h"
#include <lib_l5.h>

#include "hw_uart.h"
#include "hw_crc.h"
#include "hw_gpio.h"
#include "hw_i2c.h"


void task_lcd12864(void const *arg);

void task_parser(const void *arg);

void task_oled(void const *arg);

void task_wifi(const void *arg);

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
        hw_uart_init();

        //l5_led_init();
        //l5_led_on(Led1);

        /* hw_i2c_init(); */
    }
    {
        /*
            osThreadDef(main, task_main, osPriorityNormal, 0, 1024);
            osThreadCreate(osThread(main), NULL);


            osThreadDef(parser, task_parser, osPriorityNormal, 1, 512);
            osThreadCreate(osThread(parser), NULL);

            osThreadDef(oled, task_oled, osPriorityNormal, 0, 1024);
            osThreadCreate(osThread(oled), NULL);
         */

        osThreadDef(wifi, task_wifi, osPriorityNormal, 1, 4096);
        osThreadCreate(osThread(wifi), NULL);
    }
    osKernelStart();
    return 0;
}


void ll_init(void) {
#ifdef STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
#endif

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

#ifdef STM32F1
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

    /* Set APB1 & APB2 prescaler*/
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    /* Set systick to 1ms in using frequency set to 72MHz */
    LL_Init1msTick(72000000);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(72000000);
}