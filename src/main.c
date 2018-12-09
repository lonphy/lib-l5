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


void system_clock_config(void);
void _error_handler(char *file, int line) {
    UNUSED(file);
    UNUSED(line);

    while (1) {
        HAL_Delay(1000);
        l5_led_toggle(Led1);
    }
}

int main(void) {
    HAL_Init();
    {
        system_clock_config();
        hw_gpio_init();
        /* hw_crc_init(); */
        hw_uart_init();

        l5_led_init();
        l5_led_on(Led1);
        l5_led_on(Led2);

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

        osThreadDef(wifi, task_wifi, osPriorityNormal, 1, 2048);
        osThreadCreate(osThread(wifi), NULL);


    }
    osKernelStart();
    return 0;
}


void HAL_MspInit(void) {
#ifdef STM32F1
    __HAL_RCC_AFIO_CLK_ENABLE();
#endif
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);

#ifdef STM32F1
    __HAL_AFIO_REMAP_SWJ_NOJTAG();
#endif
}

void SysTick_Handler(void) {
    HAL_IncTick();
    osSystickHandler();
}

void system_clock_config(void) {

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

#ifdef STM32F4
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
#endif

    {
        /**Initializes the CPU, AHB and APB busses clocks
        */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
#ifdef STM32F1
        RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
        RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
#elif STM32F4
        RCC_OscInitStruct.PLL.PLLM = 8;
        RCC_OscInitStruct.PLL.PLLN = 336;
        RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
        RCC_OscInitStruct.PLL.PLLQ = 7;
#endif
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
            Error_Handler();
        }
    }
    {
        /**Initializes the CPU, AHB and APB busses clocks
        */
        RCC_ClkInitStruct.ClockType =
                RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
#ifdef STM32F1
        #define FLASH_LATENCY_x FLASH_LATENCY_2
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
#elif STM32F4
#define FLASH_LATENCY_x FLASH_LATENCY_5
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
#endif
        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_x) != HAL_OK) {
            Error_Handler();
        }

    }
    /**Configure the Systick interrupt time
    */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

    /**Configure the Systick
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}