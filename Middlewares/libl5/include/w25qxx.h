//
// Created by lonphy on 2019/1/20.
//

#ifndef __LIB_L5_W25QXX_H__
#define __LIB_L5_W25QXX_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "mcu.h"

#define W25Q16 0xEF4015


extern uint8_t spi_write_read(SPI_TypeDef *spi, uint8_t dat);


uint8_t l5_w25qxx_read_sr1(void);

uint8_t l5_w25qxx_read_sr2(void);

uint8_t l5_w25qxx_read_sr3(void);

void l5_w25qxx_write_sr1(uint8_t sr);

void l5_w25qxx_write_sr2(uint8_t sr);

void l5_w25qxx_write_sr3(uint8_t sr);

void l5_w25qxx_write_enable(void);

void l5_w25qxx_write_disable(void);

void l5_w25qxx_erase_chip(void);

void l5_w25qxx_erase_sector(uint32_t sector);

void l5_w25qxx_low_power(void);

void l5_w25qxx_wakeup(void);

void l5_w25qxx_read(uint32_t address, uint8_t *buff, uint16_t size);

void l5_w25qxx_write_page(uint32_t page, uint8_t *buff, uint16_t size);

void l5_w25qxx_page_write(uint32_t address, uint8_t *buff, uint16_t size);

void l5_w25qxx_write_sector(uint32_t addr, uint8_t *buff, uint16_t size);

uint32_t l5_w25qxx_read_id();

#ifdef __cplusplus
}
#endif
#endif //__LIB_L5_W25QXX_H__
