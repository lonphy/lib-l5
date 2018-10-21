#ifndef __LIB_L5_ESP8266_01_H__
#define __LIB_L5_ESP8266_01_H__


#ifdef L5_USE_ESP8266

#include <stdio.h>

// 1000ms = 1s 默认读写超时
#define __L5_ESP8266_TIMEOUT 2000

#define WifiLog(f, ...) printf("[l5_esp8266:%s] " f "\n", __FUNCTION__, ##__VA_ARGS__)

/* --------------- at command defines ------------------------- */
#define CMD_AT                  (uint8_t *)"AT\r\n"
#define CMD_AT_SIZE             4
#define CMD_GMR                 (uint8_t *)"AT+GMR\r\n"
#define CMD_GMR_SIZE            8
#define CMD_RESET               (uint8_t *)"AT+RST\r\n"
#define CMD_RESET_SIZE          8
#define CMD_MODE_QUERY          (uint8_t *)"AT+CWMODE?\r\n"
#define CMD_MODE_QUERY_SIZE     12
#define CMD_MODE_SET_TPL        (const char *)"AT+CWMODE=%d\r\n"
#define CMD_MODE_SET_SIZE       13
#define CMD_JOIN_AP_QUERY       (uint8_t *)"AT+CWJAP?\r\n"
#define CMD_JOIN_AP_QUERY_SIZE  11
#define CMD_JOIN_AP_TPL         (const char *)"AT+CWJAP=%s,%s\r\n"
#define CMD_JOIN_AP_SIZE        128
#define CMD_LIST_AP             (uint8_t *)"AT+CWLAP=\r\n"
#define CMD_LIST_AP_SIZE        11
#define CMD_LEAVE_AP            (uint8_t *)"AT+CWQAP\r\n"
#define CMD_LEAVE_AP_SIZE       10
#define CMD_HOTSPOT_QUERY       (uint8_t *)"AT+CWSAP?\r\n"
#define CMD_HOTSPOT_QUERY_SIZE  11
#define CMD_HOTSPOT_SET_TPL     (const char *)"AT+CWSAP=%s,%s,%d,%d\r\n"
#define CMD_HOTSPOT_SET_TPL_SIZE 128
#define CMD_LIST_CLIENT_IP      (uint8_t *)"AT+CWLIF\r\n"
#define CMD_LIST_CLIENT_IP_SIZE 10
/* --------------- at command defines ------------------------- */

// wifi工作模式
typedef enum {
    CWM_Unknown = 0,
    CWM_Station,
    CWM_AP,
    CWM_StationAndAP,
} L5_Wifi_WorkMode;


// 传输加密类型
typedef enum {
    Enc_Open = 0,
    Enc_WPA_PSK = 2,
    Enc_WPA2_PSK,
    Enc_WPA_WPA2_PSK,
} L5_Wifi_EncType;

// 热点配置信息
typedef struct {
    char *SSID;
    char *Pwd;
    uint8_t Channel;
    L5_Wifi_EncType EncType;
} L5_Wifi_HOTSPOT_InitTypeDef;

typedef struct {
    HAL_LockTypeDef Lock; // used to lock instance;
    UART_HandleTypeDef *hUart;
} L5_Wifi_DrvTypeDef;



/* ---- export functions ------------------ */
// 初始化WIFI串口驱动
HAL_StatusTypeDef L5_Wifi_Init(L5_Wifi_DrvTypeDef *h);

// WIFI模块重置(重启)
HAL_StatusTypeDef L5_Wifi_Reset(L5_Wifi_DrvTypeDef *h);

// 获取WIFI模块信息
HAL_StatusTypeDef L5_Wifi_GetVersion(L5_Wifi_DrvTypeDef *h, uint8_t *buf, uint16_t buf_len);

// 获取当前工作模式
L5_Wifi_WorkMode L5_Wifi_GetWorkMode(L5_Wifi_DrvTypeDef *h);

// 设置当前工作模式
HAL_StatusTypeDef L5_Wifi_SetWorkMode(L5_Wifi_DrvTypeDef *h, L5_Wifi_WorkMode mode);

HAL_StatusTypeDef L5_Wifi_Ping(L5_Wifi_DrvTypeDef *h);

// 获取当前加入的AP
HAL_StatusTypeDef L5_Wifi_GetJoinedAP(L5_Wifi_DrvTypeDef *h, char *ssid, uint16_t bufLen);

// 加入给定AP
HAL_StatusTypeDef L5_Wifi_JoinAP(L5_Wifi_DrvTypeDef *h, const char *ssid, const char *pwd);

// 查询所有可用AP
HAL_StatusTypeDef L5_Wifi_GetAPList(L5_Wifi_DrvTypeDef *h);

// 与当前连接AP断开连接
HAL_StatusTypeDef L5_Wifi_LeaveAp(L5_Wifi_DrvTypeDef *h);

// 获取WIFI热点配置信息
HAL_StatusTypeDef L5_Wifi_GetHotspot(L5_Wifi_DrvTypeDef *h, L5_Wifi_HOTSPOT_InitTypeDef *opt);

// 设置WIFI热点配置信息
HAL_StatusTypeDef L5_Wifi_SetHotspot(L5_Wifi_DrvTypeDef *h, L5_Wifi_HOTSPOT_InitTypeDef *opt);

// 获取接入设备的IP列表
HAL_StatusTypeDef L5_Wifi_GetClientIPList(L5_Wifi_DrvTypeDef *h, char *buf, uint16_t bufSize);

#endif // L5_USE_ESP8266

#endif // __LIB_L5_ESP8266_01_H__
