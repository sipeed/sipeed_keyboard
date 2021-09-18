#include "smk_spirgb.h"

const uint8_t maxValue = 0xFF;

const uint16_t maxSaturation = 0xFFFF;

const uint32_t hueEdgeLen = 65537;
const uint32_t maxHue = hueEdgeLen * 6;


DRGB hsv2rgb(uint32_t h, uint16_t s, uint8_t v) {
    DRGB rval;
    rval.word = 0;
    if (s == 0 || v == 0) {
        rval.R = v;
        rval.G = v;
        rval.B = v;
        return rval;
    }

    uint32_t delta = ((s * v) >> 16) + 1;
    uint8_t min = v - delta;
    uint8_t* mid;

    if (h >= hueEdgeLen * 4) {
        h -= hueEdgeLen * 4;
        if (h < hueEdgeLen) {
            rval.B = v;
            rval.G = min;
            mid = &(rval.R);
        } else {
            h -= hueEdgeLen;
            h = hueEdgeLen - h;
            rval.R = v;
            rval.G = min;
            mid = &(rval.B);
        }
    } else if (h >= hueEdgeLen * 2) {
        h -= hueEdgeLen * 2;
        if (h < hueEdgeLen) {
            rval.G = v;
            rval.R = min;
            mid = &(rval.B);
        } else {
            h -= hueEdgeLen;
            h = hueEdgeLen - h;
            rval.B = v;
            rval.R = min;
            mid = &(rval.G);
        }
    } else {
        if (h < hueEdgeLen) {
            rval.R = v;
            rval.B = min;
            mid = &(rval.G);
        } else {
            h -= hueEdgeLen;
            h = hueEdgeLen - h;
            rval.G = v;
            rval.B = min;
            mid = &(rval.R);
        }
    }

    *mid = ((h * delta) >> 16) + min;
    return rval;
}

uint32_t rgb_lfsr(uint32_t seed) {
    for (int r = 0; r < 16; r++) {
        seed = (seed & 1) ? (seed >> 1) ^ 0xA3000000 : (seed >> 1); /* Shift 8 bits the 32-bit LFSR */
    }
    return seed;
}

// Fetching color from HSV hue
// func_data[0] -> Hue step value
// func_data[1] -> Hue offset value
DRGB RGB_Color_Rainbow(RGB_COLOR_DESC *desc, uint16_t colorpos)
{
    return hsv2rgb( ( ((uint64_t)(colorpos + desc->func_data[1]) * desc->func_data[0] * maxHue) >> 16) % maxHue, maxSaturation, maxValue);
}

// Fixed single color
// func_data[0] -> RGB value of the selected color
DRGB RGB_Color_Fixed(RGB_COLOR_DESC *desc, uint16_t colorpos)
{
    return *(DRGB *)(desc->func_data);
}

// Random color from HSV hue
DRGB RGB_Color_Random(RGB_COLOR_DESC *desc, uint16_t colorpos) {
    return hsv2rgb( (((uint64_t)rgb_lfsr(colorpos) * maxHue) >> 16) % maxHue, maxSaturation, maxValue);
}

// Two-color gradient color matching
// func_data[0] -> Hue step value
// func_data[1] -> Hue offset value
DRGB RGB_Color_Gradient(RGB_COLOR_DESC *desc, uint16_t colorpos)
{
    DRGB *gradient_color_first = (DRGB *)&desc->func_data[0];
    DRGB *gradient_color_second = (DRGB *)&desc->func_data[1];

    DRGB gradient_color_rx;

    if (colorpos <= 32768) {
        gradient_color_rx.R = gradient_color_first->R +
                              ((gradient_color_second->R - gradient_color_first->R) * colorpos) / 32768;
        gradient_color_rx.G = gradient_color_first->G +
                              ((gradient_color_second->G - gradient_color_first->G) * colorpos) / 32768;
        gradient_color_rx.B = gradient_color_first->B +
                              ((gradient_color_second->B - gradient_color_first->B) * colorpos) / 32768;
    } else {
        gradient_color_rx.R = gradient_color_second->R -
                              ((gradient_color_second->R - gradient_color_first->R) * (colorpos-32768)) / 32768;
        gradient_color_rx.G = gradient_color_second->G -
                              ((gradient_color_second->G - gradient_color_first->G) * (colorpos-32768)) / 32768;
        gradient_color_rx.B = gradient_color_second->B -
                              ((gradient_color_second->B - gradient_color_first->B) * (colorpos-32768)) / 32768;
    }

    return gradient_color_rx;
}
