/**
 * @file shell.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "smk_shell.h"
#include "smk_cdc.h"
#include "hal_usb.h"
#include "usbd_core.h"
#include "usbd_cdc.h"


struct shell_syscall *_syscall_table_begin = NULL;
struct shell_syscall *_syscall_table_end = NULL;
struct shell_sysvar *_sysvar_table_begin = NULL;
struct shell_sysvar *_sysvar_table_end = NULL;

struct shell _shell;
static struct shell *shell;

int shell_help(int argc, char **argv)
{
    // 输出 help 信息
    SHELL_PRINTF("shell commands list:\r\n");
    {
        struct shell_syscall *index;

        // 存储命令名称的字符串前6位为__cmd_
        for (index = _syscall_table_begin; index < _syscall_table_end; index++) {
            if (strncmp(index->name, "__cmd_", 6) != 0) {
                continue;
            }

#if defined(SHELL_USING_DESCRIPTION)
            SHELL_PRINTF("%-16s - %s\r\n", &index->name[6], index->desc);
#else
            SHELL_PRINTF("%s\r\n", &index->name[6]);
#endif
        }
    }
    SHELL_PRINTF("\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_help, help, shell help.);

static char *shell_get_prompt(void)
{
    // 输出 shell 头：bouffalo/>
    static char shell_prompt[SHELL_CONSOLEBUF_SIZE + 1] = { 0 };

    strcpy(shell_prompt, "\r\n");
    strcat(shell_prompt, SHELL_NAME);

    strcat(shell_prompt, "/>");

    return shell_prompt;
}

static int str_common(const char *str1, const char *str2)
{
    const char *str = str1;

    while ((*str != 0) && (*str2 != 0) && (*str == *str2)) {
        str++;
        str2++;
    }

    return (str - str1);
}

static void shell_handle_history(struct shell *shell)
{
    SHELL_PRINTF("\033[2K\r"); // ANSI控制码：删除当前行
    SHELL_PRINTF("%s%s", shell_get_prompt(), shell->line);
}

static void shell_push_history(struct shell *shell)
{
    if (shell->line_position != 0) {
        /* push history */
        if (shell->history_count >= SHELL_HISTORY_LINES) {
            /* if current cmd is same as last cmd, don't push */
            if (memcmp(&shell->cmd_history[SHELL_HISTORY_LINES - 1], shell->line,
                       SHELL_CMD_SIZE)) {
                /* move history */
                int index;

                for (index = 0; index < SHELL_HISTORY_LINES - 1; index++) {
                    memcpy(&shell->cmd_history[index][0],
                           &shell->cmd_history[index + 1][0], SHELL_CMD_SIZE);
                }

                memset(&shell->cmd_history[index][0], 0, SHELL_CMD_SIZE);
                memcpy(&shell->cmd_history[index][0], shell->line,
                       shell->line_position);

                /* it's the maximum history */
                shell->history_count = SHELL_HISTORY_LINES;
            }
        } else {
            /* if current cmd is same as last cmd, don't push */
            if (shell->history_count == 0 ||
                memcmp(&shell->cmd_history[shell->history_count - 1], shell->line,
                       SHELL_CMD_SIZE)) {
                shell->current_history = shell->history_count;
                memset(&shell->cmd_history[shell->history_count][0], 0, SHELL_CMD_SIZE);
                memcpy(&shell->cmd_history[shell->history_count][0], shell->line,
                       shell->line_position);

                /* increase count and set current history position */
                shell->history_count++;
            }
        }
    }

    shell->current_history = shell->history_count;
}


