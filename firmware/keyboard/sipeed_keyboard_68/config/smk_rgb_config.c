#include "smk_spirgb.h"

#include "rgbeffect/smk_rgb_color.h"
#include "rgbeffect/smk_rgb_effect.h"

RGB_EFF_DESC rgb_effect_descriptor[] = {
    {
        .eff_func = RGB_Effect_Fixed,
        .keyhandler_func = 0
    },
    {
        .eff_func = RGB_Effect_Breath,
        .keyhandler_func = 0
    },
    {
        .eff_func = RGB_Effect_DistanceFlow,
        .keyhandler_func = 0
    }
};

enum rgb_colorcfg_list_t {
    RGB_COLORCFG_FIXEDC_ID = 0,
    RGB_COLORCFG_FIXEDB_ID,
    RGB_COLORCFG_FIXEDG_ID,
    RGB_COLORCFG_FIXEDR_ID,
    RGB_COLORCFG_FIXEDW_ID,
    RGB_COLORCFG_RAINBOW_ID,
    RGB_COLORCFG_6COLOR_ID,
    RGB_COLORCFG_3COLOR_ID,
    RGB_COLORCFG_RANDOM_ID,
    RGB_COLORCFG_GRADIENT_ID
};


RGB_COLOR_DESC rgb_color_descriptor[10] = {
    {
        .color_func = RGB_Color_Fixed,
        .func_data = {0x66ccff}
    },
    {
        .color_func = RGB_Color_Fixed,
        .func_data = {0x0000FF}             // 1: B
    },
    {
        .color_func = RGB_Color_Fixed,
        .func_data = {0x00FF00}             // 2: G
    },
    {
        .color_func = RGB_Color_Fixed,
        .func_data = {0xFF0000}             // 3: R
    },
    {
        .color_func = RGB_Color_Fixed,
        .func_data = {0xFFFFFF}             // 4: W
    },
    {
        .color_func = RGB_Color_Rainbow,
        .func_data = {1, 0}                 // 5: continuous rainbow
    },
    {
        .color_func = RGB_Color_Rainbow,
        .func_data = {0x2AAA, 0}            // 6: 1/6 period = 6 color cycling
    },
    {
        .color_func = RGB_Color_Rainbow,
        .func_data = {0x5555, 0}            // 7: 1/3 period = 3 color cycling
    },
    {
        .color_func = RGB_Color_Random,
        .func_data = {0}                    // 8: LFSR random
    },
    {
        .color_func = RGB_Color_Gradient,
        .func_data = { 0xfc466b, 0x3f5efb } // 9: gradient //配色方案可以去https://www.coocolors.com/browseGradient找
    }
};

RGB_EFF_NODE rgb_effect_list_fixed[8] = {
    {                                           // Full-keyboard rainbow
        .eff_id = RGB_EFFECT_FIXED_ID,
        .color_id = RGB_COLORCFG_RAINBOW_ID,
        .mask_id = 0xFF,
        .blend_type = RGB_BLEND_OVERWRITE,
        .eff_next = 0xFF,
        .eff_var = { 65536/8*65536/100 }
    },
    {                                           // 6-color breathing
        .eff_id = RGB_EFFECT_BREATH_ID,
        .color_id = RGB_COLORCFG_6COLOR_ID,
        .mask_id = 0xFF,
        .blend_type = RGB_BLEND_OVERWRITE,
        .eff_next = 0xFF,
        .eff_var = { 65536/400, (uint32_t)((1LL<<32) / 400), 0}
    },
    {                                           // narrow rainbow flow
        .eff_id = RGB_EFFECT_DISTANCE_ID,
        .color_id = RGB_COLORCFG_RAINBOW_ID,
        .mask_id = 0xFF,
        .blend_type = RGB_BLEND_OVERWRITE,
        .eff_next = 0xFF,
        .eff_var = { 0x00900000, 0x0f000000, 0x0, 0x02000001 }
    },
    {                                           // wide rainbow flow
        .eff_id = RGB_EFFECT_DISTANCE_ID,
        .color_id = RGB_COLORCFG_RAINBOW_ID,
        .mask_id = 0xFF,
        .blend_type = RGB_BLEND_OVERWRITE,
        .eff_next = 0xFF,
        .eff_var = { 0x00020000, 0x01000000, 0x0, 0xF0000003 }
    },
    {                                           // RB gradient
        .eff_id = RGB_EFFECT_DISTANCE_ID,
        .color_id = RGB_COLORCFG_GRADIENT_ID,
        .mask_id = 0xFF,
        .blend_type = RGB_BLEND_OVERWRITE,
        .eff_next = 0xFF,
        .eff_var = { 0x00020000, 0x01000000, 0x0, 0xf0000003 }
    },
    {                                           // Random starlight
        .eff_id = RGB_EFFECT_DISTANCE_ID,
        .color_id = RGB_COLORCFG_RAINBOW_ID,
        .mask_id = 0xFF,
        .blend_type = RGB_BLEND_OVERWRITE,
        .eff_next = 0xFF,
        .eff_var = { 65536/4*65536/100, 0x00100010, 0x00000003, 0x01000100 }
    },
    {                                           // Circle rainbow flow
        .eff_id = RGB_EFFECT_DISTANCE_ID,
        .color_id = RGB_COLORCFG_RAINBOW_ID,
        .mask_id = 0xFF,
        .blend_type = RGB_BLEND_OVERWRITE,
        .eff_next = 0xFF,
        .eff_var = { 0x10000000, 0x00100010, ((uint16_t)(15*256) << 17) | ((uint16_t)(3.8*256) << 2) | 0x02, 0xFFF00200 }
    },
    {                                           // random breathing
        .eff_id = RGB_EFFECT_BREATH_ID,
        .color_id = RGB_COLORCFG_RANDOM_ID,
        .mask_id = 0xFF,
        .blend_type = RGB_BLEND_OVERWRITE,
        .eff_next = 0xFF,
        .eff_var = { 65536/400, (uint32_t)((1LL<<32) / 400), 0}
    },
};