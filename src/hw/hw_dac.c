//
// Created by lonphy on 2019-01-27.
//

#include "hw.h"

#define DAC_Ch     LL_DAC_CHANNEL_2
#define DAC_DMA    DMA2
#define DAC_DMA_Ch LL_DMA_CHANNEL_4

void hw_dac_init(void) {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);

    /* GPIO config, PA5->DAC Channel2 */
    GPIOA->CRL &= 0xff0fffff;
    GPIOC->CRH &= 0xfffffff0; /* set PC8 to Analog */

    { /* DAC Channel2 use DMA2 Channel4 */
        LL_DMA_ConfigTransfer(DAC_DMA, DAC_DMA_Ch,
                              LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                              LL_DMA_PRIORITY_VERYHIGH |
                              // LL_DMA_MODE_NORMAL |
                              LL_DMA_MODE_CIRCULAR |
                              LL_DMA_PERIPH_NOINCREMENT |
                              LL_DMA_MEMORY_INCREMENT |
                              LL_DMA_PDATAALIGN_BYTE |
                              LL_DMA_MDATAALIGN_BYTE);
        LL_DMA_EnableIT_TC(DAC_DMA, DAC_DMA_Ch);
        LL_DMA_EnableIT_HT(DAC_DMA, DAC_DMA_Ch);
    }

    {
        /* Peripheral clock enable */
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);

        /* TIM6 @APB1, default TIMxCLK is PCLK1*2 = SystemCoreClock */
        // LL_TIM_SetPrescaler(TIM6, 36 - 1);    /* TIM6CLK(2MHz) = 72MHz / 36 */
        // LL_TIM_SetAutoReload(TIM6, 125 - 1);  /* ARR(16k)      = 2MHz / 125 */

        LL_TIM_SetAutoReload(TIM6, SystemCoreClock / 16000 - 1); /* 16000 from wav file's sample rate */
        LL_TIM_GenerateEvent_UPDATE(TIM6);
        LL_TIM_EnableARRPreload(TIM6);
        LL_TIM_SetTriggerOutput(TIM6, LL_TIM_TRGO_UPDATE);
    }

    LL_DAC_InitTypeDef opt = {
            .TriggerSource = LL_DAC_TRIG_EXT_TIM6_TRGO,
            .OutputBuffer = LL_DAC_OUTPUT_BUFFER_DISABLE,
            /*.WaveAutoGeneration = LL_DAC_WAVE_AUTO_GENERATION_NONE,*/
    };
    LL_DAC_Init(DAC, DAC_Ch, &opt);

    LL_DAC_Enable(DAC, DAC_Ch);
    LL_DAC_EnableTrigger(DAC, DAC_Ch);
}

void hw_dac_start_dma(void *buf, uint32_t len) {

    LL_TIM_EnableCounter(TIM6);
    LL_DMA_ClearFlag_GI4(DAC_DMA);

    LL_DMA_ConfigAddresses(DAC_DMA, DAC_DMA_Ch,
                           (uint32_t) buf,
                           LL_DAC_DMA_GetRegAddr(DAC1, DAC_Ch, LL_DAC_DMA_REG_DATA_8BITS_RIGHT_ALIGNED),
                           LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    LL_DMA_SetDataLength(DAC_DMA, DAC_DMA_Ch, len);

    LL_DAC_EnableDMAReq(DAC, DAC_Ch);
    LL_DMA_EnableChannel(DAC_DMA, DAC_DMA_Ch);
}

void hw_dac_stop() {
    LL_TIM_DisableCounter(TIM6);
    LL_DAC_DisableDMAReq(DAC, DAC_Ch);
    LL_DMA_DisableChannel(DAC_DMA, DAC_DMA_Ch);
}