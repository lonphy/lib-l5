//
// Created by lonphy on 2019/1/16.
//

#include <lib_l5.h>
#include "hw_i2c.h"

#if defined(L5_USE_AT24CXX)
#define total_bytes (256)  /* total size = (2 * 1024)bits  = (2048 / 8)Bytes = 256Bytes */

static uint8_t buf[total_bytes] = {0};

void task_24c02(const void *arg) {
    hw_i2c_init(AT24CXX_I2C, 0x30);

//    for (int i = 0; i < total_bytes; ++i) {
//        l5_at24cxx_write((uint8_t) i, (uint8_t) i);
//        osDelay(10);
//    }

    for (int j = 0; j < total_bytes; ++j) {
        buf[j] = l5_at24cxx_read(j);
        osDelay(10);
    }

    osDelay(osWaitForever);
}

#endif
