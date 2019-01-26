//
// Created by lonphy on 2019/1/20.
//

#include "lib_l5.h"


#define SPI_W25QXX_CS(n) (n?\
                        (LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4)):\
                        (LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4)))

#define dummy              0xff
#define W25QXX_PAGE_SIZE   0x100
#define W25QXX_SECTOR_SIZE 0x1000

#define spi_wr(dat) spi_write_read(SPI1, dat)
#define w25qxx_cmd1(cmd) do { \
        SPI_W25QXX_CS(0);     \
        spi_wr(cmd);          \
        SPI_W25QXX_CS(1);     \
    }while(0)

static inline uint8_t w25qxx_read_srn(uint8_t cmd) {
    uint8_t sr = 0;
    SPI_W25QXX_CS(0);
    spi_wr(cmd);
    sr = spi_wr(dummy);
    SPI_W25QXX_CS(1);
    return sr;
}

static inline void w25qxx_write_srn(uint8_t cmd, uint8_t sr) {
    SPI_W25QXX_CS(0);
    spi_wr(cmd);
    spi_wr(sr);
    SPI_W25QXX_CS(1);
}

static inline void w25xx_wait_busy() {
    while ((l5_w25qxx_read_sr1() & 0x01) == 0x01);
}

uint8_t l5_w25qxx_read_sr1(void) { return w25qxx_read_srn(0x05); }

uint8_t l5_w25qxx_read_sr2(void) { return w25qxx_read_srn(0x35); }

uint8_t l5_w25qxx_read_sr3(void) { return w25qxx_read_srn(0x15); }

void l5_w25qxx_write_sr1(uint8_t sr) { w25qxx_write_srn(0x01, sr); }

void l5_w25qxx_write_sr2(uint8_t sr) { w25qxx_write_srn(0x31, sr); }

void l5_w25qxx_write_sr3(uint8_t sr) { w25qxx_write_srn(0x11, sr); }


void l5_w25qxx_write_enable(void) { w25qxx_cmd1(0x06); }

void l5_w25qxx_write_disable(void) { w25qxx_cmd1(0x04); }

void l5_w25qxx_erase_chip(void) {
    l5_w25qxx_write_enable();
    w25xx_wait_busy();
    w25qxx_cmd1(0xc7);
    w25xx_wait_busy();
}

void l5_w25qxx_erase_sector(uint32_t sector) {
    sector *= 4096;
    l5_w25qxx_write_enable();
    w25xx_wait_busy();
    SPI_W25QXX_CS(0);
    spi_wr(0x20);
    spi_wr((uint8_t) ((sector) >> 16));
    spi_wr((uint8_t) ((sector) >> 8));
    spi_wr((uint8_t) (sector));
    SPI_W25QXX_CS(1);
    w25xx_wait_busy();
}

void l5_w25qxx_low_power(void) { w25qxx_cmd1(0xB9); }

void l5_w25qxx_wakeup(void) { w25qxx_cmd1(0xAB); }

void l5_w25qxx_read(uint32_t address, uint8_t *buff, uint16_t size) {
    SPI_W25QXX_CS(0);
    spi_wr(0x03);     //发送读取命令
    spi_wr((uint8_t) ((address) >> 16));
    spi_wr((uint8_t) ((address) >> 8));
    spi_wr((uint8_t) (address));

    while (size--) {
        *buff = spi_wr(dummy);
        buff++;
    }
    SPI_W25QXX_CS(1);
}

void l5_w25qxx_write_page(uint32_t page, uint8_t *buff, uint16_t size) {
    l5_w25qxx_write_enable();
    SPI_W25QXX_CS(0);
    spi_wr(0x02);
    spi_wr((uint8_t) ((page) >> 16));
    spi_wr((uint8_t) ((page) >> 8));
    spi_wr((uint8_t) (page));
    while (size--) {
        spi_wr(*buff);
        buff++;
    }
    SPI_W25QXX_CS(1);
    w25xx_wait_busy();
}

