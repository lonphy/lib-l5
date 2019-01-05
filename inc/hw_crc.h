#ifndef __crc_H
#define __crc_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mcu.h"
#include "main.h"

void hw_crc_init(void);

uint32_t hw_crc32(const uint8_t * buf, uint32_t size);

#ifdef __cplusplus
}
#endif
#endif /*__ crc_H */
