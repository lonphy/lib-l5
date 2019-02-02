//
// Created by lonphy on 2018/10/19.
//

#include "hw.h"
#include <lib_l5.h>

#if defined(L5_USE_ESP8266) || defined(L5_MUSIC_DEMO)

#if defined(STM32F1)
#define UART_DMA                 DMA2

#define Tx_DMA_Ch                LL_DMA_CHANNEL_5
#define Tx_DMA_IRQHandler        hw_DMA2_Channel5_IRQHandler
#define Tx_DMA_IRQn              DMA2_Channel4_5_IRQn

#define Rx_DMA_Ch                LL_DMA_CHANNEL_3
#define Rx_DMA_IRQHandler        DMA2_Channel3_IRQHandler
#define Rx_DMA_IRQn              DMA2_Channel3_IRQn

static inline void Rx_DMA_Off() {
    LL_DMA_ClearFlag_GI3(UART_DMA);
    LL_USART_DisableDMAReq_RX(WIFI_USART);
    LL_DMA_DisableChannel(UART_DMA, Rx_DMA_Ch);
}

static inline void Tx_DMA_Off() {
    LL_DMA_ClearFlag_GI5(UART_DMA);
    LL_USART_DisableDMAReq_TX(WIFI_USART);
    LL_DMA_DisableChannel(UART_DMA, Tx_DMA_Ch);
}

static inline void Rx_DMA_On() {
    LL_USART_EnableDMAReq_RX(WIFI_USART);
    LL_DMA_EnableChannel(UART_DMA, Rx_DMA_Ch);
}

static inline void Tx_DMA_On() {
    LL_USART_EnableDMAReq_TX(WIFI_USART);
    LL_DMA_EnableChannel(UART_DMA, Tx_DMA_Ch);
}

#elif defined(STM32F4)
#define UART_DMA                 DMA1

#define Tx_DMA_Ch                LL_DMA_STREAM_4
#define WIFI_Tx_DMA_IRQn         DMA1_Stream4_IRQn
#define Tx_DMA_IRQHandler        DMA1_Stream4_IRQHandler

#define Rx_DMA_Ch           LL_DMA_STREAM_2
#define Rx_DMA_IRQn         DMA1_Stream2_IRQn
#define Rx_DMA_IRQHandler   DMA1_Stream2_IRQHandler
#endif

static inline void hw_uart_dma_init() {

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

    {/* Configure the DMA for UART Tx */
        LL_DMA_ConfigTransfer(UART_DMA, Tx_DMA_Ch,
                              LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                              LL_DMA_PRIORITY_MEDIUM |
                              LL_DMA_MODE_NORMAL |
                              LL_DMA_PERIPH_NOINCREMENT |
                              LL_DMA_MEMORY_INCREMENT |
                              LL_DMA_PDATAALIGN_BYTE |
                              LL_DMA_MDATAALIGN_BYTE);

        L5_NVIC_EnableIRQ(Tx_DMA_IRQn, 14);
    }

    {/* Configure the DMA for UART Rx */
        LL_DMA_ConfigTransfer(UART_DMA, Rx_DMA_Ch,
                              LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
                              LL_DMA_PRIORITY_VERYHIGH |
                              LL_DMA_MODE_NORMAL |
                              LL_DMA_PERIPH_NOINCREMENT |
                              LL_DMA_MEMORY_INCREMENT |
                              LL_DMA_PDATAALIGN_BYTE |
                              LL_DMA_MDATAALIGN_BYTE);
        L5_NVIC_EnableIRQ(Rx_DMA_IRQn, 14);
    }
    {
        LL_DMA_EnableIT_TC(UART_DMA, Rx_DMA_Ch);
        LL_DMA_EnableIT_TE(UART_DMA, Rx_DMA_Ch);

        LL_DMA_EnableIT_TC(UART_DMA, Tx_DMA_Ch);
        LL_DMA_EnableIT_TE(UART_DMA, Tx_DMA_Ch);
    }
}

