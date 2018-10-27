//
// Created by lonphy on 2018/10/27.
//

#ifndef RTOS_RINGQUEUE_H
#define RTOS_RINGQUEUE_H
#include <cmsis_os.h>

void rqWrite(uint8_t dat);
osStatus rqRead(uint8_t * buf );

#endif //RTOS_RINGQUEUE_H
