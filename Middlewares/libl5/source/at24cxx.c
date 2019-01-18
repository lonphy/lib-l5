//
// Created by lonphy on 2019/1/17.
//

#include "lib_l5.h"

#if defined(L5_USE_AT24CXX)

#if !defined(AT24CXX_ADDRESS)
#error AT24CXX_ADDRESS not configured
#endif

void l5_at24cxx_write(uint8_t mem_address, uint8_t dat) {
    at24cxx_i2c_write(AT24CXX_I2C, AT24CXX_ADDRESS, mem_address, dat);
}

uint8_t l5_at24cxx_read(uint8_t mem_address) {
    return at24cxx_i2c_read(AT24CXX_I2C, AT24CXX_ADDRESS, mem_address);
}

#endif