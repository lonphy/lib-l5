#include "lib_l5.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hw_uart.h"

#ifdef L5_USE_ESP8266

static inline const char *parse_ip(const char *str, uint32_t *dest);

static inline void  uart_transmit(const char *buf, uint16_t buf_size);

static inline void esp8266_exec(const char *cmd, uint16_t cmd_len, const char *rsp_suffix);


static inline void esp8266_query(const char *cmd, uint16_t cmdSize, uint8_t *buf, uint16_t buf_size);

static inline void esp8266_write_dat(const void *buf, uint16_t buf_len);

static inline void alloc_command_response(size_t size, const void *dat);

static void response_parser_task(const void *arg);

/* ---------- config check ----------------------*/
#ifndef WIFI_USART
#error WIFI_USART not configed
#endif

static wifi_t esp8266;
static const char simple_rsp_ok[] = "OK\r\n";

#define lock()   osMutexWait(esp8266.lock, osWaitForever)
#define unlock() osMutexRelease(esp8266.lock)

void l5_tx_complete(wifi_err_t err) {
    esp8266.err = err;
    osSemaphoreRelease(esp8266.tc_sem);
}

void response_parser_task(__unused const void *arg) {
    char *buf = 0;
    char *p = 0;
    char *pProcessing = 0;

    size_t size = 0, size1 = 0, current_frame_size = 0, need_dat_size = 0;
    uint8_t IPDFlag = 0;
    net_dat_t *dat = NULL;

    while (1) {
        if (osOK != osSemaphoreWait(esp8266.rx_sem, osWaitForever)) {
            continue;
        }

        size = esp8266.rx_buf_size[esp8266.ready_buf_idx];
        if (size == 0) {
            continue;
        }

        /* at first copy uart rx data to another buffer */
        {
            buf = pvPortMalloc(size + 1);
            memcpy(buf, esp8266.rx_buf[esp8266.ready_buf_idx], size);
            buf[size] = 0;
        }

        pProcessing = buf;

        while (pProcessing - buf < size) {
            /* skip all `\r\n` */
            while (*pProcessing == '\r' || *pProcessing == '\n') {
                ++pProcessing;
            }

            if (IPDFlag == 1) {
                if (size < need_dat_size) {
                    // data not enough, save dat, then wait next frame
                    memcpy(dat->raw, pProcessing, size);
                    need_dat_size -= size;
                    break;
                } else {
                    memcpy(dat->raw+(dat->rawSize - need_dat_size), pProcessing, need_dat_size);
                    dat->raw[dat->rawSize] = 0;
                    osMessagePut(esp8266.dat_queue, (uint32_t) dat, 10);
                    pProcessing += need_dat_size;
                    IPDFlag = 0;
                    continue;
                }
            }

            /* check `+IPD,n...` */
            p = strstr(pProcessing, "+IPD,");
            if (p && p == pProcessing) { /* start with `\r\n+IPD:n...` */
                dat = pvPortMalloc(sizeof(net_dat_t));
                dat->rawSize = (uint16_t) strtoul(p + 5, &p, 10);
                p++;
                dat->raw = pvPortMalloc(dat->rawSize + 1);
                current_frame_size = size - (p-buf);

                // check frame data is enough
                if (current_frame_size >= dat->rawSize) {
                    memcpy(dat->raw, p, dat->rawSize);
                    dat->raw[dat->rawSize] = 0;
                    osMessagePut(esp8266.dat_queue, (uint32_t) dat, 10);
                    pProcessing = p + dat->rawSize;
                    continue;
                } else {
                    // data not enough, save dat, then wait next frame
                    memcpy(dat->raw, p, current_frame_size);
                    need_dat_size = dat->rawSize - current_frame_size;
                    IPDFlag = 1;
                    break;
                }
            }

            /* in data sending, need check rsp */
            if (esp8266.state == command_dat) {

                /* parse `Recv ... bytes\r\n` */
                p = strstr(pProcessing, "Recv ");
                if (p && p == pProcessing) {
                    p = strstr(p, "bytes\r\n");
                    pProcessing = p + 7;
                    continue;
                }


                /* parse `SEND OK\r\n` or `SEND FAIL\r\n`` */
                p = strstr(pProcessing, "SEND ");
                if (p && p == pProcessing) {
                    p = strstr(pProcessing + 5, "\n");
                    size1 = (p + 1 - pProcessing);
                    alloc_command_response(size1, pProcessing);
                    pProcessing = p + 1;
                    continue;
                }
            }

            if (esp8266.state == command_rx) {

                /* sending data len */
                p = strstr(pProcessing, "OK\r\n> ");
                if (p && p == pProcessing) {
                    alloc_command_response(6, pProcessing);
                    pProcessing += 6;
                    continue;
                }

                p = strstr(pProcessing, "OK\r\n");
                if (p && p == pProcessing) {
                    alloc_command_response(4, pProcessing);
                    pProcessing = p + 4;
                    continue;
                }

                p = strstr(pProcessing, "ERROR\r\n");
                if (p && p == pProcessing) {
                    alloc_command_response(7, pProcessing);
                    pProcessing = p + 7;
                    continue;
                }

                /**
                 * parse query at command `+XXX:xxxx\r\n\r\nOK\r\n`
                 * current command is `AT+XXX?\r\n`
                 */
                if (*pProcessing == '+') {

                    /* response with query command prefix */
                    if (*(pProcessing + 1) == esp8266.command[3]) {
                        p = strstr(pProcessing, "\r\nOK\r\n");
                        if (p) {
                            size1 = (p + 6 - pProcessing);
                            alloc_command_response(size1, pProcessing);
                            pProcessing += size1;
                            continue;
                        }

                        p = strstr(pProcessing, "\r\nERROR\r\n");
                        if (p) {
                            size1 = (p + 9 - pProcessing);
                            alloc_command_response(size1, pProcessing);
                            pProcessing += size1;
                            continue;
                        }
                    }
                }

                /* for AT+CIPSTATUS */
                p = strstr(pProcessing, "STATUS:");
                if (p && p == pProcessing) {
                    p = strstr(pProcessing, "\r\n\r\nOK\r\n");
                    if (p) {
                        size1 = (p + 8 - pProcessing);
                        alloc_command_response(size1, pProcessing);
                        pProcessing += size1;
                        continue;
                    }
                }

                /* for AT+CIPSTART=... */
                p = strstr(pProcessing, "CONNECT\r\n\r\n");
                if (p && p == pProcessing) {
                    p = strstr(p + 11, "\n");
                    size1 = (p + 1 - pProcessing);
                    alloc_command_response(size1, pProcessing);
                    pProcessing += size1;
                    continue;
                }

                /* for AT+CIPCLOSE\r\n */
                p = strstr(pProcessing, "CLOSED\r\n\r\n");
                if (p && p == pProcessing) {
                    p = strstr(p+10, "\n");
                    size1 = (uint16_t) (p + 1 - pProcessing);
                    alloc_command_response(size1, pProcessing);
                    pProcessing += size1;
                    continue;
                }

                /* skip busy s...\r\n*/
                p = strstr(pProcessing, "busy s...");
                if (p && p == pProcessing) {
                    pProcessing += 8;
                    continue;
                }

                /* skip ATE0\r\n*/
                p = strstr(pProcessing, "ATE0\r\n");
                if (p && p == pProcessing) {
                    pProcessing += 6;
                    continue;
                }
            }

            if (strstr(pProcessing, "WIFI CONNECTED")) {
                pProcessing += 16; /* skip suffix \r\n */
                continue;
            }
            if (strstr(pProcessing, "WIFI GOT IP")) {
                pProcessing += 13; /* skip suffix \r\n */
                continue;
            }

            /* never goes to here, occurs when at fw version not 1.6 */
            // esp8266.command_response = buf;
            break;
        }
        vPortFree(buf);
        buf = NULL;
    }
}

