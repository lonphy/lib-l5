//
// Created by lonphy on 2018/10/21.
//

#ifndef __LIB_L5_LCD12864_H__
#define __LIB_L5_LCD12864_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu.h"



/*
________________________________________________________________________________________________________________________________
                           |            Command Code                   |
  Command                  |-------------------------------------------|                    Function                            
                           |  A0 | /RD | /WR | D7 D6 D5 D4 D3 D2 D1 D0 |
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(1) Display ON/OFF         |  0  |  1  |  0  | 1  0  1  0  1  1  1  0  | LCD display ON/OFF
                           |     |     |     |                      1  | 0: OFF, 1: ON
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(2) Display start line set |  0  |  1  |  0  | 0  1  v  v  v  v  v  v  | Sets the display RAM display start line address
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(3) Page address set       |  0  |  1  |  0  | 1  0  1  1  v  v  v  v  | Sets the display RAM page address
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
4.Column addr set upper bit|  0  |  1  |  0  | 0  0  0  1  v  v  v  v  | most significant 4 bits of the display RAM column address.
Column addr set lower bit  |  0  |  1  |  0  | 0  0  0  0  v  v  v  v  | least significant 4 bits of the display RAM column address.
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(5) Status read            |  0  |  0  |  1  | v  v  v  v  0  0  0  0  | Reads the status data
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(6) Display data write     |  1  |  1  |  0  | v  v  v  v  v  v  v  v  | Writes to the display RAM
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(7) Display data read      |  1  |  0  |  1  | v  v  v  v  v  v  v  v  | Reads from the display RAM
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(8) ADC select             |  0  |  1  |  0  | 1  0  1  0  0  0  0  0  | Sets the display RAM address SEG output correspondence
                           |     |     |     |                      1  | 0: normal, 1: reverse
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
9. Display normal/ reverse |  0  |  1  |  0  | 1  0  1  0  0  1  1  0  | Sets the LCD display normal/ reverse
                           |     |     |     |                      1  | 0: normal, 1: reverse
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
10.Display all points      |  0  |  1  |  0  | 1  0  1  0  0  1  0  0  | Display all points 0: normal display
    ON/OFF                 |     |     |     |                      1  | 1: all points ON
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(11) LCD bias set          |  0  |  1  |  0  | 1  0  1  0  0  0  1  0  | Sets the LCD drive voltage bias ratio
                           |     |     |     |                      1  | 0: 1/9 bias, 1: 1/7 bias (ST7565P)
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(12) Read/modify/write     |  0  |  1  |  0  | 1  1  1  0  0  0  0  0  | Column address increment At write: +1 At read: 0
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(13) End                   |  0  |  1  |  0  | 1  1  1  0  1  1  1  0  | Clear read/modify/write
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(14) Reset                 |  0  |  1  |  0  | 1  1  1  0  0  0  1  0  | Internal reset
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
15.Common output mode      |  0  |  1  |  0  | 1  1  0  0  0  *  *  *  | Select COM output scan direction 0: normal direction
                           |     |     |     |             1           | 1: reverse direction
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(16) Power control set     |  0  |  1  |  0  | 0  0  1  0  1  v  v  v  | Select internal power supply operating mode
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
17.V0 R ratio              |  0  | 1   |  0  | 0  0  1  0  0  v  v  v  | Select internal resistor ratio(Rb/Ra) mode
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
18. Electronic volume mode |  0  |  1  |  0  | 1  0  0  0  0  0  0  1  | Set the V0 output voltage electronic volume register
Electronic volume register |     |     |     | 0  0  v  v  v  v  v  v  |
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
20. Booster ratio set      |  0  |  1  |  0  | 1  1  1  1  1  0  0  0  | select booster ratio
                           |     |     |     | 0  0  0  0  0  0  v  v  | 00: 2x,3x,4x, 01: 5x, 11: 6x
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(21) Power saver           |     |     |     |                         | Display OFF and display all points ON compound command
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(22) NOP                   |  0  |  1  |  0  | 1  1  1  0  0  0  1  1  | Command for non-operation
---------------------------|-----|-----|-----|-------------------------|--------------------------------------------------------
(23) Test                  |  0  |  1  |  0  | 1  1  1  1  *  *  *  *  | Command for IC test. Do not use this command
--------------------------------------------------------------------------------------------------------------------------------
 */

