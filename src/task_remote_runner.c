//
// Created by lonphy on 2018/12/16.
//

#include <lib_l5.h>
#include <string.h>

#ifdef L5_USE_ESP8266
#define runner_log(f, ...) printf("[runner] " f "\n", ##__VA_ARGS__)

void task_remote_runner(__unused void const *arg) {

    char * buf;
    uint16_t size;
    wifi_err_t err;

    while (1) {
        err = l5_tcp_read((void **) &buf, &size, osWaitForever);
        if (err == wifi_timeout) {
            continue;
        }

        l5_tcp_write(buf, size);

        // runner_log("received data from server:(%d) %s", size, buf);
        vPortFree(buf);
    }
}

#endif