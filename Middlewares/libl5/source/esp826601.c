#include "lib_l5.h"

#include <string.h>
#include <stdlib.h>

#ifdef L5_USE_ESP8266

static inline const char *parse_ip(const char *str, uint32_t *dest);

static inline void ll_gpio_init();

static inline void ll_usart_init();

static inline void _esp8266_exec(const char *cmd, uint16_t cmd_len, const char *rsp_suffix);

static inline void ll_usart_transmit(const char *buf, uint16_t buf_size);

static inline void ll_usart_receive(uint8_t *buf, uint16_t buf_size);

static inline void exp8266_query(const char *cmd, uint16_t cmdSize, uint8_t *buf, uint16_t buf_size);

/* --------------- at command defines ------------------------- */
#define at_cwmode_tpl           "AT+CWMODE=%d\r\n"
#define at_cwmode_tpl_size      sizeof(at_cwmode_tpl)
#define at_cwjap_tpl            "AT+CWJAP=\"%s\",\"%s\"\r\n"
#define at_cwjap_tpl_size        128
#define at_cwlap                "AT+CWLAP=\r\n"
#define at_cwlap_size           sizeof(at_cwlap)
#define at_cwqap                "AT+CWQAP\r\n"
#define at_cwqap_size           sizeof(at_cwqap)
#define at_cwsap_query          "AT+CWSAP?\r\n"
#define at_cwsap_query_size     sizeof(at_cwsap_query)
#define at_cwsap_set_tpl        "AT+CWSAP=\"%s\",\"%s\",%d,%d,%d,%d\r\n"
#define at_cwsap_set_tpl_size    128
#define at_cwlif                "AT+CWLIF\r\n"
#define at_cwlif_size           sizeof(at_cwlif)
#define at_cipsta               "AT+CIPSTA?\r\n"
#define at_cipsta_size          sizeof(at_cipsta)
#define at_cwlapopt             "AT+CWLAPOPT=1,1567\r\n"
#define at_cwlapopt_size        sizeof(at_cwlapopt)


#define at_cipstatus            "AT+CIPSTATUS\r\n"
#define at_cipstatus_size       sizeof(at_cipstatus)

const char simple_rsp_ok[] = "\r\nOK\r\n";

/* --------------- at command defines ------------------------- */
/* ---------- config check ----------------------*/
#ifndef ESP8266_USART
#error ESP8266_USART not configed
#endif

#define ESP8266_USARTx             ((USART_TypeDef *)ESP8266_USART)
#if (ESP8266_USART == USART1_BASE)

#define __RAW_RCC_USARTx_CLK_ENABLE()  __HAL_RCC_USART1_CLK_ENABLE()
#define __RAW_RCC_USARTx_CLK_DISABLE() __HAL_RCC_USART1_CLK_DISABLE()
#define USARTx_IRQHandler              USART1_IRQHandler
#define USARTx_IRQn                    USART1_IRQn
#elif (ESP8266_USART == USART2_BASE)

#define __RAW_RCC_USARTx_CLK_ENABLE()  __HAL_RCC_USART2_CLK_ENABLE()
#define __RAW_RCC_USARTx_CLK_DISABLE() __HAL_RCC_USART2_CLK_DISABLE()
#define USARTx_IRQHandler              USART2_IRQHandler
#define USARTx_IRQn                    USART2_IRQn
#elif (ESP8266_USART == USART3_BASE)

#define __RAW_RCC_USARTx_CLK_ENABLE()  __HAL_RCC_USART3_CLK_ENABLE()
#define __RAW_RCC_USARTx_CLK_DISABLE() __HAL_RCC_USART3_CLK_DISABLE()
#define USARTx_IRQHandler              USART3_IRQHandler
#define USARTx_IRQn                    USART3_IRQn
#elif (ESP8266_USART == UART4_BASE)

#define __RAW_RCC_USARTx_CLK_ENABLE()  __HAL_RCC_UART4_CLK_ENABLE()
#define __RAW_RCC_USARTx_CLK_DISABLE() __HAL_RCC_UART4_CLK_DISABLE()
#define USARTx_IRQHandler              UART4_IRQHandler
#define USARTx_IRQn                    UART4_IRQn

