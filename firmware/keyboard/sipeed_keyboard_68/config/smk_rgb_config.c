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

RGB_COLOR_DESC rgb_color_descriptor[] = {
    {
        .color_func = RGB_Color_Fixed,
        .func_data = {0x66ccff}
    },
    {
        .color_func = RGB_Color_Rainbow,
        .func_data = {1, 0}
    },
    {
        .color_func = RGB_Color_Random,
        .func_data = {0} 
    },
    {
        .color_func = RGB_Color_Gradient,
        .func_data = { 0xfc466b, 0x3f5efb } //配色方案可以去https://www.coocolors.com/browseGradient找
    }
};

RGB_EFF_NODE rgb_effect_list_fixed[] = {
    {
        .eff_id = RGB_EFFECT_FIXED_ID,
        .color_id = RGB_COLOR_RAINBOW_ID,
        .mask_id = 0xFF,
        .blend_type = RGB_BLEND_OVERWRITE,
        .eff_next = 0xFF,
        .eff_var = { 65536/8*65536/100 }
    }
};