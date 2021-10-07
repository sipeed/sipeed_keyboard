#ifndef __SMK_RGB_COLOR_H
#define __SMK_RGB_COLOR_H

#include "smk_spirgb.h"

enum rgb_color_list_t {
    RGB_COLOR_FIXED_ID = 0,
    RGB_COLOR_RAINBOW_ID,
    RGB_COLOR_RANDOM_ID,
    RGB_COLOR_GRADIENT_ID,
};

uint32_t rgb_lfsr(uint32_t seed);

DRGB RGB_Color_Rainbow(RGB_COLOR_DESC *desc, uint16_t colorpos);
DRGB RGB_Color_Fixed(RGB_COLOR_DESC *desc, uint16_t colorpos);
DRGB RGB_Color_Random(RGB_COLOR_DESC *desc, uint16_t colorpos);
DRGB RGB_Color_Gradient(RGB_COLOR_DESC *desc, uint16_t colorpos);

#endif