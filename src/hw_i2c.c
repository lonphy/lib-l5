//
// Created by lonphy on 2018/11/10.
//

#include "hw_i2c.h"

void hw_i2c_init() {

    /* Enable the I2C1 peripheral clock *************************************/

    /* Enable the peripheral clock for I2C1 */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    /* Configure SCL Pin as : Alternate function, High Speed, Open drain, Pull up */
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);

    /* Configure SDA Pin as : Alternate function, High Speed, Open drain, Pull up */
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);

    /* Configure Event IT:
       *  - Set priority for I2C1_EV_IRQn
       *  - Enable I2C1_EV_IRQn
       */
    // NVIC_SetPriority(I2C1_EV_IRQn, 0);
    // NVIC_EnableIRQ(I2C1_EV_IRQn);

    /* Configure Error IT:
     *  - Set priority for I2C1_ER_IRQn
     *  - Enable I2C1_ER_IRQn
     */
    // NVIC_SetPriority(I2C1_ER_IRQn, 0);
    // NVIC_EnableIRQ(I2C1_ER_IRQn);

    LL_RCC_ClocksTypeDef rcc_clocks;
    LL_RCC_GetSystemClocksFreq(&rcc_clocks);

    /* Configure the SCL Clock Speed */
    LL_I2C_ConfigSpeed(I2C1, rcc_clocks.PCLK1_Frequency, 400000/* 400K */, LL_I2C_DUTYCYCLE_2);

    /* Configure the Own Address1                   */
    /* Reset Values of :
     *     - OwnAddress1 is 0x30
     *     - OwnAddrSize is LL_I2C_OWNADDRESS1_7BIT
     */
    LL_I2C_SetOwnAddress1(I2C1, 0x30, LL_I2C_OWNADDRESS1_7BIT);
}
