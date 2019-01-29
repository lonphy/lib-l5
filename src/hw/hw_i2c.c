//
// Created by lonphy on 2018/11/10.
//

#include "hw.h"

static inline void hw_send_slave_address(I2C_TypeDef *i2c, uint8_t address);

static inline void hw_i2c_write(I2C_TypeDef *i2c, uint8_t dev_address, uint8_t mem_address, uint8_t dat);

static inline uint8_t hw_i2c_read(I2C_TypeDef *i2c, uint8_t dev_address, uint8_t mem_address);

void hw_i2c_init(I2C_TypeDef *i2c, uint8_t addr1) {
    if (i2c == I2C1) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
        /* PB6(SCL), PB7(SDA) configured with AF Open-drain, HighSpeed */
        MODIFY_REG(GPIOB->CRL, 0xFF000000, 0xFF000000);
    } else if (i2c == I2C2) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
        /* PB10(SCL), PB11(SDA) configured with AF Open-drain, HighSpeed */
        MODIFY_REG(GPIOB->CRH, 0x0000FF00, 0x0000FF00);
    }

    LL_I2C_Disable(i2c);

    LL_RCC_ClocksTypeDef rcc_clocks;
    LL_RCC_GetSystemClocksFreq(&rcc_clocks);

    /* Configure the SCL Clock Speed */
    LL_I2C_ConfigSpeed(i2c, rcc_clocks.PCLK1_Frequency, LL_I2C_MAX_SPEED_FAST, LL_I2C_DUTYCYCLE_2);
    LL_I2C_SetOwnAddress1(i2c, addr1, LL_I2C_OWNADDRESS1_7BIT);
    LL_I2C_AcknowledgeNextData(i2c, LL_I2C_ACK);
    LL_I2C_Enable(i2c);
}

static inline void hw_i2c_write(I2C_TypeDef *i2c, uint8_t dev_address, uint8_t mem_address, uint8_t dat) {

    hw_send_slave_address(i2c, dev_address);

    /* write access address */
    LL_I2C_TransmitData8(i2c, mem_address);
    while (LL_I2C_IsActiveFlag_BTF(i2c) == 0);

    /* data to tx reg */
    while (LL_I2C_IsActiveFlag_TXE(i2c) == 0);
    LL_I2C_TransmitData8(i2c, dat);
    while (LL_I2C_IsActiveFlag_BTF(i2c) == 0);


    /* generate End condition */
    LL_I2C_GenerateStopCondition(i2c);
}

static inline uint8_t hw_i2c_read(I2C_TypeDef *i2c, uint8_t dev_address, uint8_t mem_address) {

    hw_send_slave_address(i2c, dev_address);

    // send address to read
    LL_I2C_TransmitData8(i2c, mem_address);
    while (LL_I2C_IsActiveFlag_TXE(i2c) == 0);

    // send start cond again
    LL_I2C_GenerateStartCondition(i2c);
    while (LL_I2C_IsActiveFlag_SB(i2c) == 0);

    // send slave address with read
    LL_I2C_TransmitData8(i2c, (uint8_t) (dev_address | 0x01));
    while (LL_I2C_IsActiveFlag_ADDR(i2c) == 0);

    LL_I2C_AcknowledgeNextData(i2c, LL_I2C_NACK);
    __disable_irq();
    LL_I2C_ClearFlag_ADDR(i2c);
    LL_I2C_GenerateStopCondition(i2c);
    __enable_irq();
    // read data
    while (LL_I2C_IsActiveFlag_RXNE(i2c) == 0);
    uint8_t val = LL_I2C_ReceiveData8(i2c);
    LL_I2C_AcknowledgeNextData(i2c, LL_I2C_ACK);

    return val;
}

static inline void hw_send_slave_address(I2C_TypeDef *i2c, uint8_t address) {
    /* wait until busy flag reset */
    while (LL_I2C_IsActiveFlag_BUSY(i2c) == 1);

    /* generate Start condition, and wait ack */
    LL_I2C_GenerateStartCondition(i2c);
    while (LL_I2C_IsActiveFlag_SB(i2c) == 0);

    /* send slave address, wait ack */
    LL_I2C_TransmitData8(i2c, address);
    while (LL_I2C_IsActiveFlag_ADDR(i2c) == 0);

    /* clear address and wait tx empty flag to be clear */
    LL_I2C_ClearFlag_ADDR(i2c);
    while (LL_I2C_IsActiveFlag_TXE(i2c) == 0);
}

void at24cxx_i2c_write(I2C_TypeDef *i2c, uint8_t dev_address, uint8_t mem_address, uint8_t dat) {
    hw_i2c_write(i2c, dev_address, mem_address, dat);
}

uint8_t at24cxx_i2c_read(I2C_TypeDef *i2c, uint8_t dev_address, uint8_t mem_address) {
    return hw_i2c_read(i2c, dev_address, mem_address);
}

void oled_i2c_write(I2C_TypeDef *i2c, uint8_t dev_address, uint8_t mem_address, uint8_t dat) {
    hw_i2c_write(i2c, dev_address, mem_address, dat);
}