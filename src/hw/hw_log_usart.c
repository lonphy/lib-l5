//
// Created by lonphy on 2019/1/13.
//
#include <hw.h>

#define LOG_GPIO       GPIOA
#define LOG_BAUD_RATE  230400
#define LOG_USART      USART1

void hw_log_usart_init(void) {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

    {/* uart Tx pin(PA9) config */
        /* Tx : Alternate function, High Speed, Push pull, Pull up */
        LOG_GPIO->CRH &= 0xffffff0f;
        LOG_GPIO->CRH |= 0x000000b0;
    }
    {/* uart parameter */
        LL_USART_InitTypeDef uartOpt = {
                .BaudRate = LOG_BAUD_RATE,
                .DataWidth = LL_USART_DATAWIDTH_8B,
                .StopBits = LL_USART_STOPBITS_1,
                .Parity = LL_USART_PARITY_NONE,
                .TransferDirection = LL_USART_DIRECTION_TX,
        };
        LL_USART_Init(LOG_USART, &uartOpt);
        LL_USART_ConfigAsyncMode(LOG_USART);
        LL_USART_Enable(LOG_USART);
    }
}

void hw_log_write(const char *buf, unsigned short len) {
    uint8_t *p = (uint8_t *) buf;
    unsigned short i = 0;
    for (i = 0; i < len && *p; ++i, ++p) {
        while (!LL_USART_IsActiveFlag_TXE(LOG_USART));
        LL_USART_TransmitData8(LOG_USART, *p);
    }
}