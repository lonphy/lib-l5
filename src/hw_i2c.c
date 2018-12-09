//
// Created by lonphy on 2018/11/10.
//

#include "hw_i2c.h"


I2C_HandleTypeDef hI2C1;

void hw_i2c_init() {
    hI2C1.Instance = I2C1;
    hI2C1.Init.ClockSpeed = 400000; /* 400K */
    hI2C1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hI2C1.Init.OwnAddress1 = 0x30;
    hI2C1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hI2C1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hI2C1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hI2C1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hI2C1);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *h) {
    GPIO_InitTypeDef gpioOpt;
    if (h->Instance == I2C1) {
        /* *
         * PB6 -> I2C1.SCL
         * PB7 -> I2C1.SDA
         */
        gpioOpt.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        gpioOpt.Mode = GPIO_MODE_AF_OD;
#ifdef STM32F1
        gpioOpt.Speed = GPIO_SPEED_FREQ_HIGH;
#elif STM32F4
        gpioOpt.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpioOpt.Pull = GPIO_PULLUP;
        gpioOpt.Alternate = GPIO_AF4_I2C1;
#endif
        HAL_GPIO_Init(GPIOB, &gpioOpt);

        __HAL_RCC_I2C1_CLK_ENABLE();
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* h)
{

    if(h->Instance==I2C1)
    {
        __HAL_RCC_I2C1_CLK_DISABLE();

        /**I2C1 GPIO Configuration
        PB6     ------> I2C1_SCL
        PB7     ------> I2C1_SDA
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);
    }
}