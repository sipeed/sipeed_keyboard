/************************************************************************************
rgbeff 序号 效果序号 颜色序号 混合方式 各个eff_var

序号：默认为0

效果序号：
0：固定
1：呼吸
2：距离

颜色序号：
0：固定
1：彩虹
2：随机

混合方式
待定

eff_var参数数量及取值根据效果序号改变

距离模式（序号2）
eff_var[0] = 16.16 移位速度
eff_var[1] = [31:16] 8.8 x 位移比例 [15:0] 8.8 y 位移比例
eff_var[2] = [31:17] 8.7 中心点 x [16:2] 8.7 中心点 y [1:0] 距离法
               0 -> 曼哈顿（菱形） 1 -> 最大值（矩形） 2 -> sqrt（圆形）
eff_var[3] = [31:16] 8.8 上色时间 [15:0] 8.8 上色距离
eff_var[4] = [31:16] 8.8 中心点 [15:8] 4.4 中心宽度 [7:1] 3.4 淡入淡出宽度 [0] 移除 -INOP
            中心点：posv + cdist == timev __s/f-w-\f___


示例：
彩虹流动快速：rgbeff 0 2 1 0 01000000 0f000000 0 02000001
彩虹流动中速：rgbeff  0 2 1 0 00900000 0f000000 0 02000001
彩虹流动超慢速：rgbeff  0 2 1 0 00400000 0f000000 0 02000001
彩虹流动超慢速：rgbeff  0 2 1 0 00300000 0f000000 0 02000001

宽彩虹流动快速 rgbeff 0 2 1 0 00020000 01000000 0 f0000003
宽彩虹流动中速 rgbeff 0 2 1 0 0000e0000 01000000 0 f0000003

配色参数路径：sipeed_keyboard_68\config\smk_rgb_config.c
渐变静止 rgbeff 0 2 3 0 0
渐变流动快速 rgbeff 0 2 3 0 00020000 01000000 0 f0000003
渐变虹流动中速 rgbeff 0 2 3 0 0000e000 01000000 0 f0000003
************************************************************************************/
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
