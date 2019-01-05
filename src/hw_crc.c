#include "hw_crc.h"


/* CRC init function */
void hw_crc_init(void) {

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);
}

/**
  * @brief  This function performs CRC calculation on BufferSize bytes from input data buffer aDataBuffer.
  * @param  BufferSize Nb of bytes to be processed for CRC calculation
  * @retval 32-bit CRC value computed on input data buffer
  */
uint32_t hw_crc32(const uint8_t *buf, uint32_t size) {
    register uint32_t data = 0;
    register uint32_t index = 0;

    /* Compute the CRC of Data Buffer array*/
    for (index = 0; index < (size / 4); index++) {
        data = (uint32_t) ((buf[4 * index + 3] << 24) | (buf[4 * index + 2] << 16) | (buf[4 * index + 1] << 8) |
                           buf[4 * index]);
        LL_CRC_FeedData32(CRC, data);
    }

    return (LL_CRC_ReadData32(CRC));
}