#include "smk_spirgb.h"
#include "rgbeffect/smk_rgb_effect.h"

DRGB rgb_alpha(DRGB source, uint8_t alpha)
{
    DRGB rval;
    rval.R = (source.R * alpha) >> 8;
    rval.G = (source.G * alpha) >> 8;
    rval.B = (source.B * alpha) >> 8;
    return rval;
}

void blend_color(DRGB *target, DRGB source, uint8_t blend_mode)
{
    switch (blend_mode) {
        case RGB_BLEND_OVERWRITE:
            target->word = (target->D << 24) | (source.word & 0x00FFFFFF);
            break;
        case RGB_BLEND_ADDITIVE:
            if ((int)(target->R) + (int)(source.R) > 255) {
                target->R = 255;
            } else {
                target->R = target->R + source.R;
            }
            if ((int)(target->G) + (int)(source.G) > 255) {
                target->G = 255;
            } else {
                target->G = target->G + source.G;
            }
            if ((int)(target->B) + (int)(source.B) > 255) {
                target->B = 255;
            } else {
                target->B = target->B + source.B;
            }
            break;
        default: // RGB_BLEND_ALPHA:
            target->R = ((int)(target->R) * (256-blend_mode) + (int)(source.R) * blend_mode) >> 8;
            target->G = ((int)(target->G) * (256-blend_mode) + (int)(source.G) * blend_mode) >> 8;
            target->B = ((int)(target->B) * (256-blend_mode) + (int)(source.B) * blend_mode) >> 8;
    }
}

int rgb_abs(int x)
{
    return (x > 0) ? x : (-x);
}

uint32_t rgb_fxsqrt(uint32_t x)
{
    uint64_t xdiv2 = x << 15;
    uint32_t yn = x >> 1;
    uint32_t ynp;
    int32_t ydiff;

    // Bound excecution round to avoid corner cases
    for (int i = 0; i < 16; i++) {
        ynp = (yn>>1) + xdiv2 / yn;
        ydiff = ynp - yn;
        if (32767 > ydiff && ydiff > -32768) {
            break;
        }
        yn = ynp;
    }

    return ynp;
}

uint32_t rgb_mixdistance(uint32_t xdist, uint32_t ydist, int mode)
{
    switch (mode) {
    case RGB_DISTANCE_MANHATTAN:
        return xdist + ydist;
    case RGB_DISTANCE_MAXIMUM:
        return (xdist > ydist) ? xdist : ydist;
    case RGB_DISTANCE_SQRT:
        return rgb_fxsqrt(((xdist * xdist) >> 16) + ((ydist * ydist) >> 16));
    }
    return 0;
}

extern DRGB RGB_Buffer[RGB_LENGTH];

// eff_var[0] = 16.16 color shifting speed 
void RGB_Effect_Fixed(RGB_EFF_NODE *node, uint32_t timestamp)
{
    RGB_COLOR_DESC *color = rgb_color_descriptor + node->color_id;
    DRGB (*color_func)(RGB_COLOR_DESC *, uint16_t) = color->color_func;

    uint32_t speed = node->eff_var[0];
    DRGB recv_color = color_func(color, (timestamp * speed) >> 16);

    for (int i = 0; i < RGB_LENGTH; i++) {
        blend_color(RGB_Buffer+i, recv_color, node->blend_type);
    }
}

// eff_var[0] = 16.16 color shifting speed
// eff_var[1] = 16.16 transparency shifting speed
// eff_var[2] = 16.16 transparency offset
void RGB_Effect_Breath(RGB_EFF_NODE *node, uint32_t timestamp)
{
    RGB_COLOR_DESC *color = rgb_color_descriptor + node->color_id;
    DRGB (*color_func)(RGB_COLOR_DESC *, uint16_t) = color->color_func;

    uint32_t color_speed = node->eff_var[0];
    uint32_t transparency_speed = node->eff_var[1];
    uint32_t transparency_offset = node->eff_var[2];

    DRGB recv_color = color_func(color, (timestamp * color_speed) >> 16);

    uint32_t breath_step = ((timestamp * transparency_speed + transparency_offset) >> 23) & 0x1FF;
    uint32_t vvalue =  (breath_step >= 256) ? (511 - breath_step) : breath_step;

    recv_color = rgb_alpha(recv_color, vvalue);

    for (int i = 0; i < RGB_LENGTH; i++) {
        blend_color(RGB_Buffer+i, recv_color, node->blend_type);
    }
}