static void shell_auto_complete(char *prefix)
{
    int length, min_length;
    const char *name_ptr, *cmd_name;
    struct shell_syscall *index;

    min_length = 0;
    name_ptr = NULL;

    SHELL_PRINTF("\r\n");

    if (*prefix == '\0') {
        shell_help(0, NULL);
        return;
    }

    /* checks in internal command */
    {
        for (index = _syscall_table_begin; index < _syscall_table_end; index++) {
            /* skip finsh shell function */
            if (strncmp(index->name, "__cmd_", 6) != 0) {
                continue;
            }

            cmd_name = (const char *)&index->name[6];

            if (strncmp(prefix, cmd_name, strlen(prefix)) == 0) {
                if (min_length == 0) {
                    /* set name_ptr */
                    name_ptr = cmd_name;
                    /* set initial length */
                    min_length = strlen(name_ptr);
                }

                length = str_common(name_ptr, cmd_name);

                if (length < min_length) {
                    min_length = length;
                }

                SHELL_PRINTF("%s\r\n", cmd_name);
            }
        }
    }

    /* auto complete string */
    if (name_ptr != NULL) {
        strncpy(prefix, name_ptr, min_length);
    }

    SHELL_PRINTF("%s%s", shell_get_prompt(), prefix);
    return;
}

static int shell_split(char *cmd, uint32_t length, char *argv[SHELL_ARG_NUM])
{
    char *ptr;
    uint32_t position;
    uint32_t argc;
    uint32_t i;

    ptr = cmd;
    position = 0;
    argc = 0;

    while (position < length) {
        /* strip bank and tab */
        while ((*ptr == ' ' || *ptr == '\t') && position < length) {
            *ptr = '\0';
            ptr++;
            position++;
        }

        if (argc >= SHELL_ARG_NUM) {
            SHELL_PRINTF("Too many args ! We only Use:\n");

            for (i = 0; i < argc; i++) {
                SHELL_PRINTF("%s ", argv[i]);
            }

            SHELL_PRINTF("\r\n");
            break;
        }

        if (position >= length) {
            break;
        }

        /* handle string */
        if (*ptr == '"') {
            ptr++;
            position++;
            argv[argc] = ptr;
            argc++;

            /* skip this string */
            while (*ptr != '"' && position < length) {
                if (*ptr == '\\') {
                    if (*(ptr + 1) == '"') {
                        ptr++;
                        position++;
                    }
                }

                ptr++;
                position++;
            }

            if (position >= length) {
                break;
            }

            /* skip '"' */
            *ptr = '\0';
            ptr++;
            position++;
        } else {
            argv[argc] = ptr;
            argc++;

            while ((*ptr != ' ' && *ptr != '\t') && position < length) {
                ptr++;
                position++;
            }

            if (position >= length) {
                break;
            }
        }
    }

    return argc;
}

static cmd_function_t shell_get_cmd(char *cmd, int size)
{
    struct shell_syscall *index;
    cmd_function_t cmd_func = NULL;

    for (index = _syscall_table_begin; index < _syscall_table_end; index++) {
        if (strncmp(index->name, "__cmd_", 6) != 0) {
            continue;
        }

        if (strncmp(&index->name[6], cmd, size) == 0 &&
            index->name[6 + size] == '\0') {
            cmd_func = (cmd_function_t)index->func;
            break;
        }
    }

    return cmd_func;
}

static int shell_exec_cmd(char *cmd, uint32_t length, int *retp)
{
    int argc;
    uint32_t cmd0_size = 0;
    cmd_function_t cmd_func;
    char *argv[SHELL_ARG_NUM];

    // ASSERT(cmd);
    // ASSERT(retp);

    /* find the size of first command */
    while ((cmd[cmd0_size] != ' ' && cmd[cmd0_size] != '\t') &&
           cmd0_size < length) {
        cmd0_size++;
    }

    if (cmd0_size == 0) {
        return -1;
    }

    cmd_func = shell_get_cmd(cmd, cmd0_size);

    if (cmd_func == NULL) {
        return -1;
    }

    /* split arguments */
    memset(argv, 0x00, sizeof(argv));
    argc = shell_split(cmd, length, argv);

    if (argc == 0) {
        return -1;
    }

    /* exec this command */
    *retp = cmd_func(argc, argv);
    return 0;
}

