//
// Created by lonphy on 2018/10/21.
//

#ifndef __LIB_L5_LCD12864_FONT_H__
#define __LIB_L5_LCD12864_FONT_H__

#include <stdint-gcc.h>


uint8_t font5x8[] = {
        0x00,  // - -
        0x00,
        0x00,
        0x00,
        0x00,

        0x00,  // -!-
        0x00,
        0x5F,
        0x00,
        0x00,

        0x00,  // -"-
        0x07,
        0x00,
        0x07,
        0x00,

        0x14,  // -#-
        0x7F,
        0x14,
        0x7F,
        0x14,

        0x24,  // -$-
        0x2E,
        0x7B,
        0x2A,
        0x12,

        0x23,  // -%-
        0x13,
        0x08,
        0x64,
        0x62,

        0x36,  // -&-
        0x49,
        0x56,
        0x20,
        0x50,

        0x00,  // -'-
        0x04,
        0x03,
        0x01,
        0x00,

        0x00,  // -(-
        0x1C,
        0x22,
        0x41,
        0x00,

        0x00,  // -)-
        0x41,
        0x22,
        0x1C,
        0x00,

        0x22,  // -*-
        0x14,
        0x7F,
        0x14,
        0x22,

        0x08,  // -+-
        0x08,
        0x3e,
        0x08,
        0x08,

        0x40,  // -,-
        0x30,
        0x10,
        0x00,
        0x00,

        0x08,  // ---
        0x08,
        0x08,
        0x08,
        0x08,

        0x00,  // -.-
        0x00,
        0x30,
        0x00,
        0x00,

        0x20,  // -/-
        0x10,
        0x08,
        0x04,
        0x02,

        0x3E,  // -0-
        0x51,
        0x49,
        0x45,
        0x3E,

        0x00,  // -1-
        0x42,
        0x7F,
        0x40,
        0x00,

        0x62,  // -2-
        0x51,
        0x49,
        0x49,
        0x46,

        0x21,  // -3-
        0x41,
        0x49,
        0x4D,
        0x33,

        0x18,  // -4-
        0x14,
        0x12,
        0x7F,
        0x10,

        0x27,  // -5-
        0x45,
        0x45,
        0x45,
        0x39,

        0x3C,  // -6-
        0x4A,
        0x49,
        0x49,
        0x31,

        0x01,  // -7-
        0x71,
        0x09,
        0x05,
        0x03,

        0x36,  // -8-
        0x49,
        0x49,
        0x49,
        0x36,

        0x46,  // -9-
        0x49,
        0x49,
        0x29,
        0x1E,

        0x00,  // -:-
        0x00,
        0x14,
        0x00,
        0x00,

        0x00,  // -;-
        0x20,
        0x14,
        0x00,
        0x00,

        0x00,
        0x08,  // -<-
        0x14,
        0x22,
        0x00,

        0x14,  // -=-
        0x14,
        0x14,
        0x14,
        0x14,

        0x00,  // ->-
        0x22,
        0x14,
        0x08,
        0x00,

        0x02,  // -?-
        0x01,
        0x59,
        0x05,
        0x02,

        0x3E,  // -@-
        0x41,
        0x5D,
        0x55,
        0x5E,

        0x7C,  // -A-
        0x12,
        0x11,
        0x12,
        0x7C,

        0x7F,  // -B-
        0x49,
        0x49,
        0x49,
        0x36,

        0x3E,  // -C-
        0x41,
        0x41,
        0x41,
        0x22,

        0x7F,  // -D-
        0x41,
        0x41,
        0x41,
        0x3E,

        0x7F,  // -E-
        0x49,
        0x49,
        0x49,
        0x41,

        0x7F,  // -F-
        0x09,
        0x09,
        0x09,
        0x01,

        0x3E,  // -G-
        0x41,
        0x51,
        0x51,
        0x72,

        0x7F,  // -H-
        0x08,
        0x08,
        0x08,
        0x7F,

        0x00,  // -I-
        0x41,
        0x7F,
        0x41,
        0x00,

        0x20,  // -J-
        0x40,
        0x41,
        0x3F,
        0x01,

        0x7F,  // -K-
        0x08,
        0x14,
        0x22,
        0x41,

        0x7F,  // -L-
        0x40,
        0x40,
        0x40,
        0x40,

        0x7F,  // -M-
        0x02,
        0x0C,
        0x02,
        0x7F,

        0x7F,  // -N-
        0x04,
        0x08,
        0x10,
        0x7F,

        0x3E,  // -O-
        0x41,
        0x41,
        0x41,
        0x3E,

        0x7F,  // -P-
        0x09,
        0x09,
        0x09,
        0x06,

        0x3E,  // -Q-
        0x41,
        0x51,
        0x21,
        0x5E,

        0x7F,  // -R-
        0x09,
        0x19,
        0x29,
        0x46,

        0x26,  // -S-
        0x49,
        0x49,
        0x49,
        0x32,

        0x01,  // -T-
        0x01,
        0x7F,
        0x01,
        0x01,

        0x3F,  // -U-
        0x40,
        0x40,
        0x40,
        0x3F,

        0x1F,  // -V-
        0x20,
        0x40,
        0x20,
        0x1F,

        0x7F,  // -W-
        0x20,
        0x18,
        0x20,
        0x7F,

        0x63,  // -X-
        0x14,
        0x08,
        0x14,
        0x63,

        0x03,  // -Y-
        0x04,
        0x78,
        0x04,
        0x03,

        0x61,  // -Z-
        0x51,
        0x49,
        0x45,
        0x43,

        0x00,  // -[-
        0x3f,
        0x21,
        0x00,
        0x00,

        0x02,  // -\-
        0x04,
        0x08,
        0x10,
        0x20,

        0x00,  // -]-
        0x00,
        0x21,
        0x3f,
        0x00,

        0x08,  // -^-
        0x04,
        0x02,
        0x04,
        0x08,

        0x20,  // -_-
        0x20,
        0x20,
        0x20,
        0x20,

        0x00,  // -`-
        0x00,
        0x01,
        0x02,
        0x04,

        0x24,  // -a-
        0x54,
        0x54,
        0x38,
        0x40,

        0x7F,  // -b-
        0x28,
        0x44,
        0x44,
        0x38,

        0x38,  // -c-
        0x44,
        0x44,
        0x44,
        0x08,

        0x38,  // -d-
        0x44,
        0x44,
        0x28,
        0x7F,

        0x38,  // -e-
        0x54,
        0x54,
        0x54,
        0x08,

        0x08,  // -f-
        0x7E,
        0x09,
        0x09,
        0x02,

        0x98,  // -g-
        0xA4,
        0xA4,
        0xA4,
        0x78,

        0x7F,  // -h-
        0x08,
        0x04,
        0x04,
        0x78,

        0x00,  // -i-
        0x00,
        0x79,
        0x00,
        0x00,

        0x00,  // -j-
        0x80,
        0x88,
        0x79,
        0x00,

        0x7F,  // -k-
        0x10,
        0x28,
        0x44,
        0x40,

        0x00,  // -l-
        0x41,
        0x7F,
        0x40,
        0x00,

        0x78,  // -m-
        0x04,
        0x78,
        0x04,
        0x78,

        0x04,  // -n-
        0x78,
        0x04,
        0x04,
        0x78,

        0x38,  // -o-
        0x44,
        0x44,
        0x44,
        0x38,

        0xFC,  // -p-
        0x24,
        0x24,
        0x24,
        0x18,

        0x18,  // -q-
        0x24,
        0x24,
        0x24,
        0xFC,

        0x04,  // -r-
        0x78,
        0x04,
        0x04,
        0x08,

        0x48,  // -s-
        0x54,
        0x54,
        0x54,
        0x24,

        0x04,  // -t-
        0x3F,
        0x44,
        0x44,
        0x24,

        0x3C,  // -u-
        0x40,
        0x40,
        0x3C,
        0x40,

        0x1C,  // -v-
        0x20,
        0x40,
        0x20,
        0x1C,

        0x3C,  // -w-
        0x40,
        0x3C,
        0x40,
        0x3C,

        0x44,  // -x-
        0x28,
        0x10,
        0x28,
        0x44,

        0x9C,  // -y-
        0xA0,
        0xA0,
        0x90,
        0x7C,

        0x44,  // -z-
        0x64,
        0x54,
        0x4C,
        0x44,

        0x08,  // -{-
        0x36,
        0x41,
        0x00,
        0x00,

        0x00,  // -|-
        0x00,
        0x3f,
        0x00,
        0x00,

        0x00,  // -}-
        0x00,
        0x41,
        0x36,
        0x08,

        0x08,  // -~-
        0x04,
        0x08,
        0x10,
        0x08,
};

#define font5x8_width 5
#define font5x8_display_width 7

#define font5x8_last_offset sizeof(font5x8)-font5x8_width

/* index of ? */
#define font5x8_unknow   31

#endif //__LIB_L5_LCD12864_FONT_H__