wifi_err_t l5_wifi_init(uint16_t tx_timeout, uint16_t rx_timeout) {
    osMutexDef(rw_lock); /* for write */
    osSemaphoreDef(parse_sem);
    osSemaphoreDef(tc_sem);
    osSemaphoreDef(rx_sem);
    osMessageQDef(data_queue, 3, net_dat_t*);
    osThreadDef(rsp_parser, response_parser_task, osThreadGetPriority(osThreadGetId()), 1, 1024);

    esp8266.tx_timeout = tx_timeout;
    esp8266.rx_timeout = rx_timeout;

    esp8266.current_buf_idx = 1;

    // ll_usart_init();
    esp8266.lock = osMutexCreate(osMutex(rw_lock));

    {
        /* sema for tx complete */
        esp8266.tc_sem = osSemaphoreCreate(osSemaphore(tc_sem), 1);
        osSemaphoreWait(esp8266.tc_sem, 0); // it has 1 signal when created, free it.

        /* sema for parse */
        esp8266.parse_sem = osSemaphoreCreate(osSemaphore(parse_sem), 1);
        osSemaphoreWait(esp8266.parse_sem, 0); // it has 1 signal when created, free it.

        esp8266.rx_sem = osSemaphoreCreate(osSemaphore(rx_sem), 1);
        osSemaphoreWait(esp8266.rx_sem, 0); // it has 1 signal when created, free it.

        /* TODO: think bind to runner task? */
        esp8266.dat_queue = osMessageCreate(osMessageQ(data_queue), NULL);
    }

    /* start response parse task */
    esp8266.response_task = osThreadCreate(osThread(rsp_parser), NULL);
    if (esp8266.response_task == NULL) {
        return wifi_error;
    }

    hw_usart_start_dma_rx(esp8266.rx_buf[1], dma_rx_buf_size);

    esp8266_exec("AT\r\n", 4, simple_rsp_ok);
    /* close echo */
    esp8266_exec("ATE0\r\n", 6, simple_rsp_ok);

    if (esp8266.err != wifi_ok) {
        return esp8266.err;
    }

    /* setup ap scan's option */
    esp8266_exec("AT+CWLAPOPT=1,1567\r\n", 20, simple_rsp_ok);
    return esp8266.err;
}

