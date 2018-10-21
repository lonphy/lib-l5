//
// Created by lonphy on 2018/10/19.
//

#include "uart.h"

UART_HandleTypeDef hUart1;
static uint8_t uart1RxTmp = 0;

void UartInit() {
    hUart1.Instance = USART1;
    hUart1.Init.BaudRate = 115200;
    hUart1.Init.WordLength = UART_WORDLENGTH_8B;
    hUart1.Init.StopBits = UART_STOPBITS_1;
    hUart1.Init.Parity = UART_PARITY_NONE;
    hUart1.Init.Mode = UART_MODE_TX_RX;
    hUart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hUart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&hUart1) != HAL_OK) {
        Error_Handler();
    }

    // 开启Usart1 中断
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);

    //  开启接收中断
    if (HAL_UART_Receive_IT(&hUart1, &uart1RxTmp, 1) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef *hUart) {
    GPIO_InitTypeDef gpioOpt;
    if (hUart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        gpioOpt.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        gpioOpt.Mode = GPIO_MODE_AF_PP;
        gpioOpt.Pull = GPIO_NOPULL;
        gpioOpt.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &gpioOpt);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *hUart) {
    if (hUart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
}
