//
// Created by lonphy on 2018/11/10.
//

#ifndef RTOS_I2C_H
#define RTOS_I2C_H

#include <mcu.h>

void hw_i2c_init(I2C_TypeDef *i2c, uint8_t addr1);

#endif //RTOS_I2C_H