/* ST7565P Commands */
// cmd1
#define cmd1_display_off            (uint8_t)(0xAE)
#define cmd1_display_on             (uint8_t)(0xAF)
#define cmd2_display_start_line(n)  (uint8_t)(0x40 | (0x3F & n) )
#define cmd3_page_address(n)        (uint8_t)(0xB0 | (0x07 & n) )
#define cmd4_column_upper(n)        (uint8_t)(0x10 | (n >> 4) )
#define cmd4_column_lower(n)        (uint8_t)(0x0F & n)
#define cmd8_adc_select_normal      (uint8_t)(0xA0)
#define cmd8_adc_select_reverse     (uint8_t)(0xA1)
#define cmd9_display_normal         (uint8_t)(0xA6)
#define cmd9_display_reverse        (uint8_t)(0xA7)
#define cmd10_display_normal_points (uint8_t)(0xA4)
#define cmd10_display_all_points    (uint8_t)(0xA5)
#define cmd11_bias_1_9              (uint8_t)(0xA2)
#define cmd11_bias_1_7              (uint8_t)(0xA3)
#define cmd14_reset                 (uint8_t)(0xE2)
#define cmd15_com_output_normal     (uint8_t)(0xC0)
#define cmd15_com_output_reverse    (uint8_t)(0xC8)
#define cmd16_power_ctrl(booster, regulator, follower) (uint8_t)(  \
                                     0x28                        | \
                                     ((booster  & 0x01) <<2)     | \
                                     ((regulator& 0x01) <<1)     | \
                                     ((follower & 0x01) <<0) )

/* ST7565P Commands */

/* ------------- Exports API ------------------------------------------------------------*/
#define L5_Lcd12864CDisplayON()         L5_Lcd12864WriteCmd(cmd1_display_on)
#define L5_Lcd12864CDisplayOFF()        L5_Lcd12864WriteCmd(cmd1_display_off)

/* *
 * display RAM address SEG output correspondence
 * L5_Lcd12864SetColFromLeftToRight() : SEG0   -> SEG131 [left  -> right]
 * L5_Lcd12864SetColFromRightToLeft() : SEG131 -> SEG0   [right -> left ]
 */
#define L5_Lcd12864SetColFromLeftToRight() L5_Lcd12864WriteCmd(cmd8_adc_select_normal)
#define L5_Lcd12864SetColFromRightToLeft() L5_Lcd12864WriteCmd(cmd8_adc_select_reverse)

/* *
 * Common Output Status Select
 * L5_Lcd12864SetPageFromTop2Bottom() : COM0  -> COM63 [top    -> bottom]
 * L5_Lcd12864SetPageFromBottom2Top() : COM63 -> COM0  [bottom -> top   ]
 */
#define L5_Lcd12864SetPageFromTop2Bottom()       L5_Lcd12864WriteCmd(cmd15_com_output_normal)
#define L5_Lcd12864SetPageFromBottom2Top()       L5_Lcd12864WriteCmd(cmd15_com_output_reverse)

/* Display normal 0->0, 1->1 */
#define L5_Lcd12864CDisplayNormal()  L5_Lcd12864WriteCmd(cmd9_display_normal)
/* Invert Display Data RAM to Display, means bit 0->1, 1->0 */
#define L5_Lcd12864CDisplayInvert() L5_Lcd12864WriteCmd(cmd9_display_reverse)

/* *
 * set start line address [0~7]
 * eg. L5_Lcd12864SetStartLine(5)
 * and write 5 line then:
 * line1  -> page5
 * line2  -> page6
 * line3  -> page7
 * line4  -> page0
 * line5  -> page1
 */
#define L5_Lcd12864SetStartLine(line)   L5_Lcd12864WriteCmd(cmd2_display_start_line(line))

/* set page address */
#define L5_Lcd12864SetPage(page)        L5_Lcd12864WriteCmd(cmd3_page_address(page))
/* set column address */
#define L5_Lcd12864SetColumn(col)  do{\
                        L5_Lcd12864WriteCmd(cmd4_column_upper(col)); \
                        L5_Lcd12864WriteCmd(cmd4_column_lower(col)); \
                        }while(0)

void L5_Lcd12864Init(void);

void L5_Lcd12864WriteCmd(uint8_t cmd);

void L5_Lcd12864WriteData(uint8_t dat);

void L5_Lcd12864ClearScreen(void);

void L5_Lcd12864WriteString(uint8_t x, uint8_t y, const char *str);

/* ------------- Exports API ------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif //__LIB_L5_LCD12864_H__
