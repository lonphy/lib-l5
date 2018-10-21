#include "lib_l5.h"

#if defined(L5_USE_ESP8266)
#include <malloc.h>
#include <string.h>
#include <stdio.h>

static HAL_StatusTypeDef l5_Wifi_NoGetCommand(
        L5_Wifi_DrvTypeDef *h,
        uint8_t *cmd, const uint16_t cmdSize
) {
    HAL_StatusTypeDef ret;
    uint8_t buf[128];

    __HAL_LOCK(h);
    ret = HAL_UART_Transmit(h->hUart, cmd, cmdSize, __L5_ESP8266_TIMEOUT);
    if (HAL_OK != ret) {
        __HAL_UNLOCK(h);
        WifiLog("transmit err->%d", ret);
        return ret;
    }


    ret = HAL_UART_Receive(h->hUart, buf, 128, __L5_ESP8266_TIMEOUT);
    if (HAL_OK != ret) {
        __HAL_UNLOCK(h);
        WifiLog("receive err->%d", ret);
        return ret;
    }
    WifiLog("receive data: %s", buf);

    if (buf[0] != 'O' || buf[1] != 'K') {
        __HAL_UNLOCK(h);
        return HAL_ERROR;
    }

    __HAL_UNLOCK(h);
    return HAL_OK;
}

static HAL_StatusTypeDef l5_Wifi_QueryCommand(
        L5_Wifi_DrvTypeDef *h,
        uint8_t *cmd, uint16_t cmdSize,
        uint8_t *buf, uint16_t bufSize
) {
    HAL_StatusTypeDef ret;

    __HAL_LOCK(h);
    ret = HAL_UART_Transmit(h->hUart, cmd, cmdSize, __L5_ESP8266_TIMEOUT);
    if (HAL_OK != ret) {
        __HAL_UNLOCK(h);
        return ret;
    }

    ret = HAL_UART_Receive(h->hUart, buf, bufSize, __L5_ESP8266_TIMEOUT);
    if (HAL_OK != ret) {
        __HAL_UNLOCK(h);
        return ret;
    }

    __HAL_UNLOCK(h);
    return HAL_OK;
}

// 初始化WIFI模块
HAL_StatusTypeDef L5_Wifi_Init(L5_Wifi_DrvTypeDef *h) {
    HAL_StatusTypeDef result;

    result = HAL_UART_Init(h->hUart);
    if (HAL_OK != result) {
        return result;
    }

    uint8_t * discardBuf = malloc(512);

    HAL_UART_Receive(h->hUart, discardBuf, 512, 2000);
    WifiLog("droped %s", discardBuf);
    HAL_UART_Receive(h->hUart, discardBuf, 512, 2000);
    WifiLog("droped %s", discardBuf);
    free(discardBuf);

    return HAL_OK;
}

// 获取ESP8266模块信息
HAL_StatusTypeDef L5_Wifi_GetVersion(L5_Wifi_DrvTypeDef *h, uint8_t *versionInfo, uint16_t versionInfoSize) {
    return l5_Wifi_QueryCommand(h, CMD_GMR, CMD_GMR_SIZE, versionInfo, versionInfoSize);
}

// ESP8266重置
HAL_StatusTypeDef L5_Wifi_Reset(L5_Wifi_DrvTypeDef *h) {
    return l5_Wifi_NoGetCommand(h, CMD_RESET, CMD_RESET_SIZE);
}

// 检查ESP8266是否工作
HAL_StatusTypeDef L5_Wifi_Ping(L5_Wifi_DrvTypeDef *h) {
#ifdef DEBUG
    WifiLog("`%s`", CMD_AT);
#endif
    return l5_Wifi_NoGetCommand(h, CMD_AT, CMD_AT_SIZE);
}

// 获取当前工作模式
L5_Wifi_WorkMode L5_Wifi_GetWorkMode(L5_Wifi_DrvTypeDef *h) {
    uint8_t *buf = malloc(32);
    HAL_StatusTypeDef ret = l5_Wifi_QueryCommand(h, CMD_MODE_QUERY, CMD_MODE_QUERY_SIZE, buf, 32);
    if (ret != HAL_OK) {
        WifiLog("err -> %d", ret);
        free(buf);
        return CWM_Unknown;
    }

    WifiLog("buf -> %s", buf);

    // TODO 解析返回值
    // +CWMODE:<mode>
    //
    // OK/ERROR
    free(buf);

    return CWM_Unknown;
}

