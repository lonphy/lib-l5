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

// 加密方式
typedef enum wifi_enc_t {
    enc_open,
    enc_wep,
    enc_wpa_psk,
    enc_wpa2_psk,
    enc_wpa_wpa2_psk,
    enc_wpa2_enterprise,
} wifi_enc_t;

// wifi工作模式
typedef enum wifi_work_mode_t {
    work_mode_unknown = 0,
    work_mode_station,
    work_mode_ap,
    work_mode_station_and_ap,
} wifi_work_mode_t;

// 热点配置信息
typedef struct wifi_hotspot_t {
    uint8_t    max_conn:3;    /* 最大接入client数量 1~4 */
    uint8_t    ssid_hidden:1; /* 是否隐藏 ssid */
    uint8_t    channel;
    wifi_enc_t enc;
    char       ssid[32];
    char       pwd[65];
} wifi_hotspot_t;

/* ap info */
typedef struct {
    uint8_t       wps:1;
    uint8_t       bgn:3;
    wifi_enc_t    enc:3;
    uint8_t       channel;
    int8_t        rssi; /* 信号强度 */
    wifi_ap_err_t err;
    char          ssid[32];
    char          mac[17]; /* AP的mac地址16字节  */
} wifi_ap_t;

/* 本地IP信息 */
typedef struct {
    uint8_t  mask;
    uint32_t ip;
    uint32_t gateway;
} wifi_sta_ip_t;

#define exec_rx_buf_size 64U
#define dma_rx_buf_size  1024U

typedef struct {
    const char   *command;  /* current at command */
    const char   *command_response; /* response for current at command*/
    command_state_t state; /* current command state */

    osMutexId    lock;
    wifi_err_t   err;      // current at command exec result code

    /* for rx */
    uint8_t       current_buf_idx:4;           /* 当前接收缓冲 */
    uint8_t       ready_buf_idx:4;             /* 就绪接收缓冲 */
    uint8_t       rx_buf[2][dma_rx_buf_size];  /* 接收缓冲 */
    uint16_t      rx_buf_size[2];              /* 缓冲已接收大小 */

    TaskHandle_t  response_task;           /* task with response parse */
    osSemaphoreId tc_sem;                  /* 信号量 - 发送完成     */
    osSemaphoreId dat_sem;                 /* 信号量 - 收到一帧数据  */
    osSemaphoreId parse_sem;               /* 信号量 - 可以开始解析  */
    osMessageQId  dat_queue;               /* 队列 - 收到Server数据 */

    uint16_t tx_timeout;                    /* 发送超时时间 ms */
    uint16_t rx_timeout;                    /* 接收超时时间 ms */
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

typedef struct net_dat_t{
    uint16_t rawSize;
    void *raw;
} net_dat_t;
/* ------------------ network --------------- */



/* ---- export functions ------------------ */
// init esp8266
wifi_err_t l5_wifi_init(uint16_t tx_timeout, uint16_t rx_timeout);

// WIFI模块重置(重启)
wifi_err_t l5_wifi_reset();

// 获取WIFI模块信息
wifi_err_t l5_wifi_get_version(uint8_t *versionInfo, uint16_t versionInfoSize);

// 获取当前工作模式
wifi_work_mode_t l5_wifi_get_work_mode();

// 设置当前工作模式
wifi_err_t l5_wifi_set_work_mode(wifi_work_mode_t mode);

wifi_err_t l5_wifi_ping();

// 获取当前加入的AP
void l5_wifi_get_joined_ap(wifi_ap_t *ap);

// 加入给定AP
wifi_err_t l5_wifi_join_ap(const char *ssid, const char *pwd);

// 获取当前Station的本地IP地址、网关、掩码信息
wifi_err_t l5_wifi_get_sta_ip(wifi_sta_ip_t *ip);

// 查询所有可用AP
wifi_err_t l5_wifi_scan_ap_list(wifi_ap_t list[], uint8_t limit);

// 与当前连接AP断开连接
wifi_err_t l5_wifi_exit_ap();

// 获取WIFI热点配置信息
wifi_err_t l5_wifi_get_hotspot(wifi_hotspot_t *opt);

// 设置WIFI热点配置信息
wifi_err_t l5_wifi_set_hotspot(wifi_hotspot_t *opt);

// Get Client IP list with connected, only in server mode
wifi_err_t l5_wifi_get_client_ip_list(char *buf, uint16_t bufSize);

// Query Wi-Fi's status
net_status_t l5_wifi_net_status();

// Get IPv4 for domain
wifi_err_t   l5_net_domain(const char * domain, uint32_t * ipv4);

/**
 * Dial TCP Connection
 *
 * @param {const char *} domain - server's domain or ip string
 * @param {uint16_t} port - server's tcp port
 * @param {uint16_t} keep_alive - tcp keep-alive, 0-disable, otherwise- enable [1~7200], unit:s
 * @return
 */
wifi_err_t   l5_tcp_dial(const char * domain, uint16_t port, uint16_t keep_alive);

/**
 * Send Data with TCP with block mode
 *
 * @param {const void *} buf - the buffer to be send
 * @param {uint16_t} buf_len - buffer size
 * @return
 */
wifi_err_t l5_tcp_write(const void * buf, uint16_t buf_len);

/**
 * Read TCP Data with block mode
 *
 * - callee must free buf, it's allocated in low level
 * - user should read data in real-time, otherwise data will be lost
 *
 * @param {void **} buf - point to buf pointer
 * @param {uint16_t*} buf_len - buf size
 * @param {uint16_t} timeout - read timeout in ms
 * @return wifi_err_t
 */
wifi_err_t l5_tcp_read(void **buf, uint16_t * buf_len, uint32_t timeout);

// Close TCP Connection
wifi_err_t l5_tcp_close(void);

#endif // __LIB_L5_ESP8266_01_H__