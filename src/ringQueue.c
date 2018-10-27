//
// Created by lonphy on 2018/10/27.
//

#include "ringQueue.h"

#define ringQueueSize 128
static uint8_t pWrite, pRead, ringQueue[ringQueueSize] = {0};

void rqWrite(uint8_t dat) {
    ringQueue[pWrite] = dat;

    if (++pWrite >= ringQueueSize) {
        pWrite = 0;
    }

    if (pWrite == pRead) {
        if (++pRead >= ringQueueSize) {
            pRead = 0;
        }
    }
}

osStatus rqRead(uint8_t *buf) {
    if (pRead == pWrite) {
        return osErrorResource;
    }
    *buf = ringQueue[pRead++];
    if (pRead == ringQueueSize) {
        pRead = 0;
    }
    return osOK;
}