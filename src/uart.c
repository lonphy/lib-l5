//
// Created by lonphy on 2018/10/19.
//

#include "uart.h"

UART_HandleTypeDef hUart1;

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

    // enable Usart1 interrupt
    HAL_NVIC_SetPriority(USART1_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

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

int __unused _write(int fd, char *ptr, int len) {
    if (fd > 2) {
        return -1;
    }

    if (HAL_UART_Transmit(&hUart1, (uint8_t *) ptr, (uint16_t) len, 0xffff) != HAL_OK) {
        Error_Handler();
    }

    // DMA
//   if ( HAL_UART_Transmit_DMA(&hUart1, (uint8_t *) ptr, (uint16_t) len ) != HAL_OK ) {
//        Error_Handler();
//   }
//   while (HAL_UART_GetState(&hUart1) != HAL_UART_STATE_READY);
    return len;
}
