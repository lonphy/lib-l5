//
// Created by lonphy on 2018/10/18.
//


#include "lib_l5.h"

#ifdef L5_USE_ITM_CONSOLE

int __unused _write(int fd, const char *ptr, int len) {

    if (fd > 2) {
        return -1;
    }
    for (int i = 0; i < len; ++i) {
        ITM_SendChar((uint32_t) *(ptr + i));
    }

    return len;
}

#endif // L5_USE_ITM_CONSOLE