wifi_err_t l5_wifi_set_baudrate(uint32_t baud) {
    if (baud < 50 || baud > 5000000) {
        return wifi_invalid_param;
    }

    char *buf = pvPortMalloc(32);
    int len = sprintf(buf, "AT+UART_CUR=%lu,8,1,0,0", baud);

    esp8266_exec(buf, (uint16_t) len, simple_rsp_ok);
    if (esp8266.err != wifi_ok) {
        vPortFree(buf);
        return esp8266.err;
    }

    // ll_usart_config(baud);
    vPortFree(buf);
    return esp8266.err;
}

wifi_err_t l5_wifi_get_baudrate(uint32_t *baud) {
    char *buf = pvPortMalloc(64);
    esp8266_query("AT+UART_CUR?\r\n", 14, (uint8_t *) buf, 64);
    if (esp8266.err != wifi_ok) {
        vPortFree(buf);
        return esp8266.err;
    }
    char *p = strstr(buf, "+UART_CUR:");
    if (p) {
        *baud = (uint32_t) strtol(p+10, NULL, 10);
    }
    vPortFree(buf);
    return esp8266.err;
}

// TODO: implement
wifi_err_t l5_wifi_get_version(uint8_t *versionInfo, uint16_t versionInfoSize) {
    esp8266_query("AT+GMR\r\n", 8, versionInfo, versionInfoSize);
    return esp8266.err;
}

wifi_work_mode_t l5_wifi_get_work_mode() {
    uint8_t mode = 0;
    char *buf = pvPortMalloc(32);
    esp8266_query("AT+CWMODE?\r\n", 12, (uint8_t *) buf, 32);
    if (wifi_ok != esp8266.err) {
        vPortFree(buf);
        return work_mode_unknown;
    }

    /* x offset is 20 in "strposAT+CWMODE?\r\n+CWMODE:d" */
    if (strnlen(buf, 32) > 20) {
        mode = (uint8_t) (buf[20] - '0');
    }
    vPortFree(buf);

    if (mode <= work_mode_station_and_ap) {
        return (wifi_work_mode_t) mode;
    }
    return work_mode_unknown;
}

