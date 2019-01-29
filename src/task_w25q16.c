//
// Created by lonphy on 2019/1/20.
//

#include <lib_l5.h>
#include <stdio.h>
#include "hw.h"

#if defined(L5_USE_W25QXX)

void task_w25q16(const void *arg) {
    hw_spi_init();
    uint8_t *buf = pvPortMalloc(128);
    l5_w25qxx_read(0, buf, 128);
    uint32_t id = l5_w25qxx_read_id();
    printf("id is %lx", id);
    osDelay(osWaitForever);
}

#endif
