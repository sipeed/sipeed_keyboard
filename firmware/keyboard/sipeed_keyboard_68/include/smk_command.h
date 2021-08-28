#include "smk_shell.h"

void hellowd()
{
    acm_printf("hello World\r\n");
}

int echo(int argc, char *argv[])
{
    acm_printf("%dparameter(s)\r\n", argc);

    for (uint8_t i = 1; i < argc; i++) {
        acm_printf("%s\r\n", argv[i]);
    }

    return 0;
}

SHELL_CMD_EXPORT(hellowd, hellowd test)
SHELL_CMD_EXPORT(echo, echo test)