void hw_wifi_usart_init() {
    /* enable uart and it's DMA clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);

    {/* uart gpio pin config */

        // MODIFY_REG(WIFI_GPIO->CRH, 0x0000ff00, 0x00008B00);
        /* Tx : Alternate function, High Speed, Push pull, Pull up */
        LL_GPIO_SetPinMode(WIFI_GPIO, WIFI_Tx, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetPinOutputType(WIFI_GPIO, WIFI_Tx, LL_GPIO_OUTPUT_PUSHPULL);
        LL_GPIO_SetPinSpeed(WIFI_GPIO, WIFI_Tx, LL_GPIO_SPEED_FREQ_HIGH);

        /* Rx : Alternate function, High Speed, Push pull, Pull up */
        LL_GPIO_SetPinMode(WIFI_GPIO, WIFI_Tx, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetPinMode(WIFI_GPIO, WIFI_Rx, LL_GPIO_MODE_FLOATING);
        LL_GPIO_SetPinPull(WIFI_GPIO, WIFI_Rx, LL_GPIO_PULL_UP);

    }

    hw_uart_dma_init();
    L5_NVIC_EnableIRQ(UART4_IRQn, 15);

    {/* uart parameter */
        LL_USART_InitTypeDef uartOpt = {
                .BaudRate = WIFI_BAUD_RATE,
                .DataWidth = LL_USART_DATAWIDTH_8B,
                .StopBits = LL_USART_STOPBITS_1,
                .Parity = LL_USART_PARITY_NONE,
                .TransferDirection = LL_USART_DIRECTION_TX_RX,
        };
        LL_USART_Init(WIFI_USART, &uartOpt);
        LL_USART_ConfigAsyncMode(WIFI_USART);
        LL_USART_EnableIT_IDLE(WIFI_USART);
        LL_USART_Enable(WIFI_USART);
    }
}

uint32_t hw_usart_get_dma_rx_length() {
    return LL_DMA_GetDataLength(UART_DMA, Rx_DMA_Ch);
}

/* start uart tx */
void hw_usart_start_dma_tx(void *buf, uint32_t len) {
    LL_DMA_ClearFlag_GI5(UART_DMA);

    LL_DMA_ConfigAddresses(UART_DMA, Tx_DMA_Ch,
                           (uint32_t) buf,
                           LL_USART_DMA_GetRegAddr(WIFI_USART),
                           LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    LL_DMA_SetDataLength(UART_DMA, Tx_DMA_Ch, len);

    Tx_DMA_On();
}

/* start uart rx */
void hw_usart_start_dma_rx(void *buf, uint32_t len) {
    LL_DMA_ClearFlag_GI3(UART_DMA);

    LL_DMA_ConfigAddresses(UART_DMA, Rx_DMA_Ch,
                           LL_USART_DMA_GetRegAddr(WIFI_USART),
                           (uint32_t) buf,
                           LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    LL_DMA_SetDataLength(UART_DMA, Rx_DMA_Ch, len);

    Rx_DMA_On();
}


void Rx_DMA_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_TC3(UART_DMA)) {
        Rx_DMA_Off();
        L5_wifi_rx_receive(wifi_ok);
    } else if (LL_DMA_IsActiveFlag_TE3(UART_DMA)) {
        Rx_DMA_Off();
        L5_wifi_rx_receive(wifi_ok);
    }
}

void Tx_DMA_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_TC5(UART_DMA)) {
        Tx_DMA_Off();
        l5_wifi_tx_complete(wifi_ok);
    } else if (LL_DMA_IsActiveFlag_TE5(UART_DMA)) {
        Tx_DMA_Off();
        l5_wifi_tx_complete(wifi_tx_error);
    }
}

void UART4_IRQHandler(void) {
    if (LL_USART_IsActiveFlag_IDLE(WIFI_USART) && LL_USART_IsEnabledIT_IDLE(WIFI_USART)) {
        LL_USART_ClearFlag_IDLE(WIFI_USART);
        /* stop wifi rx dma */
        CLEAR_BIT(WIFI_USART->CR3, USART_CR3_EIE);
        Rx_DMA_Off();
        L5_wifi_rx_receive(wifi_ok);
    }
}

#endif