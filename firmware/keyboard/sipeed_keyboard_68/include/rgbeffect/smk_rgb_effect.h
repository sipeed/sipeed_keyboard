#ifndef __SMK_RGB_EFFECT_H
#define __SMK_RGB_EFFECT_H

#include "smk_spirgb.h"

enum rgb_effect_list_t {
    RGB_EFFECT_FIXED_ID = 0,
    RGB_EFFECT_BREATH_ID,
    RGB_EFFECT_DISTANCE_ID,
};

enum rgb_distance_mode_t {
    RGB_DISTANCE_MANHATTAN = 0,
    RGB_DISTANCE_MAXIMUM,
    RGB_DISTANCE_SQRT
};

void RGB_Effect_Fixed(RGB_EFF_NODE *node, uint32_t timestamp);
void RGB_Effect_Breath(RGB_EFF_NODE *node, uint32_t timestamp);
void RGB_Effect_DistanceFlow(RGB_EFF_NODE *node, uint32_t timestamp);

#endif