// 设置模块工作模式
HAL_StatusTypeDef L5_Wifi_SetWorkMode(L5_Wifi_DrvTypeDef *h, L5_Wifi_WorkMode mode) {
    HAL_StatusTypeDef ret;
    char *cmdBuf;
    int realCmdLen;

    if (mode == CWM_Unknown) {
        return HAL_ERROR;
    }
    cmdBuf = malloc(CMD_MODE_SET_SIZE);
    realCmdLen = sprintf(cmdBuf, CMD_MODE_SET_TPL, mode);
    ret = l5_Wifi_NoGetCommand(h, (uint8_t *) cmdBuf, (uint16_t) realCmdLen);
    free(cmdBuf);
    return ret;
}

// 获取当前加入的AP
HAL_StatusTypeDef L5_Wifi_GetJoinedAP(L5_Wifi_DrvTypeDef *h, char *ssid, uint16_t ssidSize) {
    HAL_StatusTypeDef ret;
    ret = l5_Wifi_QueryCommand(h, CMD_JOIN_AP_QUERY, CMD_JOIN_AP_QUERY_SIZE, (uint8_t *) ssid, ssidSize);
    if (ret != HAL_OK) {
        return ret;
    }

    // TODO 解析返回值
    // + CWJAP:<ssid>
    // OK
    return HAL_OK;
}

// 加入给定AP
HAL_StatusTypeDef L5_Wifi_JoinAP(L5_Wifi_DrvTypeDef *h, const char *ssid, const char *pwd) {
    HAL_StatusTypeDef ret;
    char *cmdBuf;
    int realCmdLen;

    cmdBuf = malloc(CMD_JOIN_AP_SIZE);
    realCmdLen = sprintf(cmdBuf, CMD_JOIN_AP_TPL, ssid, pwd);
    ret = l5_Wifi_NoGetCommand(h, (uint8_t *) cmdBuf, (uint16_t) realCmdLen);
    free(cmdBuf);
    return ret;
}

// 获取所有可用AP列表
HAL_StatusTypeDef L5_Wifi_GetAPList(L5_Wifi_DrvTypeDef *h) {
    uint8_t *buf = malloc(128 * 10);
    HAL_StatusTypeDef ret = l5_Wifi_QueryCommand(h, CMD_LIST_AP, CMD_LIST_AP_SIZE, buf, 128 * 10);
    if (ret != HAL_OK) {
        free(buf);
        return ret;
    }

    WifiLog("buf -> %s", buf);
    // TODO 解析返回值
    // + CWLAP: <ecn>,<ssid>,<rssi>,<mac>
    // OK/ERROR
    free(buf);
    return HAL_OK;
}

// 退出当前连接AP
HAL_StatusTypeDef L5_Wifi_LeaveAp(L5_Wifi_DrvTypeDef *h) {
    return l5_Wifi_NoGetCommand(h, CMD_LEAVE_AP, CMD_LEAVE_AP_SIZE);
}


// 获取WIFI热点配置信息
HAL_StatusTypeDef L5_Wifi_GetHotspot(L5_Wifi_DrvTypeDef *h, L5_Wifi_HOTSPOT_InitTypeDef *opt) {
    HAL_StatusTypeDef ret;
    char *buf = malloc(128);
    ret = l5_Wifi_QueryCommand(h, CMD_HOTSPOT_QUERY, CMD_HOTSPOT_QUERY_SIZE, (uint8_t *) buf, 128);
    if (ret != HAL_OK) {
        free(buf);
        return ret;
    }

    // todo: 解析返回值
    // + CWSAP:<ssid>,<pwd>,<chl>,<ecn>

    return HAL_OK;
}

// 设置WIFI热点配置信息
HAL_StatusTypeDef L5_Wifi_SetHotspot(L5_Wifi_DrvTypeDef *h, L5_Wifi_HOTSPOT_InitTypeDef *opt) {
    HAL_StatusTypeDef ret;
    char *cmdBuf;
    int realCmdLen;

    cmdBuf = malloc(CMD_HOTSPOT_SET_TPL_SIZE);
    realCmdLen = sprintf(cmdBuf, CMD_HOTSPOT_SET_TPL, opt->SSID, opt->Pwd, opt->Channel, opt->EncType);
    ret = l5_Wifi_NoGetCommand(h, (uint8_t *) cmdBuf, (uint16_t) realCmdLen);
    free(cmdBuf);
    return ret;
}

HAL_StatusTypeDef L5_Wifi_GetClientIPList(L5_Wifi_DrvTypeDef *h, char *buf, uint16_t bufSize) {
    HAL_StatusTypeDef ret;
    ret = l5_Wifi_QueryCommand(h, CMD_LIST_CLIENT_IP, CMD_LIST_CLIENT_IP_SIZE, (uint8_t *) buf, bufSize);
    if (ret != HAL_OK) {
        return ret;
    }

    // todo: 解析返回值
    // + <ip addr>
    // OK

    return HAL_OK;
}
#endif // defined(L5_USE_ESP8266)