wifi_err_t l5_wifi_set_work_mode(wifi_work_mode_t mode) {
    char *cmdBuf;
    int realCmdLen;

    if (mode == work_mode_unknown) {
        return wifi_invalid_param;
    }

    cmdBuf = pvPortMalloc(13);
    realCmdLen = sprintf(cmdBuf, "AT+CWMODE=%d\r\n", mode);
    esp8266_exec(cmdBuf, (uint16_t) realCmdLen, simple_rsp_ok);
    vPortFree(cmdBuf);
    return esp8266.err;
}

void l5_wifi_get_joined_ap(wifi_ap_t *ap) {
    char *buf = pvPortMalloc(120);
    char *p;
    uint8_t i;

    ap->err = ap_err_timeout;

    esp8266_query("AT+CWJAP?\r\n", 11, (uint8_t *) buf, 120);
    if (esp8266.err != wifi_ok) {
        goto END;
    }

    ap->err = ap_err_parse;

    // + CWJAP:<d>
    if (strlen(buf) < 11 + sizeof(simple_rsp_ok)) {
        // TODO:error handle
        if (strncmp(buf, "No AP", 5) == 0) {
            ap->err = ap_err_no_connect;
        } else {
            ap->err = ap_err_parse;
        }
    } else {
        // + CWJAP:<ssid>,<bssid>,<channel>,<rssi>\r\n\r\nOK\r\n
        p = buf + 8;
        i = 0;

        // parse ssid
        while (*p != '"' && i < 32) {
            ap->ssid[i++] = *p++;
        }
        p += 3;

        // parse bssid
        i = 0;
        while (*p != '"' && i < 17) {
            ap->mac[i++] = *p++;
        }

        // parse channel and rssi
        ap->channel = (uint8_t) strtoul(p + 2, &p, 10);
        ap->rssi = (int8_t) strtol(p + 1, NULL, 10);

        // TODO: think for some error was happened
//        if (4 != sscanf(buf, "+CWJAP:\"%[^\"]{0,64}\",\"%[^\"]\",%u,%d\r\n", ap->ssid, ap->bssid, &ap->channel, &ap->rssi)) {
//            ap->err = ap_err_parse;
//        }
        ap->err = ap_err_ok;
    }
    END:
    vPortFree(buf);
}

