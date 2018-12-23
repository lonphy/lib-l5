#ifndef __LIB_L5_ESP8266_01_H__
#define __LIB_L5_ESP8266_01_H__

#include "mcu.h"
#include <cmsis_os.h>

typedef enum wifi_ap_err_t {
    ap_err_ok,
    ap_err_no_connect,
    ap_err_timeout,
    ap_err_not_found,
    ap_err_connect_faild,
    ap_err_parse,
} wifi_ap_err_t;

typedef enum wifi_err_t {
    wifi_ok,
    wifi_timeout,
    wifi_invalid_param,
    wifi_rx_error,
    wifi_error,
    wifi_invalid_response,
    wifi_tx_error,
    wifi_reserved,
} wifi_err_t;

typedef enum command_state_t {
    command_idle,
    command_tx,
    command_rx,
    command_dat,
} command_state_t;

typedef enum wifi_enc_t {
    enc_open,
    enc_wep,
    enc_wpa_psk,
    enc_wpa2_psk,
    enc_wpa_wpa2_psk,
    enc_wpa2_enterprise,
} wifi_enc_t;

typedef enum wifi_work_mode_t {
    work_mode_unknown = 0,
    work_mode_station,
    work_mode_ap,
    work_mode_station_and_ap,
} wifi_work_mode_t;

/**
 * @brief Soft-AP's configure
 */
typedef struct wifi_hotspot_t {
    uint8_t max_conn:3;    /* Maximum number of clients to access [1~4] */
    uint8_t ssid_hidden:1; /* flag for hidden SSID */
    uint8_t channel;
    wifi_enc_t enc;
    char ssid[32];
    char pwd[65];
} wifi_hotspot_t;

/* ap info */
typedef struct {
    uint8_t wps:1;
    uint8_t bgn:3;
    wifi_enc_t enc:3;
    uint8_t channel;
    int8_t rssi; /* signal strength */
    wifi_ap_err_t err;
    char ssid[32];
    char mac[17]; /* 16 Byte MAC for Access Point */
} wifi_ap_t;

/* local ip information */
typedef struct {
    uint8_t mask;
    uint32_t ip;
    uint32_t gateway;
} wifi_sta_ip_t;

#define dma_rx_buf_size  512U

typedef struct {
    const char *command;               /* activity at command */
    char *command_response;            /* response for activity at command*/
    volatile command_state_t state;    /* activity command state */

    osMutexId lock;
    wifi_err_t err;                     /* activity at command exec result code */

    uint8_t current_buf_idx:4;          /* rx buffer index of Receiving */
    volatile uint8_t ready_buf_idx:4;   /* rx buffer index with ready state */
    uint8_t rx_buf[2][dma_rx_buf_size]; /* rx buffer array */
    uint16_t rx_buf_size[2];            /* rx data length of every buffer */

    TaskHandle_t response_task;         /* task with response parse */
    osSemaphoreId tc_sem;               /* Semaphore for translate complete */
    osSemaphoreId parse_sem;            /* Semaphore for parsed a frame of data */
    osSemaphoreId rx_sem;               /* Semaphore for a frame of uart data, with IDLE IT */
    osMessageQId dat_queue;             /* Queue for Rx frame (start with +IPD) */

    uint16_t tx_timeout;                /* unit ms */
    uint16_t rx_timeout;                /* unit ms */
} wifi_t;


/* ------------------ network --------------- */

typedef enum net_status_t {
    ns_unknow,
    ns_ap_connected = 2,
    ns_tcp_udp_connected,
    ns_tcp_udp_disconnected,
    ns_ap_not_connect
} net_status_t;

typedef enum net_type_t {
    net_tcp,
    net_udp,
    net_ssl,
} net_type_t;

typedef struct net_dat_t {
    uint16_t rawSize;
    uint8_t *raw;
} net_dat_t;
/* ------------------ network --------------- */


/* ---- export functions ------------------ */
/**
 * @brief Init ESP8266
 */
wifi_err_t l5_wifi_init(uint16_t tx_timeout, uint16_t rx_timeout);

/**
 * @brief Reset ESP8266
 */
wifi_err_t l5_wifi_reset();

/**
 * @brief Set low level uart baud rate
 */
wifi_err_t l5_wifi_set_baudrate(uint32_t baud);

/**
 * @brief Get low level uart baud rate
 */
wifi_err_t l5_wifi_get_baudrate(uint32_t *baud);

/**
 * @brief Get ESP8266 Version Info
 */
wifi_err_t l5_wifi_get_version(uint8_t *versionInfo, uint16_t versionInfoSize);

/**
 * @brief Get active work mode
 */
wifi_work_mode_t l5_wifi_get_work_mode();

/**
 * @brief Switch work mode
 */
wifi_err_t l5_wifi_set_work_mode(wifi_work_mode_t mode);

/**
 * @brief Get active AP's information
 */
void l5_wifi_get_joined_ap(wifi_ap_t *ap);

/**
 * @brief Join to AP with specified ssid and pwd
 */
wifi_err_t l5_wifi_join_ap(const char *ssid, const char *pwd);

/**
 * @brief Get station information (local ip, gateway, mask)
 */
wifi_err_t l5_wifi_get_sta_ip(wifi_sta_ip_t *ip);

/**
 * @brief Scan all available AP list
 */
wifi_err_t l5_wifi_scan_ap_list(wifi_ap_t list[], uint8_t limit);

/**
 * @brief Disconnect active AP
 */
wifi_err_t l5_wifi_exit_ap();

/**
 * @brief Get configure of soft AP
 */
wifi_err_t l5_wifi_get_hotspot(wifi_hotspot_t *opt);

/**
 * @brief Set configure soft AP
 */
wifi_err_t l5_wifi_set_hotspot(wifi_hotspot_t *opt);

/**
 * @brief Get Client IP list with connected, only in server mode
 */
wifi_err_t l5_wifi_get_client_ip_list(char *buf, uint16_t bufSize);

/**
 * @brief Query Wi-Fi's status
 */
net_status_t l5_wifi_net_status();

/**
 * @brief Get IPv4 for domain
 */
wifi_err_t l5_net_domain(const char *domain, uint32_t *ipv4);

/**
 * @brief Dial TCP Connection
 *
 * @param {const char *} domain - server's domain or ip string
 * @param {uint16_t} port - server's tcp port
 * @param {uint16_t} keep_alive - tcp keep-alive, 0-disable, otherwise- enable [1~7200], unit:s
 * @return
 */
wifi_err_t l5_tcp_dial(const char *domain, uint16_t port, uint16_t keep_alive);

/**
 * @brief Send Data with TCP with block mode
 *
 * @param {const void *} buf - the buffer to be send
 * @param {uint16_t} buf_len - buffer size
 * @return
 */
wifi_err_t l5_tcp_write(const void *buf, uint16_t buf_len);

/**
 * @brief Read TCP Data with block mode
 *
 * @attention callee must free buffer, the buffer allocated in low level
 * @attention user should read data in real-time, otherwise data will be lost
 *
 * @param {void **} buf - point to buf pointer
 * @param {uint16_t*} buf_len - buf size
 * @param {uint16_t} timeout - read timeout in ms
 * @return wifi_err_t
 */
wifi_err_t l5_tcp_read(void **buf, uint16_t *buf_len, uint32_t timeout);

/**
 * @brief Close TCP Connection
 */
wifi_err_t l5_tcp_close(void);

#endif // __LIB_L5_ESP8266_01_H__