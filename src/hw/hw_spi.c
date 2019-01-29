//
// Created by lonphy on 2019/1/18.
//

#include "hw.h"

void hw_spi_init(void) {
    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    /**SPI1 GPIO Configuration
        PA4   ------> SPI1_NSS
        PA5   ------> SPI1_SCK
        PA6   ------> SPI1_MISO
        PA7   ------> SPI1_MOSI
    */
    MODIFY_REG(GPIOA->CRL, 0xffff0000, 0xbbb30000);
    MODIFY_REG(GPIOA->ODR, 0x00f0, 0x00f0);

    LL_SPI_InitTypeDef opt = {
            .Mode = LL_SPI_MODE_MASTER,
            .TransferDirection = LL_SPI_FULL_DUPLEX,
            .DataWidth = LL_SPI_DATAWIDTH_8BIT,
            /*.BitOrder = LL_SPI_MSB_FIRST, reset value is MSB_FIRST */
            .ClockPhase = LL_SPI_PHASE_2EDGE,
            .ClockPolarity = LL_SPI_POLARITY_HIGH,
            .NSS = LL_SPI_NSS_SOFT,
            .BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV256,
    };
    LL_SPI_Init(SPI1, &opt);
    LL_SPI_Enable(SPI1);
}

uint8_t spi_write_read(SPI_TypeDef *spi, uint8_t dat) {
    while (LL_SPI_IsActiveFlag_TXE(spi) == RESET);
    LL_SPI_TransmitData8(spi, dat);
    while (LL_SPI_IsActiveFlag_RXNE(spi) == RESET);
    return LL_SPI_ReceiveData8(spi);
}