#include "smk_shell.h"
#include "smk_spirgb.h"

uint32_t rgb_cmd_htoi(char * val)
{
    uint32_t wrdata = 0;
    for(int i = 0; i < 8; i++) {
        char cval = val[i];
        if (cval == '\0') {
            break;
        }
        wrdata <<= 4;
        if ('f' >= cval && cval >= 'a') {
            wrdata |= cval - 'a' + 10;
        } else if ('F' >= cval && cval >= 'A') {
            wrdata |= cval - 'A' + 10;
        } else if ('9' >= cval && cval >= '0') {
            wrdata |= cval - '0';
        }
    }
    return wrdata;
}

int rgbdemo(int argc, char *argv[])
{
    if (argc == 2 && '0' <= argv[1][0] && argv[1][0] <= '9') {
        rgb_mode = argv[1][0] - '0';
        acm_printf("RGB LED Demo mode %d\r\n", rgb_mode);
        return 0;
    } else {
        acm_printf("Wrong input\r\n");
        acm_printf("Expected: rgbdemo [0-7]\r\n");
        return -1;
    }
}

SHELL_CMD_EXPORT(rgbdemo, RGB LED demo \(rgbdemo \[0-7\] \))

int rgbcolor(int argc, char *argv[])
{
    if (argc >= 3) {
        int color_cnt = argv[1][0] - '0';
        acm_printf("Color: %d\r\n", color_cnt);
        for (int j = 2; j < argc; j++) {
            uint32_t rxval = rgb_cmd_htoi(argv[j]);
            rgb_color_descriptor[color_cnt].func_data[j-2] = rxval;
            acm_printf("    data[%d] = 0x%08X\r\n", j-2, rxval);
        }
        acm_printf("Color set done");
    }
    return 0;
}

SHELL_CMD_EXPORT(rgbcolor, rgbcolor)

int rgbeff(int argc, char *argv[])
{
    if (argc >= 6) {
        int eff_seq = argv[1][0] - '0';
        int eff_id = argv[2][0] - '0';
        int color_id = argv[3][0] - '0';
        int blend_id = rgb_cmd_htoi(argv[4]);
        acm_printf("Effect: %d\r\n", eff_seq);
        acm_printf("    id = %d, color = %d, blend = %d\r\n", eff_id, color_id, blend_id);
        for(int j = 5; j < argc; j++) {
            uint32_t rxval = rgb_cmd_htoi(argv[j]);
            rgb_effect_list_fixed[eff_seq].eff_var[j-5] = rxval;
            acm_printf("    eff_var[%d] = 0x%08X\r\n", j-5, rxval);
        }
        rgb_effect_list_fixed[eff_seq].eff_id = eff_id;
        rgb_effect_list_fixed[eff_seq].color_id = color_id;
        rgb_effect_list_fixed[eff_seq].blend_type = blend_id;
        acm_printf("Effect set done");
    }
    return 0;
}

SHELL_CMD_EXPORT(rgbeff, rgbeff)