/* for usart tx dma */
#define USARTx_DMAx                    DMA2
#define USARTx_Tx_DMA_Channel          5
#define USARTx_Tx_DMA_Channelx         DMA2_Channel5
#define USARTx_DMA_IRQn                DMA2_Channel4_5_IRQn
#define USARTx_DMA_Tx_IRQHandler       DMA2_Channel4_5_IRQHandler

#define USARTx_Rx_DMA_Channel          3
#define USARTx_Rx_DMA_Channelx         DMA2_Channel3
#define USARTx_Rx_DMA_IRQn             DMA2_Channel3_IRQn
#define USARTx_Rx_DMA_IRQHandler       DMA2_Channel3_IRQHandler

#define __RAW_RCC_DMAx_CLK_ENABLE()    __HAL_RCC_DMA2_CLK_ENABLE()
#define __RAW_RCC_DMAx_CLK_DISABLE()   __HAL_RCC_DMA2_CLK_DISABLE()
#endif

/* helper marcos for dma */
#define __DMA_GIFx_IDX                 (1 << ((USARTx_Tx_DMA_Channel - 1) * 4 - 1))
#define __DMA_TCIFx_IDX                (__DMA_GIFx_IDX << 1 )
#define __DMA_HTIFx_IDX                (__DMA_TCIFx_IDX << 1)
#define __DMA_TEIFx_IDX                (__DMA_HTIFx_IDX << 1)

/* ---------- config check ----------------------*/

static wifi_t esp8266;

osMutexDef(rw_lock); /* for write */
osSemaphoreDef(parse_sem);
osSemaphoreDef(tc_sem);

#define lock()   osMutexWait(esp8266.lock, osWaitForever)
#define unlock() osMutexRelease(esp8266.lock)
#define esp8266_exec(cmd, cmd_len) do{ \
    _esp8266_exec(cmd, cmd_len, simple_rsp_ok); \
    }while(0)

void USARTx_IRQHandler(void) {
    uint32_t sr = READ_REG(ESP8266_USARTx->SR);
    uint32_t cr1 = READ_REG(ESP8266_USARTx->CR1);
    uint32_t cr3 = READ_REG(ESP8266_USARTx->CR3);
    uint32_t error_flags;

    /* error checks */
    error_flags = (sr & (uint32_t) (USART_SR_FE | USART_SR_ORE | USART_SR_NE));
    if (error_flags == 0) {
        /* on reading */
        if (((sr & USART_SR_RXNE) != 0) && ((cr1 & USART_CR1_RXNEIE) != 0)) {
            if (esp8266.status == wifi_rx) {
                *esp8266.rx_buf++ = (uint8_t) (ESP8266_USARTx->DR & 0xff);
                if (--esp8266.rx_count == 0U) {
                    CLEAR_BIT(ESP8266_USARTx->CR3, USART_CR3_EIE);
                    CLEAR_BIT(ESP8266_USARTx->CR1, USART_CR1_RXNEIE | USART_CR1_IDLEIE);
                    esp8266.status = wifi_parse;
                    osSemaphoreRelease(esp8266.parse_semaphore);
                }
            }
            return;
        }

        /* idle means one frame complete */
        if (((cr1 & USART_CR1_IDLEIE) != 0) && ((sr & USART_SR_IDLE) != 0)) {
            if (esp8266.status == wifi_rx) {
                /* RDR has last byte of rx value when IDLE IT, read it to clear */
                (void) (ESP8266_USARTx->DR & 0xff);

                CLEAR_BIT(ESP8266_USARTx->CR3, USART_CR3_EIE);
                CLEAR_BIT(ESP8266_USARTx->CR1, USART_CR1_RXNEIE | USART_CR1_IDLEIE);

                esp8266.status = wifi_parse;
                osSemaphoreRelease(esp8266.parse_semaphore);
            }
            return;
        }
    }

    /* If some errors occur */
    if ((error_flags != 0) &&
        (((cr3 & USART_CR3_EIE) != 0) || ((cr1 & USART_CR1_RXNEIE) != 0))) {

        if ((cr3 & USART_CR3_EIE) != 0) {
            /* noise error */
            if ((sr & USART_SR_NE) != 0) {
                esp8266.err = wifi_rx_error;
            }

            /* frame error */
            if ((sr & USART_SR_FE) != 0) {
                esp8266.err = wifi_rx_error;
            }

            /* overrun error */
            if ((sr & USART_SR_ORE) != 0) {
                esp8266.err = wifi_rx_error;
            }
        }

        if (esp8266.err != wifi_ok) {
            // TODO: other status need to reset ?
            esp8266.status = wifi_parse;
            osSemaphoreRelease(esp8266.parse_semaphore);
        }
        return;
    } /* End if some error occurs */
}

