//
// Created by lonphy on 2018/10/19.
//

#include "hw_uart.h"

UART_HandleTypeDef hUart1;

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

    /* DMA interrupt configuration */
//    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 1);
//    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

void HAL_UART_MspInit(UART_HandleTypeDef *hUart) {
    //  static DMA_HandleTypeDef  tx1Dma;
    GPIO_InitTypeDef gpioOpt;
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

        // config Tx DMA
//        tx1Dma.Instance = DMA1_Channel4;
//        tx1Dma.Init.Direction = DMA_MEMORY_TO_PERIPH;
//        tx1Dma.Init.PeriphInc = DMA_PINC_DISABLE;
//        tx1Dma.Init.MemInc = DMA_MINC_ENABLE;
//        tx1Dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//        tx1Dma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//        tx1Dma.Init.Mode = DMA_NORMAL;
//        tx1Dma.Init.Priority = DMA_PRIORITY_MEDIUM;
//
//
//        if ( HAL_DMA_Init(&tx1Dma) != HAL_OK ) {
//            Error_Handler();
//        }
//
//        __HAL_LINKDMA(&hUart1, hdmatx, tx1Dma);
    } else if (hUart->Instance == USART2) {
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *hUart) {
    if (hUart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    } else if (hUart->Instance == USART2) {
        __HAL_RCC_USART1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);
        HAL_NVIC_DisableIRQ(USART2_IRQn);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *hUart) {
    if (hUart->Instance == USART1) {
        L5_UART1_RxCpltCallback(hUart);
    } else if (hUart->Instance == USART2) {
        L5_UART2_RxCpltCallback(hUart);
    } else if (hUart->Instance == USART3) {
        L5_UART3_RxCpltCallback(hUart);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *hUart) {
    UNUSED(hUart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *hUart) {
    if (hUart->Instance == USART1) {
        L5_UART1_ErrorCallback(hUart);
    } else if (hUart->Instance == USART2) {
        L5_UART2_ErrorCallback(hUart);
    } else if (hUart->Instance == USART3) {
        L5_UART3_ErrorCallback(hUart);
    }
}

__weak void L5_UART1_RxCpltCallback(UART_HandleTypeDef *hUart) {
    UNUSED(hUart);
}

__weak void L5_UART2_RxCpltCallback(UART_HandleTypeDef *hUart) {
    UNUSED(hUart);
}

__weak void L5_UART3_RxCpltCallback(UART_HandleTypeDef *hUart) {
    UNUSED(hUart);
}

__weak void L5_UART1_ErrorCallback(UART_HandleTypeDef *hUart) {
    UNUSED(hUart);
}

__weak void L5_UART2_ErrorCallback(UART_HandleTypeDef *hUart) {
    UNUSED(hUart);
}

__weak void L5_UART3_ErrorCallback(UART_HandleTypeDef *hUart) {
    UNUSED(hUart);
}
