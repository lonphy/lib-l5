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

#define pcm_buf_half_size 1344
static uint8_t pcm_buffer[pcm_buf_half_size * 2] = {0};

__IO static uint32_t readed_size = 0;
static uint32_t pcm_size = 0;

__IO static uint8_t need_load = 0;


static void play_pcm_buffer();

void init_wifi() {
    wifi_config_t opt = {
            .rx_timeout = WIFI_RX_TIMEOUT*2,
            .tx_timeout = WIFI_TX_TIMEOUT*4,
            .start_rx_func = hw_usart_start_dma_rx,
            .start_tx_func = hw_usart_start_dma_tx,
            .get_rx_length_func = hw_usart_get_dma_rx_length,
    };

    /* init Wifi */
    if (wifi_ok != l5_wifi_init(&opt)) {
        music_log("wifi init error");
        Error_Handler();
    }

    // l5_wifi_set_work_mode(work_mode_station);

    { /* query which AP connected */
        wifi_ap_t ap = {0};
        l5_wifi_get_joined_ap(&ap);
        switch (ap.err) {
            case ap_err_ok:
                /* disconnect first if connected others. */
                if (0 == strcmp(ap.ssid, AP_SSID)) {
                    music_log("%s already connected", AP_SSID);
                    break;
                }
                l5_wifi_exit_ap();

            case ap_err_no_connect: {
                /* else connect to specific ssid with pwd */
                if (l5_wifi_join_ap(AP_SSID, AP_PWD) != wifi_ok) {
                    Error_Handler();
                }
            }
                break;
            default: break;
        }
    }

    net_status_t status = l5_wifi_net_status();
    music_log("net status %d", status);

    l5_tcp_close();

    if (wifi_ok != l5_tcp_dial(TCP_SERVER_IP, TCP_SERVER_PORT, 3)) {
        music_log("dial tcp(%s:%d) error", TCP_SERVER_IP, TCP_SERVER_PORT);
        Error_Handler();
    }
}
void task_pcm_reader(void const *arg);

void task_music(void const *arg) {
    hw_dac_init();

    init_wifi();

    l5_tcp_write("b", 1);
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
    pcm_size = uint32_t_lit(p);
    p += 4;
    music_log("pcm total size = %lu", pcm_size);

    readed_size = size - (p - buf);

    memcpy(pcm_buffer, p, readed_size);
    vPortFree(buf);

    osThreadDef(dat_reader, task_pcm_reader, osPriorityHigh, 1, 1024);
    osThreadId tReader = osThreadCreate(osThread(dat_reader), NULL);

    while (readed_size < pcm_size) {



        osDelay(200);
    }

    /* exit current task */
    vTaskDelete(tReader);
    osDelay(osWaitForever);
}


void task_pcm_reader(void const *arg) {
    uint8_t *buf = NULL;
    uint16_t size = 0;
    wifi_err_t err;

    l5_tcp_write("n", 1);
    l5_tcp_read((void **) &buf, &size, osWaitForever);
    memcpy(pcm_buffer + pcm_buf_half_size, buf, pcm_buf_half_size);
    vPortFree(buf);

    play_pcm_buffer();


    while (readed_size < pcm_size) {
        while (need_load == 0){}

        err = l5_tcp_write("n", 1);
        if (wifi_ok != err) {
            music_log("write n error %x", err);
            hw_dac_stop();
            return;
        }

        if (wifi_ok != l5_tcp_read((void **) &buf, &size, 8000)) {
            music_log("read timeout");
            hw_dac_stop();
            return;
        }

        if ( need_load == 1) {
            need_load = 0;
            memcpy(pcm_buffer + pcm_buf_half_size, buf, pcm_buf_half_size);
        } else {
            need_load = 0;
            memcpy(pcm_buffer, buf, pcm_buf_half_size);
        }

        vPortFree(buf);
        readed_size += size;
    }

    hw_dac_stop();
}

void play_pcm_buffer() {
    hw_dac_start_dma(pcm_buffer, pcm_buf_half_size*2);
}

/* IRQn for DAC Channel2 */
void hw_DMA2_Channel4_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_TC4(DMA2)) {
        /* Clear and disable IT for DAC's DMA */
        LL_DMA_ClearFlag_TC4(DMA2);
        need_load = 1;
    } else if (LL_DMA_IsActiveFlag_HT4(DMA2)) {
        LL_DMA_ClearFlag_HT4(DMA2);
        need_load = 2;
    }
}

#endif