// eff_var[0] = 16.16 time-value shifting speed
// eff_var[1] = [31:16] 8.8 x shifting scale [15:0] 8.8 y shifting scale
// eff_var[2] = [31:17] 8.7 center point x [16:2] 8.7 center point y [1:0] distance method
//                0 -> manhattan(diamond) 1 -> maximum(rectangle) 2 -> sqrt(circle)
// eff_var[3] = [31:16] 8.8 time to color [15:0] 8.8 distance to color
// eff_var[4] = [31:16] 8.8 center point [15:8] 4.4 center width [7:1] 3.4 fade width [0] removal - INOP
//                center point: posv + cdist == timev   __s/f-w-\f___
void RGB_Effect_DistanceFlow(RGB_EFF_NODE *node, uint32_t timestamp)
{
    RGB_COLOR_DESC *color = rgb_color_descriptor + node->color_id;
    DRGB (*color_func)(RGB_COLOR_DESC *, uint16_t) = color->color_func;

    uint32_t speed = node->eff_var[0];
    uint32_t xscale = ((node->eff_var[1] >> 16) & 0xFFFF) << 8;
    uint32_t yscale = (node->eff_var[1] & 0xFFFF) << 8;
    uint32_t xcenter = ((node->eff_var[2] >> 17) & 0x7FFF) << 1;
    uint32_t ycenter = ((node->eff_var[2] >> 2) & 0x7FFF) << 1;
    int distance_method = node->eff_var[2] & 0x3;

    uint32_t timescale = (node->eff_var[3] >> 16) & 0xFFFF;
    uint32_t distscale = node->eff_var[3] & 0xFFFF;

    uint32_t center_point = ((node->eff_var[4] >> 16) & 0xFFFF) << 8;
    uint32_t center_width = ((node->eff_var[4] >> 8) & 0xFF) << 12;
    uint32_t fade_width = ((node->eff_var[4] >> 1) & 0x3F) << 12;

    uint32_t time_value = (timestamp * speed) >> 16;
    uint32_t dist_value;

    uint32_t horiz_pos, vert_pos;
    uint16_t color_pos;
    DRGB recv_color;

    for (int i = 0; i < RGB_LENGTH; i++) {
        // Get x, y distance
        // key positions are recorded in 8.8
        horiz_pos = (xscale * rgb_abs((int16_t)(rgb_key_descriptor[i].xpos - xcenter)) ) >> 8;
        vert_pos  = (yscale * rgb_abs((int16_t)(rgb_key_descriptor[i].ypos - ycenter)) ) >> 8;
        // Calculate pos value
        dist_value = rgb_mixdistance(horiz_pos, vert_pos, distance_method);
        color_pos = ((dist_value * distscale) >> 8)
                  + ((time_value * timescale) >> 8);
        // Fetch color
        recv_color = color_func(color, color_pos);
        // Add alpha layer from timev and posv
        if (center_width != 0) {
            uint32_t abscdist = rgb_abs(dist_value + center_point - time_value);
            uint8_t alphaval = 0;
            if (abscdist <= center_width) {
                alphaval = 255;
            } else if (abscdist <= center_width + fade_width) {
                alphaval = ((abscdist - center_width) * 255) / fade_width;
            }
            recv_color = rgb_alpha(recv_color, alphaval);
        }
        // Blend color into buffer
        blend_color(RGB_Buffer+i, recv_color, node->blend_type);
    }

    // Remove self if all points inside visible ring (INOP)
}
