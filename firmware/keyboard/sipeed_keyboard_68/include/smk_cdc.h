#ifndef __SMK_CDC_H
#define __SMK_CDC_H

#define CDC_IN_EP  0x84
#define CDC_OUT_EP 0x03
#define CDC_INT_EP 0x85
#define SHELL_NAME            "sipeed"

void acm_printf(char *fmt, ...);
void smk_cdc_init();

#endif