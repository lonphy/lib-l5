//
// Created by lonphy on 2018/10/28.
//
#include <lib_l5.h>
#include <string.h>

#ifdef L5_USE_ESP8266

#define wifi_log(f, ...) printf("[wifi] " f "\n", ##__VA_ARGS__)

#define wifi_tx_timeout 1000
#define wifi_rx_timeout 5000
#define tcp_server_ip   "192.168.1.17"
#define tcp_server_port 8899



void task_wifi(__unused void const *arg) {


    wifi_err_t err;

    /* init Wifi */
    if (wifi_ok != l5_wifi_init(wifi_tx_timeout, wifi_rx_timeout)) {
        wifi_log("init error");
        Error_Handler();
    }
    wifi_log("init success");

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
        l5_wifi_get_sta_ip(&local_ip);

        wifi_log("IP: %ld.%ld.%ld.%ld/%d",
                (local_ip.ip >> 24) & 0xff,
                (local_ip.ip >> 16) & 0xff,
                (local_ip.ip >> 8) & 0xff,
                local_ip.ip & 0xff,
                local_ip.mask);

        wifi_log("GW: %ld.%ld.%ld.%ld",
                (local_ip.gateway >> 24) & 0xff,
                (local_ip.gateway >> 16) & 0xff,
                (local_ip.gateway >> 8) & 0xff,
                local_ip.gateway & 0xff);
    }

//    {
//        err = l5_wifi_net_status();
//        if (err != wifi_ok) {
//            Error_Handler();
//        }
//    }

    {
        err = l5_tcp_dial(tcp_server_ip, tcp_server_port, 0);
        if (err != wifi_ok) {
            wifi_log("tcp dial to %s:%d err: %d", tcp_server_ip, tcp_server_port, err);
            Error_Handler();
        }
        wifi_log("tcp dial to %s:%d success", tcp_server_ip, tcp_server_port);

    }
#if 0
    {/*test tx*/
        char buf[1024] = {0};
        memset(buf, 'A', 1024);

        uint8_t n = 0;
        while(1) {
            buf[0] = n;
            err = l5_tcp_write(buf, 1024);
            if (err != wifi_ok) {
                Error_Handler();
            }
            ++n;
        }
    }
#endif


    osDelay(osWaitForever);
}

#endif