wifi_err_t l5_wifi_join_ap(const char *ssid, const char *pwd) {
    char *cmd_buf = pvPortMalloc(128);
    int real_cmd_len = sprintf(cmd_buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    esp8266_exec(cmd_buf, (uint16_t) real_cmd_len, simple_rsp_ok);
    vPortFree(cmd_buf);
    return esp8266.err;
}

wifi_err_t l5_wifi_get_sta_ip(wifi_sta_ip_t *ip) {
    char *buf = pvPortMalloc(120);

    esp8266_query("AT+CIPSTA?\r\n", 12, (uint8_t *) buf, 120);
    if (esp8266.err != wifi_ok) {
        goto END;
    }

    const char *p = buf;
    {
        // +CIPSTA:ip:"xx.xx.xx.xx"\r\n
        if (0 != strncmp("+CIPSTA:ip:\"", p, 12)) {
            esp8266.err = wifi_invalid_response;
            goto END;
        }
        p = parse_ip(p + 12, &ip->ip) + 3;
    }
    {
        // +CIPSTA:gateway:"xx.xx.xx.xx"\r\n
        if (0 != strncmp("+CIPSTA:gateway:\"", p, 17)) {
            esp8266.err = wifi_invalid_response;
            goto END;
        }
        p = parse_ip(p + 17, &ip->gateway) + 3;
    }
    {
        // +CIPSTA:netmask:"xx.xx.xx.xx"\r\n
        if (0 != strncmp("+CIPSTA:netmask:\"", p, 17)) {
            esp8266.err = wifi_invalid_response;
            goto END;
        }
        uint32_t mask = 0;
        parse_ip(p + 17, &mask);

#if 0
        __asm volatile( "mvn  r8, %1   \n"         /* r8 = ~mask; */
                        "clz  %0, r8   \n"         /* ip->mask = clz(r8) */
                        : "=r"(ip->mask) : "r"((uint32_t) mask) :
                      );
#endif
        ip->mask = __CLZ(~mask);
    }

    END:
    vPortFree(buf);
    return esp8266.err;
}

wifi_err_t l5_wifi_scan_ap_list(wifi_ap_t list[], uint8_t limit) {
    char *buf = pvPortMalloc(128 * 5);
    char *p;

    esp8266_query("AT+CWLAP=\r\n", 11, (uint8_t *) buf, 128 * 5);
    if (esp8266.err != wifi_ok) {
        vPortFree(buf);
        return esp8266.err;
    }

    // + CWLAP: <ecn>,<ssid>,<rssi>,<mac>,<channel>,<bgn>,<wps>
    // OK/ERROR

    uint8_t i;
    p = buf;
    for (uint8_t n = 0; n < limit && *p; n++) {
        if (strncmp(p, "+CWLAP:(", 8) != 0) {
            return wifi_invalid_response;
        }
        p += 8;
        list[n].enc = strtol(p, &p, 10);
        p += 2;

        /* ssid */
        {
            i = 0;
            while (*p != '"' && i < 32) {
                list[n].ssid[i++] = *p++;
            }
            p += 2;
        }
        list[n].rssi = (int8_t) strtol(p, &p, 10);

        p += 2;

        {
            i = 0;
            while (*p != '"' && i < 17) {
                list[n].mac[i++] = *p++;
            }
            p += 2;
        }
        list[n].channel = (uint8_t) strtol(p, &p, 10);
        ++p;
        list[n].bgn = (uint8_t) strtol(p, &p, 10);
        ++p;
        list[n].wps = (uint8_t) strtol(p, &p, 10);
        p += 3;
    }

    vPortFree(buf);
    return wifi_ok;
}

wifi_err_t l5_wifi_exit_ap() {
    esp8266_exec("AT+CWQAP\r\n", 10, simple_rsp_ok);
    return esp8266.err;
}

wifi_err_t l5_wifi_get_hotspot(wifi_hotspot_t *opt) {
    char *buf = pvPortMalloc(128);
    char *p = NULL;
    uint8_t i;
    esp8266_query("AT+CWSAP?\r\n", 11, (uint8_t *) buf, 128);
    if (esp8266.err != wifi_ok) {
        vPortFree(buf);
        return esp8266.err;
    }

    // + CWSAP:<ssid>,<pwd>,<chl>,<ecn>,<max_conn>,<ssid_hidden>
    if (strncmp(buf, "+CWSAP:\"", 8) != 0) {
        vPortFree(buf);
        return wifi_invalid_response;
    }
    p = buf + 8;
    /* ssid */
    {
        i = 0;
        while (*p != '"' && i < 32) {
            opt->ssid[i++] = *p++;
        }
        p += 3;
    }
    {
        i = 0;
        while (*p != '"' && i < 64) {
            opt->pwd[i++] = *p++;
        }
        p += 2;
    }
    opt->channel = (uint8_t) strtol(p, &p, 10);
    opt->enc = (uint8_t) strtol(p + 1, &p, 10);
    opt->max_conn = (uint8_t) strtol(p + 1, &p, 10);
    opt->ssid_hidden = (uint8_t) strtol(p + 1, NULL, 10);

    return wifi_ok;
}

wifi_err_t l5_wifi_set_hotspot(wifi_hotspot_t *opt) {
    char *cmd_buf = pvPortMalloc(128);
    int real_cmd_len = sprintf(cmd_buf,
            "AT+CWSAP=\"%s\",\"%s\",%d,%d,%d,%d\r\n",
            opt->ssid,
            opt->pwd,
            opt->channel,
            opt->enc,
            opt->max_conn,
            opt->ssid_hidden);
    esp8266_exec(cmd_buf, (uint16_t) real_cmd_len, simple_rsp_ok);
    vPortFree(cmd_buf);
    return esp8266.err;
}

wifi_err_t l5_wifi_get_client_ip_list(char *buf, uint16_t bufSize) {
    esp8266_query("AT+CWLIF\r\n", 10, (uint8_t *) buf, bufSize);
    if (esp8266.err != wifi_ok) {
        return esp8266.err;
    }

    // todo: parse
    // + <ip addr>
    // OK

    return wifi_ok;
}

net_status_t l5_wifi_net_status() {
    char *buf = pvPortMalloc(128);
    esp8266_query("AT+CIPSTATUS\r\n", 14, (uint8_t *) buf, 128);
    if (esp8266.err != wifi_ok) {
        vPortFree(buf);
        return ns_unknow;
    }

    char *p = strstr(buf, "STATUS:");
    uint8_t status = 0;
    if (p) {
        status = (uint8_t) strtol(p + 7, NULL, 10);
    }
    vPortFree(buf);
    if (status >= 2 && status <= 4) {
        return (net_status_t) status;
    }
    return ns_unknow;
}

wifi_err_t l5_net_domain(const char *domain, uint32_t *ipv4) {
    // TODO: implement
    return wifi_ok;
}

wifi_err_t l5_tcp_dial(const char *domain, uint16_t port, uint16_t keep_alive) {
    char *cmd_buf = pvPortMalloc(64);
    int real_cmd_len = sprintf(cmd_buf, "AT+CIPSTART=\"TCP\",\"%s\",%d,%d\r\n", domain, port, keep_alive);
    esp8266_exec(cmd_buf, (uint16_t) real_cmd_len, simple_rsp_ok);
    vPortFree(cmd_buf);
    return esp8266.err;
}

wifi_err_t l5_tcp_write(const void *buf, uint16_t buf_len) {
    esp8266_write_dat(buf, buf_len);
    return esp8266.err;
}

wifi_err_t l5_tcp_read(void **buf, uint16_t *buf_len, uint32_t timeout) {

    net_dat_t *dat;
    osEvent evt = osMessageGet(esp8266.dat_queue, timeout);
    if (evt.status == osEventMessage) {
        dat = evt.value.p;
        *buf = dat->raw;
        *buf_len = dat->rawSize;
        vPortFree(dat);
        return wifi_ok;
    }
    return wifi_error;
}

wifi_err_t l5_tcp_close(void) {
    esp8266_exec("AT+CIPCLOSE\r\n", 13, simple_rsp_ok);
    return esp8266.err;
}


/* -------------------- private ---------------------- */

static inline void alloc_command_response(size_t size, const void *dat) {
    vPortFree((void *) esp8266.command_response);
    esp8266.command_response = pvPortMalloc(size + 1);
    memcpy((void *) esp8266.command_response, dat, size);
    esp8266.command_response[size] = 0;
    osSemaphoreRelease(esp8266.parse_sem);
}

/* convert ip string to uint32 ip*/
static inline const char *parse_ip(const char *str, uint32_t *dest) {
    uint32_t tmp = 0, ret = 0;
    while (*str != '"') {
        if (*str == '.') {
            ++str;
            ret = (ret << 8) | (tmp & 0xff);
            tmp = 0;
            continue;
        }

        tmp = tmp * 10 + (*str - '0');
        ++str;
    }
    *dest = (ret << 8) | (tmp & 0xff);
    return str;
}

static inline void esp8266_write_dat(const void *buf, uint16_t buf_len) {
    lock();
    char *cmd_buf = pvPortMalloc(32);
    int real_cmd_len = sprintf(cmd_buf, "AT+CIPSEND=%d\r\n", buf_len);

    /* step1. tx with it */
    uart_transmit(cmd_buf, (uint16_t) real_cmd_len);
    if (esp8266.err != wifi_ok) {
        goto END;
    }
    esp8266.state = command_rx;

    /* step2. wait rx done */
    if (osSemaphoreWait(esp8266.parse_sem, esp8266.rx_timeout) != osOK) {
        esp8266.err = wifi_timeout;
        goto END;
    }

    if (esp8266.err != wifi_ok) {
        goto END;
    }

    /* step3. compare suffix only */
    int rsp_len = strnlen(esp8266.command_response, dma_rx_buf_size - 1);
    if (strncmp(esp8266.command_response + (rsp_len - 4), "\r\n> ", 4) != 0) {
        esp8266.err = wifi_invalid_response;
        goto END;
    }
    vPortFree((void *) esp8266.command_response);
    esp8266.command_response = NULL;

    /* step4. write data to esp8266 */
    uart_transmit(buf, buf_len);
    esp8266.state = command_dat;
    if (esp8266.err != wifi_ok) {
        goto END;
    }

    /* step5. check send result */
    if (osSemaphoreWait(esp8266.parse_sem, esp8266.rx_timeout) != osOK) {
        esp8266.err = wifi_timeout;
        goto END;
    }

    END:
    vPortFree(cmd_buf);
    esp8266.state = command_idle;
    unlock();
}

static inline void esp8266_exec(const char *cmd, uint16_t cmd_len, const char *rsp_suffix) {
    lock();

    /* step1. tx with it */
    uart_transmit(cmd, cmd_len);
    if (esp8266.err != wifi_ok) {
        goto END;
    }
    esp8266.state = command_rx;

    /* step2. wait rx done */
    if (osSemaphoreWait(esp8266.parse_sem, esp8266.rx_timeout) != osOK) {
        esp8266.err = wifi_timeout;
        goto END;
    }

    if (esp8266.err != wifi_ok) {
        goto END;
    }

    /* step3. compare suffix only */
    int rsp_len = strnlen(esp8266.command_response, dma_rx_buf_size - 1);
    if (strncmp(esp8266.command_response + (rsp_len - strlen(rsp_suffix)), rsp_suffix, strlen(rsp_suffix)) != 0) {
        esp8266.err = wifi_error;
    }

    END:
    esp8266.state = command_idle;
    unlock();
}

static inline void esp8266_query(const char *cmd, uint16_t cmdSize, uint8_t *buf, uint16_t buf_size) {
    lock();
    /* step1. tx with it */
    uart_transmit(cmd, cmdSize);
    if (esp8266.err != wifi_ok) {
        goto END;
    }
    esp8266.state = command_rx;

    /* step2. wait rx done */
    /* TODO: if response is more than once, how do it? */
    if (osSemaphoreWait(esp8266.parse_sem, esp8266.rx_timeout) != osOK) {
        esp8266.err = wifi_timeout;
        goto END;
    }
    if (esp8266.err != wifi_ok) {
        goto END;
    }

    /* FIXME: possible overflow of buf */
    memcpy(buf, esp8266.command_response, strnlen(esp8266.command_response, dma_rx_buf_size - 1));

    END:
    esp8266.state = command_idle;
    unlock();
}

void L5_rx_receive(wifi_err_t err) {
    if (esp8266.response_task == NULL) {
        return;
    }
    esp8266.err = err;
    uint8_t idx = esp8266.current_buf_idx;
    esp8266.ready_buf_idx = idx;
    /* handle previous buf len */
    esp8266.rx_buf_size[idx] = (uint16_t) (dma_rx_buf_size - hw_usart_get_dma_rx_length()); /* get data error*/
    esp8266.rx_buf[idx][esp8266.rx_buf_size[idx]] = 0;

    idx = esp8266.current_buf_idx = (uint8_t) ((idx + 1) % 2);
    esp8266.rx_buf_size[idx] = 0;

    hw_usart_start_dma_rx(esp8266.rx_buf[esp8266.current_buf_idx], dma_rx_buf_size);


    osSemaphoreRelease(esp8266.rx_sem);
}

static inline void  uart_transmit(const char *buf, uint16_t buf_size) {
    esp8266.state = command_tx;
    esp8266.command = buf;
    esp8266.err = wifi_ok;

    hw_usart_start_dma_tx((void *) buf, buf_size);
    if (osSemaphoreWait(esp8266.tc_sem, esp8266.tx_timeout) != osOK) {
        esp8266.err = wifi_timeout;
    }
}
#endif // defined(L5_USE_ESP8266)
