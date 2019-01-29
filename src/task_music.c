//
// Created by lonphy on 2019-01-27.
//

#include <lib_l5.h>
#include <string.h>
#include <stdio.h>
#include "hw.h"

#if defined(L5_MUSIC_DEMO)

#define music_log(f, ...) printf("[music] " f "\n", ##__VA_ARGS__)
#define uint32_t_lit(p)   (*((uint32_t *) p))
#define uint16_t_lit(p)   (*((uint16_t *) p))

static l5_mem_block_t dual_buffer[2] = {0};
__IO static uint8_t used_buffer_idx = 0;
static osSemaphoreId buf_play_end_evt;

static void play_pcm_buffer(uint8_t idx);

void init_wifi() {
    wifi_config_t opt = {
            .rx_timeout = WIFI_RX_TIMEOUT,
            .tx_timeout = WIFI_TX_TIMEOUT,
            .start_rx_func = hw_usart_start_dma_rx,
            .start_tx_func = hw_usart_start_dma_tx,
            .get_rx_length_func = hw_usart_get_dma_rx_length,
    };

    /* init Wifi */
    if (wifi_ok != l5_wifi_init(&opt)) {
        music_log("wifi init error");
        Error_Handler();
    }

    l5_wifi_set_work_mode(work_mode_station);

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
                if (wifi_ok != l5_wifi_join_ap(AP_SSID, AP_PWD)) {
                    Error_Handler();
                }
            }
                break;
            default:
                (void) 0;
        }
    }

    if (ns_tcp_udp_connected == l5_wifi_net_status()) {
        l5_tcp_close();
    }

    if (wifi_ok != l5_tcp_dial(TCP_SERVER_IP, TCP_SERVER_PORT, 0)) {
        music_log("tcp dial to %s:%d error", TCP_SERVER_IP, TCP_SERVER_PORT);
        Error_Handler();
    }
}

void task_music(void const *arg) {
    hw_dac_init();

    init_wifi();

    if (wifi_ok != l5_tcp_write("begin", 5)) {
        Error_Handler();
    }
    uint8_t *buf = NULL, *p;
    uint16_t size = 0;
    uint32_t file_size = 0;

    if (wifi_ok != l5_tcp_read((void **) &buf, &size, osWaitForever)) {
        Error_Handler();
    }

    /* start with RIFF */
    if (memcmp(buf, "RIFF", 4) != 0 || memcmp(buf + 8, "WAVEfmt ", 8) != 0) {
        music_log("file is not valid WAV format");
        vPortFree(buf);
        Error_Handler();
    }

    file_size = uint32_t_lit(buf + 4);
    music_log("file size is %lu bytes", file_size + 4);

    p = buf + 16;

    uint32_t chunk_size = uint32_t_lit(p);
    p += 4;
    uint16_t enc_type = uint16_t_lit(p);
    p += 2;
    uint16_t channel_num = uint16_t_lit(p);
    p += 2;
    uint32_t sampling_rate = uint32_t_lit(p);
    p += 4;
    music_log("file header size is %lu bytes, enc type %d, channel num:%d, sampling rate: %lu",
              chunk_size, enc_type, channel_num, sampling_rate);

    uint32_t trans_rate = uint32_t_lit(p);
    p += 4;
    uint16_t frame_size = uint16_t_lit(p);
    p += 2;
    uint16_t sampling_bit = uint16_t_lit(p);
    p += 2;
    music_log("transfer rate %lu, frame size %d, sampling bit %d", trans_rate, frame_size, sampling_bit);

    if (memcmp(p, "data", 4) != 0) {
        music_log("WAV file's data section not found");
        vPortFree(buf);
        Error_Handler();
    }
    p += 4;

    /* total length for PCM data */
    uint32_t dat_size = uint32_t_lit(p);
    p += 4;
    music_log("pcm total size = %lu", dat_size);

    used_buffer_idx = 0;
    dual_buffer[0].size = size - (p - buf);
    dual_buffer[0].buf = pvPortMalloc(dual_buffer[0].size);
    memcpy(dual_buffer[0].buf, p, dual_buffer[0].size);
    vPortFree(buf);

    osSemaphoreDef(play_evt);
    buf_play_end_evt = osSemaphoreCreate(osSemaphore(play_evt), 1);

    play_pcm_buffer(used_buffer_idx);

    uint32_t readed_size = dual_buffer[0].size;

    while (readed_size < dat_size) {

        if (wifi_ok != l5_tcp_write("next", 4)) {
            Error_Handler();
        }

        if (wifi_ok != l5_tcp_read((void **) &buf, &size, osWaitForever)) {
            continue;
        }

        /* wait sem, fill next buffer */
        /* FIXME: it may be panic when net speed slower than dac convert speed */
        osSemaphoreWait(buf_play_end_evt, osWaitForever);

        used_buffer_idx = (uint8_t) ((used_buffer_idx + 1) % 2);

        dual_buffer[used_buffer_idx].size = size;
        dual_buffer[used_buffer_idx].buf = pvPortMalloc(size);
        memcpy(dual_buffer[used_buffer_idx].buf, buf, size);
        vPortFree(buf);
        readed_size += size;

    }

    /* exit current task */
    vTaskDelete(NULL);
}

void play_pcm_buffer(uint8_t idx) {
    hw_dac_start_dma(dual_buffer[idx].buf, dual_buffer[idx].size);
}

/* IRQn for DAC Channel2 */
void hw_DMA2_Channel4_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_TC4(DMA2)) {

        /* Clear and disable IT for DAC's DMA */
        LL_DMA_ClearFlag_TC4(DMA2);
        LL_DAC_DisableDMAReq(DAC, LL_DAC_CHANNEL_2);

        /* play next buffer */
        play_pcm_buffer(used_buffer_idx);

        /* release sem, notify to fetch next buffer */
        osSemaphoreRelease(buf_play_end_evt);
    }
}

#endif