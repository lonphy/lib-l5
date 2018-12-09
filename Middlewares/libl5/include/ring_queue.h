//
// Created by lonphy on 2018/10/27.
//

#ifndef __LIB_L5_RING_QUEUE_H_
#define __LIB_L5_RING_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cmsis_os.h>

typedef struct {
    uint8_t pRead, pWrite;
    uint16_t size;
    uint8_t *buf;

} L5_RingQueue_t;

L5_RingQueue_t *L5_RingQueueInit(uint16_t size);

osStatus L5_RingQueueRead(L5_RingQueue_t *q, uint8_t *buf);

void L5_RingQueueWrite(L5_RingQueue_t *q, uint8_t dat);

void L5_RingQueueFree(L5_RingQueue_t *q);
#ifdef __cplusplus
}
#endif

#endif //__LIB_L5_RING_QUEUE_H_