int shell_exec(char *cmd, uint32_t length)
{
    int cmd_ret;

    /* strim the beginning of command */
    while (*cmd == ' ' || *cmd == '\t') {
        cmd++;
        length--;
    }

    if (length == 0) {
        return 0;
    }

    /* Exec sequence:
   * 1. built-in command
   * 2. module(if enabled)
   */
    if (shell_exec_cmd(cmd, length, &cmd_ret) == 0) {
        return cmd_ret;
    }

#ifdef SHELL_USING_LWIP

    if (shell_exec_lwp(cmd, length) == 0) {
        return 0;
    }

#endif

    /* truncate the cmd at the first space. */
    {
        char *tcmd;
        tcmd = cmd;

        while (*tcmd != ' ' && *tcmd != '\0') {
            tcmd++;
        }

        *tcmd = '\0';
    }
    SHELL_PRINTF("%s: command not found.\r\n", cmd);
    return -1;
}

void shell_handler(uint8_t data)
{
    /*
   * handle control key
   * up key  : 0x1b 0x5b 0x41
   * down key: 0x1b 0x5b 0x42
   * right key:0x1b 0x5b 0x43
   * left key: 0x1b 0x5b 0x44
   */
    if (data == 0x1b) {
        shell->stat = WAIT_SPEC_KEY;
        return;
    } else if (shell->stat == WAIT_SPEC_KEY) {
        if (data == 0x5b) {
            shell->stat = WAIT_FUNC_KEY;
            return;
        }

        shell->stat = WAIT_NORMAL;
    } else if (shell->stat == WAIT_FUNC_KEY) {
        shell->stat = WAIT_NORMAL;

        if (data == 0x41) /* up key */
        {
            /* prev history */
            if (shell->current_history > 0) {
                shell->current_history--;
            } else {
                shell->current_history = 0;
                return;
            }

            /* copy the history command */
            memcpy(shell->line, &shell->cmd_history[shell->current_history][0],
                   SHELL_CMD_SIZE);
            shell->line_curpos = shell->line_position = strlen(shell->line);
            shell_handle_history(shell);

            return;
        } else if (data == 0x42) /* down key */
        {
            /* next history */
            if (shell->current_history < shell->history_count - 1) {
                shell->current_history++;
            } else {
                /* set to the end of history */
                if (shell->history_count != 0) {
                    shell->current_history = shell->history_count - 1;
                } else {
                    return;
                }
            }

            memcpy(shell->line, &shell->cmd_history[shell->current_history][0],
                   SHELL_CMD_SIZE);
            shell->line_curpos = shell->line_position = strlen(shell->line);
            shell_handle_history(shell);

            return;
        } else if (data == 0x44) /* left key */
        {
            if (shell->line_curpos) {
                SHELL_PRINTF("\b");
                shell->line_curpos--;
            }

            return;
        } else if (data == 0x43) /* right key */
        {
            if (shell->line_curpos < shell->line_position) {
                SHELL_PRINTF("%c", shell->line[shell->line_curpos]);
                shell->line_curpos++;
            }
            return;
        }
    }

    /* received null or error */
    if (data == '\0' || data == 0xFF) {
        return;
    }
    /* handle tab key */
    else if (data == '\t') {
        int i;

        /* move the cursor to the beginning of line */
        for (i = 0; i < shell->line_curpos; i++) {
            SHELL_PRINTF("\b");
        }

        /* auto complete */
        shell_auto_complete(&shell->line[0]);
        /* re-calculate position */
        shell->line_curpos = shell->line_position = strlen(shell->line);

        return;
    }
    /* handle backspace key */
    else if (data == 0x7f || data == 0x08) {
        /* note that shell->line_curpos >= 0 */
        if (shell->line_curpos == 0) {
            return;
        }

        shell->line_position--;
        shell->line_curpos--;

        if (shell->line_position > shell->line_curpos) {
            int i;

            memmove(&shell->line[shell->line_curpos],
                    &shell->line[shell->line_curpos + 1],
                    shell->line_position - shell->line_curpos);
            shell->line[shell->line_position] = 0;

            SHELL_PRINTF("\b%s  \b", &shell->line[shell->line_curpos]);

            /* move the cursor to the origin position */
            for (i = shell->line_curpos; i <= shell->line_position; i++) {
                SHELL_PRINTF("\b");
            }
        } else {
            SHELL_PRINTF("\b \b");
            shell->line[shell->line_position] = 0;
        }

        return;
    }

    /* handle end of line, break */
    if (data == '\r' || data == '\n') {
        shell_push_history(shell);

        SHELL_PRINTF("\r\n");
        shell_exec(shell->line, shell->line_position);

        SHELL_PRINTF(shell_get_prompt());
        memset(shell->line, 0, sizeof(shell->line));
        shell->line_curpos = shell->line_position = 0;
        return;
    }

    /* it's a large line, discard it */
    if (shell->line_position >= SHELL_CMD_SIZE) {
        shell->line_position = 0;
    }

    /* normal character */
    if (shell->line_curpos < shell->line_position) {
        int i;

        memmove(&shell->line[shell->line_curpos + 1],
                &shell->line[shell->line_curpos],
                shell->line_position - shell->line_curpos);
        shell->line[shell->line_curpos] = data;

        SHELL_PRINTF("%s", &shell->line[shell->line_curpos]);

        /* move the cursor to new position */
        for (i = shell->line_curpos; i < shell->line_position; i++) {
            SHELL_PRINTF("\b");
        }
    } else {
        shell->line[shell->line_position] = data;
        SHELL_PRINTF("%c", data);
    }

    data = 0;
    shell->line_position++;
    shell->line_curpos++;

    if (shell->line_position >= SHELL_CMD_SIZE) {
        /* clear command line */
        shell->line_position = 0;
        shell->line_curpos = 0;
    }
}

