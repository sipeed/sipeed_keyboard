#include "smk_shell.h"
#include "smk_spirgb.h"


int rgbdemo(int argc, char *argv[])
{
    if (argc == 2 && '0' <= argv[1][0] && argv[1][0] <= '7') {
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