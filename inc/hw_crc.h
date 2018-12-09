#ifndef __crc_H
#define __crc_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mcu.h"
#include "main.h"

extern CRC_HandleTypeDef hcrc;

void hw_crc_init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ crc_H */