static void shell_function_init(const void *begin, const void *end)
{
    _syscall_table_begin = (struct shell_syscall *)begin;
    _syscall_table_end = (struct shell_syscall *)end;
}

static void shell_var_init(const void *begin, const void *end)
{
    _sysvar_table_begin = (struct shell_sysvar *)begin;
    _sysvar_table_end = (struct shell_sysvar *)end;
}

/*
 * @ingroup shell
 *
 * This function will initialize shell
 */
void shell_init(void)
{
#if defined(__CC_ARM) || defined(__CLANG_ARM) /* ARM C Compiler */
    extern const int FSymTab$$Base;
    extern const int FSymTab$$Limit;
    extern const int VSymTab$$Base;
    extern const int VSymTab$$Limit;
    shell_function_init(&FSymTab$$Base, &FSymTab$$Limit);
    shell_var_init(&VSymTab$$Base, &VSymTab$$Limit);
#elif defined(__ICCARM__) || defined(__ICCRX__) /* for IAR Compiler */
    shell_function_init(__section_begin("FSymTab"), __section_end("FSymTab"));
    shell_var_init(__section_begin("VSymTab"), __section_end("VSymTab"));
#elif defined(__GNUC__)
    /* GNU GCC Compiler and TI CCS */
    extern const int __fsymtab_start;
    extern const int __fsymtab_end;
    extern const int __vsymtab_start;
    extern const int __vsymtab_end;
    shell_function_init(&__fsymtab_start, &__fsymtab_end);
    shell_var_init(&__vsymtab_start, &__vsymtab_end);
#endif
    shell = &_shell;
}

void acm_printf(char *fmt, ...)
{
    char print_buf[64];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(print_buf, sizeof(print_buf) - 1, fmt, ap);
    va_end(ap);
#if ((defined BOOTROM) || (defined BFLB_EFLASH_LOADER))
    uint32_t len = strlen(print_buf);
    if (log_len + len < sizeof(eflash_loader_logbuf)) {
        memcpy(eflash_loader_logbuf + log_len, print_buf, len);
        log_len += len;
    }
#endif
    usbd_ep_write(CDC_IN_EP,(uint8_t *)print_buf,strlen(print_buf),NULL);
}