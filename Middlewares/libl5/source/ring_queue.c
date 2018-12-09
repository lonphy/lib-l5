//
// Created by lonphy on 2018/10/27.
//

#include "lib_l5.h"


L5_RingQueue_t *L5_RingQueueInit(uint16_t size) {
    L5_RingQueue_t *q = pvPortMalloc(sizeof(L5_RingQueue_t));
    q->size = size;
    q->pRead = 0;
    q->pWrite = 0;
    q->buf = pvPortMalloc(size);
    return q;
}

void L5_RingQueueWrite(L5_RingQueue_t *q, uint8_t dat) {
    q->buf[q->pWrite] = dat;

    if (++q->pWrite >= q->size) {
        q->pWrite = 0;
    }

    if (q->pWrite == q->pRead) {
        if (++q->pRead >= q->size) {
            q->pRead = 0;
        }
    }
}

osStatus L5_RingQueueRead(L5_RingQueue_t *q, uint8_t *buf) {
    if (q->pRead == q->pWrite) {
        return osErrorResource;
    }
    *buf = q->buf[q->pRead++];
    if (q->pRead == q->size) {
        q->pRead = 0;
    }
    return osOK;
}

void L5_RingQueueFree(L5_RingQueue_t *q) {
    vPortFree(q);
}