/* IT handler for Tx DMA */
void USARTx_DMA_Tx_IRQHandler(void) {
    uint32_t flag_it = USARTx_DMAx->ISR;
    uint32_t source_it = USARTx_Tx_DMA_Channelx->CCR;

    /* Transfer Complete Interrupt management ***********************************/
    if (((flag_it & __DMA_TCIFx_IDX) != RESET) && ((source_it & DMA_IT_TC) != RESET)) {
        if ((USARTx_Tx_DMA_Channelx->CCR & DMA_CCR_CIRC) == 0U) {
            /* disable the transfer complete and error interrupt */
            CLEAR_BIT(USARTx_Tx_DMA_Channelx->CCR, DMA_CCR_TCIE | DMA_CCR_TEIE | DMA_IT_HT);
        }
        USARTx_DMAx->IFCR = __DMA_TCIFx_IDX; /* Clear the transfer complete flag */
        osSemaphoreRelease(esp8266.tc_semaphore);
    }

        /* Transfer Error Interrupt management **************************************/
    else if ((RESET != (flag_it & (__DMA_TEIFx_IDX))) && (RESET != (source_it & DMA_IT_TE))) {
        /* When a DMA transfer error occurs */
        /* A hardware clear of its EN bits is performed */
        /* Disable ALL DMA IT */
        CLEAR_BIT(USARTx_Tx_DMA_Channelx->CCR, DMA_CCR_TCIE | DMA_CCR_TEIE | DMA_IT_HT);
        USARTx_DMAx->IFCR = __DMA_GIFx_IDX; /* Clear all flags */

        esp8266.err = wifi_tx_error;
        osSemaphoreRelease(esp8266.tc_semaphore);
    }
}

/* IT handler for Rx DMA */
void USARTx_Rx_DMA_IRQHandler(void) {

}

__used static void ll_usart_deinit() {
    __RAW_RCC_USARTx_CLK_DISABLE();
    HAL_GPIO_DeInit(ESP8266_GPIO, ESP8266_Tx | ESP8266_Rx);
    HAL_NVIC_DisableIRQ(USARTx_IRQn);
}


// init esp8266-01 mode
wifi_err_t l5_wifi_init(uint16_t tx_timeout, uint16_t rx_timeout) {
    esp8266.tx_timeout = tx_timeout;
    esp8266.rx_timeout = rx_timeout;

    ll_usart_init();
    esp8266.lock = osMutexCreate(osMutex(rw_lock));

    /* sema for tx complete */
    esp8266.tc_semaphore = osSemaphoreCreate(osSemaphore(tc_sem), 1);
    osSemaphoreWait(esp8266.tc_semaphore, 0); // it has 1 signal when created, free it.

    /* sema for parse */
    esp8266.parse_semaphore = osSemaphoreCreate(osSemaphore(parse_sem), 1);
    osSemaphoreWait(esp8266.parse_semaphore, 0); // it has 1 signal when created, free it.

    /* close echo */
    esp8266_exec("ATE0\r\n", 6);
    if (esp8266.err != wifi_ok) {
        return esp8266.err;
    }

    /* setup ap scan's option */
    esp8266_exec(at_cwlapopt, at_cwlapopt_size);
    return esp8266.err;
}

// TODO: implement
wifi_err_t l5_wifi_get_version(uint8_t *versionInfo, uint16_t versionInfoSize) {
    exp8266_query("AT+GMR\r\n", 8, versionInfo, versionInfoSize);
    return esp8266.err;
}

// TODO: implement
wifi_err_t l5_wifi_reset() {
    esp8266_exec("AT+RST\r\n", 8);
    return esp8266.err;
}

// test
wifi_err_t l5_wifi_ping() {
    esp8266_exec("AT\r\n", 4);
    return esp8266.err;
}