void l5_w25qxx_page_write(uint32_t address, uint8_t *buff, uint16_t size) {
    uint8_t temp = 0;
    uint8_t page_offset     = (uint8_t) (address % W25QXX_PAGE_SIZE); /* 计算将要写入的页偏移量 */
    uint8_t page_space_free = (uint8_t) (W25QXX_PAGE_SIZE - page_offset); /* 计算目标页还有多少字节可写入 */
    uint8_t page_need       = (uint8_t) (size / W25QXX_PAGE_SIZE); /* 计算 数据 需要写几页 */
    uint8_t page_more     = (uint8_t) (size % W25QXX_PAGE_SIZE); /* 计算 超出一页的部分有多少字节 */

    /* 写入地址是一页的开始 */
    if (page_offset == 0) {
        /* 写入的数据量不到一页 */
        if (page_need == 0) {
            l5_w25qxx_write_page(address, buff, size);
            return;
        }

        while (page_need--) {
            l5_w25qxx_write_page(address, buff, W25QXX_PAGE_SIZE);
            address += W25QXX_PAGE_SIZE;
            buff += W25QXX_PAGE_SIZE;
        }
        l5_w25qxx_write_page(address, buff, page_more);
        return;
    }

    /* 开始写的地址不在页的首地址上 */

    /* 写入的数据量不到一页 */
    if (page_need == 0) {

        /* 写入的数据， 超过当前页剩余空间 */
        if (page_more > page_space_free) {
            temp = page_more - page_space_free; /* 计算当前页需要写入多少数据 */
            l5_w25qxx_write_page(address, buff, page_space_free); /* Fixme: 页写入是否需要边界对齐 */
            address += page_space_free;
            buff += page_space_free;
            l5_w25qxx_write_page(address, buff, temp);
        } else {
            l5_w25qxx_write_page(address, buff, size);
        }
        return;
    }

    /* 写入的数据量超过一页 */
    size -= page_space_free;
    page_need = (uint8_t) (size / W25QXX_PAGE_SIZE);
    page_more = (uint8_t) (size % W25QXX_PAGE_SIZE);

    l5_w25qxx_write_page(address, buff, page_space_free);
    address += page_space_free;
    buff += page_space_free;

    while (page_need--) {
        l5_w25qxx_write_page(address, buff, W25QXX_PAGE_SIZE);
        address += W25QXX_PAGE_SIZE;
        buff += W25QXX_PAGE_SIZE;
    }
    if (page_more != 0) {
        l5_w25qxx_write_page(address, buff, page_more);
    }
}

static uint8_t sector_cache[W25QXX_SECTOR_SIZE] = {0};

void l5_w25qxx_write_sector(uint32_t addr, uint8_t *buff, uint16_t size) {
    uint16_t i = 0;
    uint8_t *cache = sector_cache;
    uint32_t sec_addr = addr / W25QXX_SECTOR_SIZE;                              /* 计算将要写入哪一个sector */
    uint16_t sec_used_space = (uint16_t) (addr % W25QXX_SECTOR_SIZE);           /* 计算sector内的偏移地址 */
    uint16_t sec_free_space = (uint16_t) (W25QXX_SECTOR_SIZE - sec_used_space); /* 计算该sector还可以写入多少字节数据  */

    if (size <= sec_free_space) {
        sec_free_space = size;
    }

    // TODO: 考虑写入算法
    while (1) {
        /* 先读出第一个sector */
        l5_w25qxx_read(sec_addr * W25QXX_SECTOR_SIZE, cache, W25QXX_SECTOR_SIZE);

//        for (i = 0;  i< sec_free_space; ++i) {
//            cache[sec_used_space + i] = dummy;
//        }

        i = 0;
        while (cache[sec_used_space + i] == dummy) {
            if (++i == sec_free_space) {
                break;
            }
        }

        //需要擦除
        if (i < sec_free_space) {
            l5_w25qxx_erase_sector(sec_addr); //擦除这个扇区

            //复制
            for (i = 0; i < sec_free_space; i++) {
                cache[i + sec_used_space] = buff[i];
            }
            l5_w25qxx_page_write(sec_addr * W25QXX_SECTOR_SIZE, cache, W25QXX_SECTOR_SIZE);//写入整个扇区
        } else {
            l5_w25qxx_page_write(addr, buff, sec_free_space);//写已经擦除了的,直接写入扇区剩余区间.
        }

        if (size == sec_free_space) {
            break;//写入结束了
        }
        sec_addr++;//扇区地址增1
        sec_used_space = 0;//偏移位置为0

        buff += sec_free_space;  //指针偏移
        addr += sec_free_space;  //写地址偏移
        size -= sec_free_space;  //字节数递减
        if (size > W25QXX_SECTOR_SIZE) {
            sec_free_space = W25QXX_SECTOR_SIZE;
        } else {
            sec_free_space = size;
        }
    }
}


uint32_t l5_w25qxx_read_id() {
    uint32_t m, h, l;
    SPI_W25QXX_CS(0);
    spi_wr(0x9f);
    m = spi_wr(0);
    h = spi_wr(0);
    l = spi_wr(0);
    SPI_W25QXX_CS(1);

    return (m << 16 | h << 8 | l);
}
