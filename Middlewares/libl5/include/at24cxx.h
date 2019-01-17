//
// Created by lonphy on 2019/1/17.
//

#ifndef __LIB_L5_AT24CXX_H__
#define __LIB_L5_AT24CXX_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "mcu.h"

/**
 * Write one byte to specify address
 *
 * @param dev_address - slave address for AT24CXX with shift
 * @param mem_address - access address for AT24CXX
 * @param dat
 */
extern void hw_i2c_write(uint8_t dev_address, uint8_t mem_address, uint8_t dat);

/**
 * Read one byte from specify address
 *
 * @param dev_address - slave address for AT24CXX with shift
 * @param mem_address - access address for AT24CXX
 * @param dat
 */
extern uint8_t hw_i2c_read(uint8_t dev_address, uint8_t mem_address);

void l5_at24cxx_write(uint8_t mem_address, uint8_t dat);
uint8_t l5_at24cxx_read(uint8_t mem_address);

#ifdef __cplusplus
}
#endif

#endif //__LIB_L5_AT24CXX_H__
