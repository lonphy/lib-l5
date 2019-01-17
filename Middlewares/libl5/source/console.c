//
// Created by lonphy on 2018/10/18.
//


#include "lib_l5.h"
#include <stdio.h>
#if defined(L5_USE_ITM_CONSOLE)

int _write(int fd, const char *ptr, int len) {

    if (fd > 2) {
        return -1;
    }
    for (int i = 0; i < len; ++i) {
        ITM_SendChar((uint32_t) *(ptr + i));
    }

    return len;
}

#elif defined(L5_USE_USART_CONSOLE)
extern void hw_log_write(const char *buf, unsigned short len);

int _write(int fd, const char *ptr, int len) {
    hw_log_write(ptr, (unsigned short) len);
    return len;
}

#endif
