//
// Created by lonphy on 2019/1/27.
//

#include "hw.h"

void hw_DMA2_Channel4_IRQHandler(void);

void hw_DMA2_Channel5_IRQHandler(void);

void DMA2_Channel4_5_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_GI5(DMA2)) {
        hw_DMA2_Channel5_IRQHandler();
    } else if (LL_DMA_IsActiveFlag_GI4(DMA2)) {
        hw_DMA2_Channel4_IRQHandler();
    }
}

__attribute__((weak)) void hw_DMA2_Channel4_IRQHandler(void) {}

__attribute__((weak)) void hw_DMA2_Channel5_IRQHandler(void) {}