wifi_work_mode_t l5_wifi_get_work_mode() {
    uint8_t mode = 0;
    char *buf = pvPortMalloc(32);
    exp8266_query("AT+CWMODE?\r\n", 12, (uint8_t *) buf, 32);
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

    cmdBuf = pvPortMalloc(at_cwmode_tpl_size);
    realCmdLen = sprintf(cmdBuf, at_cwmode_tpl, mode);
    esp8266_exec(cmdBuf, (uint16_t) realCmdLen);
    vPortFree(cmdBuf);
    return esp8266.err;
}

// get joined ap info
void l5_wifi_get_joined_ap(wifi_ap_t *ap) {
    char *buf = pvPortMalloc(120);
    char *p;
    uint8_t i;

    ap->err = ap_err_timeout;

    exp8266_query("AT+CWJAP?\r\n", 11, (uint8_t *) buf, 120);
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

// join to spec ap
wifi_err_t l5_wifi_join_ap(const char *ssid, const char *pwd) {
    char *cmd_buf = pvPortMalloc(at_cwjap_tpl_size);
    int real_cmd_len = sprintf(cmd_buf, at_cwjap_tpl, ssid, pwd);
    esp8266_exec(cmd_buf, (uint16_t) real_cmd_len);
    if (esp8266.err != wifi_ok) {
        goto END;
    }

    END:
    vPortFree(cmd_buf);
    return esp8266.err;
}


// get station's information(ip, mask, gateway)
wifi_err_t l5_wifi_get_sta_ip(wifi_sta_ip_t *ip) {
    char *buf = pvPortMalloc(120);

    exp8266_query(at_cipsta, at_cipsta_size, (uint8_t *) buf, 120);
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

// scan ap list with available
wifi_err_t l5_wifi_scan_ap_list(wifi_ap_t list[], uint8_t limit) {
    char *buf = pvPortMalloc(128 * 5);
    char *p;

    exp8266_query(at_cwlap, at_cwlap_size, (uint8_t *) buf, 128 * 5);
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

// leave current joined ap
wifi_err_t l5_wifi_exit_ap() {
    esp8266_exec(at_cwqap, at_cwqap_size);
    return esp8266.err;
}

// get soft ap config
wifi_err_t l5_wifi_get_hotspot(wifi_hotspot_t *opt) {
    char *buf = pvPortMalloc(128);
    char *p = NULL;
    uint8_t i;
    exp8266_query(at_cwsap_query, at_cwsap_query_size, (uint8_t *) buf, 128);
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

// set soft ap config
wifi_err_t l5_wifi_set_hotspot(wifi_hotspot_t *opt) {
    char *cmd_buf = pvPortMalloc(at_cwsap_set_tpl_size);
    int real_cmd_len = sprintf(cmd_buf, at_cwsap_set_tpl, opt->ssid, opt->pwd, opt->channel, opt->enc, opt->max_conn,
                               opt->ssid_hidden);
    esp8266_exec(cmd_buf, (uint16_t) real_cmd_len);
    vPortFree(cmd_buf);
    return esp8266.err;
}

wifi_err_t l5_wifi_get_client_ip_list(char *buf, uint16_t bufSize) {
    exp8266_query(at_cwlif, at_cwlif_size, (uint8_t *) buf, bufSize);
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
    exp8266_query(at_cipstatus, at_cipstatus_size, (uint8_t *) buf, 128);
    if (esp8266.err != wifi_ok) {
        return ns_unknow;
    }

    return ns_ap_connected;
}

wifi_err_t l5_net_domain(const char *domain, uint32_t *ipv4) {
    // TODO: implement
    return wifi_ok;
}

wifi_err_t l5_tcp_dial(const char *domain, uint16_t port, uint16_t keep_alive) {
    char *cmd_buf = pvPortMalloc(64);
    int real_cmd_len = sprintf(cmd_buf, "AT+CIPSTART=\"TCP\",\"%s\",%d,%d\r\n", domain, port, keep_alive);
    esp8266_exec(cmd_buf, (uint16_t) real_cmd_len);
    vPortFree(cmd_buf);
    return esp8266.err;
}

wifi_err_t l5_tcp_write(const void *buf, uint16_t buf_len) {
    char *cmd_buf = pvPortMalloc(32);
    int real_cmd_len = sprintf(cmd_buf, "AT+CIPSEND=%d\r\n", buf_len);
    _esp8266_exec(cmd_buf, (uint16_t) real_cmd_len, "\r\n> ");
    vPortFree(cmd_buf);

    if (esp8266.err != wifi_ok) {
        return esp8266.err;
    }
    _esp8266_exec(buf, buf_len, "");

    return esp8266.err;
}

wifi_err_t l5_tcp_close(void) {
    // TODO: implement
    return esp8266.err;
}


/* -------------------- private ---------------------- */
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

static inline void ll_gpio_init() {
    GPIO_InitTypeDef gpioOpt;
    gpioOpt.Pin = ESP8266_Tx | ESP8266_Rx;
    gpioOpt.Mode = GPIO_MODE_AF_PP;
    gpioOpt.Pull = GPIO_PULLUP;
    gpioOpt.Speed = GPIO_SPEED_FREQ_HIGH;
#ifdef STM32F4
    gpioOpt.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpioOpt.Alternate = GPIO_AF7_USART2; /* FIXME~ */
#endif
    HAL_GPIO_Init(ESP8266_GPIO, &gpioOpt);
#ifdef STM32F1
    // Not required in other STM32 series, eg F407, H743
    gpioOpt.Pin = ESP8266_Rx;
    gpioOpt.Mode = GPIO_MODE_INPUT;
    gpioOpt.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ESP8266_GPIO, &gpioOpt);
#endif
}

static void ll_usart_init() {
    __RAW_RCC_USARTx_CLK_ENABLE();

    ll_gpio_init();

    {/* configure tx DMA */
        __RAW_RCC_DMAx_CLK_ENABLE();

        USARTx_Tx_DMA_Channelx->CCR = (DMA_MEMORY_TO_PERIPH |
                                       DMA_PINC_DISABLE |
                                       DMA_MINC_ENABLE |
                                       DMA_PDATAALIGN_BYTE |
                                       DMA_MDATAALIGN_BYTE |
                                       DMA_NORMAL |
                                       DMA_PRIORITY_MEDIUM);

        /* DMA interrupt init and configuration */
        HAL_NVIC_SetPriority(USARTx_DMA_IRQn, 14, 0);
        HAL_NVIC_EnableIRQ(USARTx_DMA_IRQn);
    }

    {/* configure rx DMA */
        USARTx_Rx_DMA_Channelx->CCR = (DMA_PERIPH_TO_MEMORY |
                                       DMA_PINC_DISABLE |
                                       DMA_MINC_ENABLE |
                                       DMA_PDATAALIGN_BYTE |
                                       DMA_MDATAALIGN_BYTE |
                                       DMA_NORMAL |
                                       DMA_PRIORITY_HIGH);

        /* DMA interrupt init and configuration */
        HAL_NVIC_SetPriority(USARTx_Rx_DMA_IRQn, 14, 0);
        HAL_NVIC_EnableIRQ(USARTx_Rx_DMA_IRQn);
    }
    /* Disable the uart peripheral */
    CLEAR_REG(ESP8266_USARTx->CR1);
    CLEAR_REG(ESP8266_USARTx->CR2);
    CLEAR_REG(ESP8266_USARTx->CR3);

    {
        // BRR
        if (ESP8266_USARTx == USART1) {
            ESP8266_USARTx->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK2Freq(), ESP8266_BAUD_RATE);
        } else {
            ESP8266_USARTx->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK1Freq(), ESP8266_BAUD_RATE);
        }

        SET_BIT(ESP8266_USARTx->CR1, UART_MODE_TX_RX);
    }

    /* Enable the uart peripheral */
    SET_BIT(ESP8266_USARTx->CR1, USART_CR1_UE);

    /* setup interrupt */
    HAL_NVIC_SetPriority(USARTx_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(USARTx_IRQn);
}

static inline void _esp8266_exec(const char *cmd, uint16_t cmd_len, const char *rsp_suffix) {
    lock();

    /* step1. tx with it */
    ll_usart_transmit(cmd, cmd_len);

    /* step2. wait tx done */
    if (osSemaphoreWait(esp8266.tc_semaphore, esp8266.tx_timeout) != osOK) {
        esp8266.err = wifi_timeout;
        goto END;
    }

    /* step3. rx with it, use internal buffer */
    ll_usart_receive(esp8266.exec_rx_buf, exec_rx_buf_size);

    /* step4. wait rx done */
    if (osSemaphoreWait(esp8266.parse_semaphore, esp8266.rx_timeout) != osOK) {
        esp8266.err = wifi_timeout;
        goto END;
    }

    if (esp8266.err != wifi_ok) {
        goto END;
    }

    /* step5. compare suffix only */
    int rsp_len = strnlen((char *) esp8266.exec_rx_buf, exec_rx_buf_size - 1);
    if (strncmp((char *) esp8266.exec_rx_buf + (rsp_len - strlen(rsp_suffix)), rsp_suffix, strlen(rsp_suffix)) != 0) {
        esp8266.err = wifi_error;
    }

    END:
    unlock();
}

static inline void exp8266_query(const char *cmd, uint16_t cmdSize, uint8_t *buf, uint16_t buf_size) {
    lock();
    /* step1. tx with it */
    ll_usart_transmit(cmd, cmdSize);

    /* step2. wait tx done */
    if (osSemaphoreWait(esp8266.tc_semaphore, esp8266.tx_timeout) != osOK) {
        esp8266.err = wifi_timeout;
        goto END;
    }

    /* step3. rx with it, use external buffer */
    ll_usart_receive(buf, buf_size);

    /* step4. wait rx done */
    /* TODO: if response is more than once, how do it? */
    if (osSemaphoreWait(esp8266.parse_semaphore, esp8266.rx_timeout) != osOK) {
        esp8266.err = wifi_timeout;
        goto END;
    }

    END:
    unlock();
}

/* low level uart tx */
static inline void ll_usart_transmit(const char *buf, uint16_t buf_size) {
    esp8266.active_command = buf;
    esp8266.err = wifi_ok;
    esp8266.status = wifi_tx;

    /* Disable the peripheral */
    CLEAR_BIT(USARTx_Tx_DMA_Channelx->CCR, DMA_CCR_EN);

    /* Configure the source, destination address and the data length & clear flags*/
    {
        USARTx_DMAx->IFCR = __DMA_GIFx_IDX; /* Clear all flags */
        USARTx_Tx_DMA_Channelx->CNDTR = buf_size; /* Configure DMA Channel data length */
        USARTx_Tx_DMA_Channelx->CPAR = (uint32_t) (&ESP8266_USARTx->DR); /* Configure DMA Channel destination address */
        USARTx_Tx_DMA_Channelx->CMAR = (uint32_t) buf;        /* Configure DMA Channel source address */
    }


    CLEAR_BIT(USARTx_Tx_DMA_Channelx->CCR, DMA_CCR_HTIE); /* disable half transfer it */
    SET_BIT(USARTx_Tx_DMA_Channelx->CCR,
            DMA_CCR_TCIE | DMA_CCR_TEIE); /* enable the transfer complete it and error it */
    SET_BIT(USARTx_Tx_DMA_Channelx->CCR, DMA_CCR_EN); /* enable the DMA channel */

    CLEAR_BIT(ESP8266_USARTx->SR, UART_FLAG_TC); /* clear transmit complete flag */
    /* enable the DMA transfer for transmit request by setting the DMAT bit in the UART CR3 register */
    SET_BIT(ESP8266_USARTx->CR3, USART_CR3_DMAT);

}

/* low level uart rx */
static inline void ll_usart_receive(uint8_t *buf, uint16_t buf_size) {
    memset(buf, 0, buf_size);
    esp8266.rx_buf = buf;
    esp8266.rx_buf_size = buf_size;
    esp8266.rx_count = buf_size;

    esp8266.status = wifi_rx;

    /* enable error it: (frame error, noise error, overrun error) */
    SET_BIT(ESP8266_USARTx->CR3, USART_CR3_EIE);

    /* enable rx not empty it and idle it */
    SET_BIT(ESP8266_USARTx->CR1, USART_CR1_RXNEIE | USART_CR1_IDLEIE);
}

#endif // defined(L5_USE_ESP8266)
