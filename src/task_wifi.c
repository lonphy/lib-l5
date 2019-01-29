//
// Created by lonphy on 2018/10/28.
//
#include <lib_l5.h>
#include <string.h>
#include <stdio.h>
#include "hw.h"

#if defined(L5_WIFI_DEMO)

#define wifi_log(f, ...) printf("[wifi] " f "\n", ##__VA_ARGS__)

void task_wifi(__unused void const *arg) {

    wifi_err_t err;
    wifi_config_t opt = {
            .rx_timeout = WIFI_RX_TIMEOUT,
            .tx_timeout = WIFI_TX_TIMEOUT,
            .start_rx_func = hw_usart_start_dma_rx,
            .start_tx_func = hw_usart_start_dma_tx,
            .get_rx_length_func = hw_usart_get_dma_rx_length,
    };
    /* init Wifi */
    if (wifi_ok != l5_wifi_init(&opt)) {
        /* Maybe esp8266's baud rate is others */
        wifi_log("init error");
        Error_Handler();
    }
    wifi_log("init success");

    uint32_t baudRate = 0;
    if (wifi_ok != l5_wifi_get_baudrate(&baudRate)) {
        wifi_log("get worker baud rate error");
        Error_Handler();
    }
    wifi_log("baud rate is %lu", baudRate);

    /* query work mode, insure it's Station mode */
    if (l5_wifi_get_work_mode() != work_mode_station) {
        wifi_log("current work mode is not station");
        err = l5_wifi_set_work_mode(work_mode_station);
        if (err != wifi_ok) {
            wifi_log("set current work mode to station error~");
            Error_Handler();
        }
        wifi_log("set current work mode to station success~");
    }

    { /* query which AP connected */
        wifi_ap_t ap;
        l5_wifi_get_joined_ap(&ap);
        switch (ap.err) {
            case ap_err_ok:
                /* disconnect first if connected others. */
                if (0 == strcmp(ap.ssid, AP_SSID)) {
                    break;
                }
                l5_wifi_exit_ap();

            case ap_err_no_connect: {
                /* else connect to specific ssid with pwd */
                err = l5_wifi_join_ap(AP_SSID, AP_PWD);
                if (err != wifi_ok) {
                    Error_Handler();
                }
            }
                break;
            default:
                break;

        }
    }
    wifi_log("connect [%s] success", AP_SSID);

    { /* query Wifi's information */
        wifi_sta_ip_t local_ip;

        while (1) {
            l5_wifi_get_sta_ip(&local_ip);

            if (local_ip.ip > 0 ) {
                break;
            }
            osDelay(500);
        }
    }

    {
        net_status_t status = l5_wifi_net_status();
        if (status == ns_tcp_udp_connected) {
            l5_tcp_close();
        }

        err = l5_tcp_dial(TCP_SERVER_IP, TCP_SERVER_PORT, 0);
        if (err != wifi_ok) {
            wifi_log("tcp dial to %s:%d err: %d", TCP_SERVER_IP, TCP_SERVER_PORT, err);
            Error_Handler();
        }
        wifi_log("tcp dial to %s:%d success", TCP_SERVER_IP, TCP_SERVER_PORT);

    }
    {/* test read write */
        err = l5_tcp_write("begin", (uint16_t) strlen("begin"));
        if (err != wifi_ok) {
            Error_Handler();
        }
        uint8_t *buf = NULL;
        uint16_t size = 0;

        uint8_t count = 0;
        while (1) {
            err = l5_tcp_read((void **) &buf, &size, osWaitForever);
            if (err == wifi_timeout) {
                continue;
            }
            if(++count == 0) {
                count = 1;
            }
            vPortFree(buf);

            err = l5_tcp_write("next", 4);
            if (err != wifi_ok) {
                Error_Handler();
            }
        }
    }
}

#endif