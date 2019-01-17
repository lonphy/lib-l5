//
// Created by lonphy on 2018/10/20.
//

#include "lib_l5.h"

#if defined(L5_USE_LCD1602)

#define    __L5_LCD1602_WRITE_MODE()        HAL_GPIO_WritePin(LCD1602_CTL_GPIO, LCD1602_RW_PIN, GPIO_PIN_RESET)
#define    __L5_LCD1602_READ_MODE()         HAL_GPIO_WritePin(LCD1602_CTL_GPIO, LCD1602_RW_PIN, GPIO_PIN_SET)
#define    __L5_LCD1602_ENABLE()            HAL_GPIO_WritePin(LCD1602_CTL_GPIO, LCD1602_EN_PIN, GPIO_PIN_SET)
#define    __L5_LCD1602_DISABLE()           HAL_GPIO_WritePin(LCD1602_CTL_GPIO, LCD1602_EN_PIN, GPIO_PIN_RESET)
#define    __L5_LCD1602_SELECT_CMD_REG()    HAL_GPIO_WritePin(LCD1602_CTL_GPIO, LCD1602_RS_PIN, GPIO_PIN_RESET)
#define    __L5_LCD1602_SELECT_DAT_REG()    HAL_GPIO_WritePin(LCD1602_CTL_GPIO, LCD1602_RS_PIN, GPIO_PIN_SET)

// 测试LCD忙碌状态
static void L5_Lcd1602_IsBusy(void) {
    uint8_t status = 1;
    LCD1602_DAT_GPIO->BSRR = 0x0000ff00; // PB8~PB15 置1

    __L5_LCD1602_SELECT_CMD_REG(); // PB1=0
    __L5_LCD1602_READ_MODE();      // PB2=1

    while (status) {
        __L5_LCD1602_ENABLE();
        status = (uint8_t) ( (LCD1602_CTL_GPIO->IDR >> 15) & 0x01 );
        __L5_LCD1602_DISABLE();
    }
}

/******************************写命令*************************************************/

void L5_Lcd1602_WriteCmd(uint16_t cmd) {
    L5_Lcd1602_IsBusy();

    __L5_LCD1602_SELECT_CMD_REG();
    __L5_LCD1602_WRITE_MODE();

    LCD1602_DAT_GPIO->BSRR = (uint32_t) (cmd << 8 & 0xff00);
    LCD1602_DAT_GPIO->BRR = (uint32_t) ((~cmd) << 8 & 0xff00);

    __L5_LCD1602_ENABLE();
    __L5_LCD1602_DISABLE();
}

//设定显示位置
void L5_Lcd1602_SetPos(uint8_t x, uint8_t y) {
    uint8_t addr;
    if (y == 0) {
        addr = (uint8_t) 0x00 + x;
    } else {
        addr = (uint8_t) 0x40 + x;
    }
    L5_Lcd1602_WriteCmd((uint16_t) (addr | 0x80));
}

/******************************写数据*************************************************/


void L5_Lcd1602_WriteData(uint16_t dat) {
    L5_Lcd1602_IsBusy();
    __L5_LCD1602_SELECT_DAT_REG();

    __L5_LCD1602_WRITE_MODE();

    LCD1602_DAT_GPIO->BSRR = (uint32_t) ( (dat << 8)    & 0xff00 );
    LCD1602_DAT_GPIO->BRR = (uint32_t) (  ((~dat) << 8) & 0xff00);

    __L5_LCD1602_ENABLE();
    __L5_LCD1602_DISABLE();
}

void L5_Lcd1602_WriteString(uint8_t x, uint8_t y, char *str) {
    L5_Lcd1602_SetPos(x, y);
    while (*str != '\0') {
        L5_Lcd1602_WriteData((uint16_t)*str);
        str++;
    }
}

void L5_Lcd1602_Init()                //LCD初始化设定
{

    GPIO_InitTypeDef gpioOpt;
    gpioOpt.Pin = LCD1602_DAT_PINS;
    gpioOpt.Mode = GPIO_MODE_OUTPUT_OD;
    gpioOpt.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD1602_DAT_GPIO, &gpioOpt);

    gpioOpt.Pin = (LCD1602_RS_PIN | LCD1602_RW_PIN | LCD1602_EN_PIN);
    gpioOpt.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(LCD1602_CTL_GPIO, &gpioOpt);

    osDelay(1);

    // 16*2显示，5*7点阵，8位数据
    L5_Lcd1602_WriteCmd(__FUNC_CMD(__FUNC_8Bit, __FUNC_2_ROW, __FUNC_DOT_5x7));
    osDelay(1);

    L5_Lcd1602_WriteCmd(__INPUT_CMD(__AC_INC,__SCROLL_ENABLE));
    // 屏幕开, 光标、闪烁关
    L5_Lcd1602_WriteCmd(__DISPLAY_SWITCH_CMD(__SCREEN_ON, __CURSOR_ON, __BLINK_OFF));
    L5_Lcd1602_PosReset();
    L5_Lcd1602_SetCursorDir(CursorRight);

    // 清除LCD
    L5_Lcd1602_ClearScreen();
    osDelay(1);
}

void L5_Lcd1602_SetCursorDir(LcdCursorDir dir) {
    if (dir == CursorRight) {
        L5_Lcd1602_WriteCmd(__MOV_CMD(__MOV_CURSOR, __MOV_RIGHT));
    }else {
        L5_Lcd1602_WriteCmd(__MOV_CMD(__MOV_CURSOR, __MOV_LEFT));
    }
}
#endif // L5_USE_LCD1602
