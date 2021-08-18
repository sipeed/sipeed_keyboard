#ifndef __SMK_HID_H
#define __SMK_HID_H

#if 1
#include "bflb_platform.h"
#define HID_DEBUG(fmt, ...) MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define HID_DEBUG(fmt, ...)
#endif

#define HID_DESCRIPTOR_LEN 25
#define HID_KEYBOARD_REPORT_DESC_SIZE 63

#define HID_INT_EP          0x81
#define HID_INT_EP_SIZE     8
#define HID_INT_EP_INTERVAL 10

void smk_hid_usb_init();

void smk_usb_hid_daemon_task(void *pvParameters);

#endif