//
// Created by lonphy on 2018/10/19.
//

#include "hw_uart.h"

UART_HandleTypeDef hUart1 = {0};

static inline void uartInit(UART_HandleTypeDef *h, USART_TypeDef *usart, uint32_t baudRate, IRQn_Type ir) {
    h->Instance = usart;
    h->Init.BaudRate = baudRate;
    h->Init.Mode = UART_MODE_TX_RX;

    if (HAL_UART_Init(h) != HAL_OK) {
        Error_Handler();
    }

    if (ir == USART1_IRQn) {
        // enable Usart1 interrupt
        HAL_NVIC_SetPriority(ir, 15, 0);
        HAL_NVIC_EnableIRQ(ir);
    }
}

void hw_uart_init() {
    uartInit(&hUart1, USART1, 115200, USART1_IRQn);
}

void HAL_UART_MspInit(UART_HandleTypeDef *hUart) {
    //  static DMA_HandleTypeDef  tx1Dma;
    GPIO_InitTypeDef gpioOpt = {0};
    if (hUart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        gpioOpt.Pin = GPIO_PIN_9;
        gpioOpt.Mode = GPIO_MODE_AF_PP;
        gpioOpt.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &gpioOpt);

        // Not required in other STM32 series, eg F407, H743
        gpioOpt.Pin = GPIO_PIN_10;
        gpioOpt.Mode = GPIO_MODE_INPUT;
        gpioOpt.Pull = GPIO_NOPULL;
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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *hUart) {
    if (hUart->Instance == USART1) {
        L5_UART1_RxCpltCallback(hUart);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *hUart) {
    UNUSED(hUart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *hUart) {
    if (hUart->Instance == USART1) {
        L5_UART1_ErrorCallback(hUart);
    }
}

__weak void L5_UART1_RxCpltCallback(UART_HandleTypeDef *hUart) {
    UNUSED(hUart);
}


__weak void L5_UART1_ErrorCallback(UART_HandleTypeDef *hUart) {
    UNUSED(hUart);
}
