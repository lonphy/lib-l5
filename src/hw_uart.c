//
// Created by lonphy on 2018/10/19.
//

#include "hw_uart.h"
#include <lib_l5.h>

#if defined(STM32F4)
#elif defined(STM32F1)
#define WIFI_UART_IRQn           UART4_IRQn
#define WIFI_UART_IRQHandler     UART4_IRQHandler

#define WIFI_DMA                 DMA2

#define WIFI_Tx_DMA_Chx          LL_DMA_CHANNEL_5
#define WIFI_Tx_DMA_IRQHandler   DMA2_Channel4_5_IRQHandler
#define WIFI_Tx_DMA_IRQn         DMA2_Channel4_5_IRQn

#define WIFI_Rx_DMA_Chx          LL_DMA_CHANNEL_3
#define WIFI_Rx_DMA_IRQHandler   DMA2_Channel3_IRQHandler
#define WIFI_Rx_DMA_IRQn         DMA2_Channel3_IRQn
#endif

void hw_uart_init() {
    /* enable uart and it's DMA clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

    /* uart gpio pin config */
    {
        /* Configure Tx Pin as : Alternate function, High Speed, Push pull, Pull up */
        LL_GPIO_SetPinMode(WIFI_GPIO, WIFI_Tx, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetPinSpeed(WIFI_GPIO, WIFI_Tx, LL_GPIO_SPEED_FREQ_HIGH);
        LL_GPIO_SetPinOutputType(WIFI_GPIO, WIFI_Tx, LL_GPIO_OUTPUT_PUSHPULL);
        LL_GPIO_SetPinPull(WIFI_GPIO, WIFI_Tx, LL_GPIO_PULL_UP);

        /* Configure Rx Pin as : Alternate function, High Speed, Push pull, Pull up */
        LL_GPIO_SetPinMode(WIFI_GPIO, WIFI_Rx, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetPinSpeed(WIFI_GPIO, WIFI_Rx, LL_GPIO_SPEED_FREQ_HIGH);
        LL_GPIO_SetPinOutputType(WIFI_GPIO, WIFI_Rx, LL_GPIO_MODE_INPUT);
    }

    /* uart parameter */
    {
        LL_USART_Disable(WIFI_USART);
        LL_USART_ConfigCharacter(WIFI_USART, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
        LL_USART_SetTransferDirection(WIFI_USART, LL_USART_DIRECTION_TX_RX);
        LL_USART_SetBaudRate(WIFI_USART, SystemCoreClock / 2, WIFI_BAUD_RATE);
    }

    {
        NVIC_SetPriority(WIFI_Rx_DMA_IRQn, 14);
        NVIC_EnableIRQ(WIFI_Rx_DMA_IRQn);

        NVIC_SetPriority(WIFI_Tx_DMA_IRQn, 14);
        NVIC_EnableIRQ(WIFI_Tx_DMA_IRQn);

        LL_USART_EnableIT_IDLE(WIFI_USART);
        NVIC_SetPriority(WIFI_UART_IRQn, 15);
        NVIC_EnableIRQ(WIFI_UART_IRQn);
    }

    {
        /* Configure the DMA for UART Tx */
        LL_DMA_ConfigTransfer(WIFI_DMA, WIFI_Tx_DMA_Chx,
                              LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                              LL_DMA_PRIORITY_HIGH |
                              LL_DMA_MODE_NORMAL |
                              LL_DMA_PERIPH_NOINCREMENT |
                              LL_DMA_MEMORY_INCREMENT |
                              LL_DMA_PDATAALIGN_BYTE |
                              LL_DMA_MDATAALIGN_BYTE);
    }

    {
        /* Configure the DMA for UART Rx */
        LL_DMA_ConfigTransfer(WIFI_DMA, WIFI_Rx_DMA_Chx,
                              LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
                              LL_DMA_PRIORITY_MEDIUM |
                              LL_DMA_MODE_NORMAL |
                              LL_DMA_PERIPH_NOINCREMENT |
                              LL_DMA_MEMORY_INCREMENT |
                              LL_DMA_PDATAALIGN_BYTE |
                              LL_DMA_MDATAALIGN_BYTE);


        /* (5) Enable DMA transfer complete/error interrupts  */
        LL_DMA_EnableIT_TC(WIFI_DMA, WIFI_Rx_DMA_Chx);
        LL_DMA_EnableIT_TE(WIFI_DMA, WIFI_Rx_DMA_Chx);

        LL_DMA_EnableIT_TC(WIFI_DMA, WIFI_Tx_DMA_Chx);
        LL_DMA_EnableIT_TE(WIFI_DMA, WIFI_Tx_DMA_Chx);
    }

    LL_USART_Enable(WIFI_USART);
}

uint32_t hw_usart_get_dma_rx_length() {
    return LL_DMA_GetDataLength(WIFI_DMA, WIFI_Rx_DMA_Chx);
}

void hw_usart_start_dma_tx(void *buf, uint32_t len) {
    LL_DMA_ClearFlag_GI4(WIFI_DMA); // TODO:

    LL_DMA_ConfigAddresses(WIFI_DMA, WIFI_Tx_DMA_Chx,
                           LL_USART_DMA_GetRegAddr(WIFI_USART),
                           (uint32_t) buf,
                           LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    LL_DMA_SetDataLength(WIFI_DMA, WIFI_Tx_DMA_Chx, len);

    LL_DMA_EnableChannel(WIFI_DMA, WIFI_Tx_DMA_Chx);
    LL_USART_EnableDMAReq_RX(WIFI_USART);
}

void hw_usart_start_dma_rx(void *buf, uint32_t len) {
    LL_USART_DisableDMAReq_RX(WIFI_USART);

    LL_DMA_ClearFlag_GI3(WIFI_DMA); // TODO:

    LL_DMA_ConfigAddresses(WIFI_DMA, WIFI_Rx_DMA_Chx,
                           LL_USART_DMA_GetRegAddr(WIFI_USART),
                           (uint32_t) buf,
                           LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    LL_DMA_SetDataLength(WIFI_DMA, WIFI_Rx_DMA_Chx, len);

    LL_DMA_EnableChannel(WIFI_DMA, WIFI_Rx_DMA_Chx);

    LL_USART_EnableDMAReq_RX(WIFI_USART);
}

void WIFI_Rx_DMA_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_TC3(WIFI_DMA)) {
        LL_DMA_ClearFlag_GI3(WIFI_DMA);
        L5_rx_receive(wifi_ok);
    } else if (LL_DMA_IsActiveFlag_TE3(WIFI_DMA)) {
        L5_rx_receive(wifi_rx_error);
    }
}

void WIFI_Tx_DMA_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_TC5(WIFI_DMA)) {
        LL_DMA_ClearFlag_GI5(WIFI_DMA);
        LL_USART_DisableDMAReq_TX(WIFI_USART);
        l5_tx_complete(wifi_ok);
    } else if (LL_DMA_IsActiveFlag_TE5(WIFI_DMA)) {
        LL_USART_DisableDMAReq_TX(WIFI_USART);
        l5_tx_complete(wifi_tx_error);
    }
}

void WIFI_UART_IRQHandler(void) {
    if (LL_USART_IsActiveFlag_IDLE(WIFI_USART) && LL_USART_IsEnabledIT_IDLE(WIFI_USART)) {
        LL_USART_ClearFlag_IDLE(WIFI_USART);

        LL_USART_DisableDMAReq_RX(WIFI_USART);

        /* stop wifi rx dma */
        CLEAR_BIT(WIFI_USART->CR3, USART_CR3_EIE);
        L5_rx_receive(wifi_ok);
    }
}