//
// Created by lonphy on 2018/11/10.
//

#include "mcu.h"
#include "hw_i2c.h"

static inline void hw_send_slave_address(uint8_t address);

void hw_i2c_init() {
    /* Enable the peripheral clock for I2C1 */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    /* PB6(SCL), PB7(SDA) configured with AF Open-drain, HighSpeed */
    MODIFY_REG(GPIOB->CRL, 0xFF000000, 0xFF000000);

    LL_I2C_Disable(I2C1);

    LL_RCC_ClocksTypeDef rcc_clocks;
    LL_RCC_GetSystemClocksFreq(&rcc_clocks);

    /* Configure the SCL Clock Speed */
    LL_I2C_ConfigSpeed(I2C1, rcc_clocks.PCLK1_Frequency, LL_I2C_MAX_SPEED_FAST, LL_I2C_DUTYCYCLE_2);
    LL_I2C_SetOwnAddress1(I2C1, 0x30, LL_I2C_OWNADDRESS1_7BIT);
    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
    LL_I2C_Enable(I2C1);
}

void hw_i2c_write(uint8_t dev_address, uint8_t mem_address, uint8_t dat) {

    hw_send_slave_address(dev_address);

    /* write access address */
    LL_I2C_TransmitData8(I2C1, mem_address);
    while (LL_I2C_IsActiveFlag_BTF(I2C1) == 0);

    /* data to tx reg */
    while (LL_I2C_IsActiveFlag_TXE(I2C1) == 0);
    LL_I2C_TransmitData8(I2C1, dat);
    while (LL_I2C_IsActiveFlag_BTF(I2C1) == 0);


    /* generate End condition */
    LL_I2C_GenerateStopCondition(I2C1);
}

uint8_t hw_i2c_read(uint8_t dev_address, uint8_t mem_address) {

    hw_send_slave_address(dev_address);

    // send address to read
    LL_I2C_TransmitData8(I2C1, mem_address);
    while (LL_I2C_IsActiveFlag_TXE(I2C1) == 0);

    // send start cond again
    LL_I2C_GenerateStartCondition(I2C1);
    while (LL_I2C_IsActiveFlag_SB(I2C1) == 0);

    // send slave address with read
    LL_I2C_TransmitData8(I2C1, (uint8_t) (dev_address | 0x01));
    while (LL_I2C_IsActiveFlag_ADDR(I2C1) == 0);

    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
    __disable_irq();
    LL_I2C_ClearFlag_ADDR(I2C1);
    LL_I2C_GenerateStopCondition(I2C1);
    __enable_irq();
    // read data
    while (LL_I2C_IsActiveFlag_RXNE(I2C1) == 0);
    uint8_t val = LL_I2C_ReceiveData8(I2C1);
    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);

    return val;
}

static inline void hw_send_slave_address(uint8_t address) {
    /* wait until busy flag reset */
    while (LL_I2C_IsActiveFlag_BUSY(I2C1) == 1);

    /* generate Start condition, and wait ack */
    LL_I2C_GenerateStartCondition(I2C1);
    while (LL_I2C_IsActiveFlag_SB(I2C1) == 0);

    /* send slave address, wait ack */
    LL_I2C_TransmitData8(I2C1, address);
    while (LL_I2C_IsActiveFlag_ADDR(I2C1) == 0);

    /* clear address and wait tx empty flag to be clear */
    LL_I2C_ClearFlag_ADDR(I2C1);
    while (LL_I2C_IsActiveFlag_TXE(I2C1) == 0);
}