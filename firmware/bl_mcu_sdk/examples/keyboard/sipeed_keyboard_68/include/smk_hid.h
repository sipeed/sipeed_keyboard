#ifndef __SMK_HID_H
#define __SMK_HID_H

#if 1
#include "bflb_platform.h"
#define HID_DEBUG(fmt, ...) MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define HID_DEBUG(fmt, ...)
#endif

#define DATA_PORT_DR \
    0x06, 0x00, 0xff,              /*USAGE_PAGE (Vendor Defined Page 1)*/\
    0x09, 0x01,                    /*USAGE (Vendor Usage 1)*/\
    0xa1, 0x01,                    /*COLLECTION (Application)*/\
    0x09, 0x02,                      /*USAGE (Vendor Usage 2)*/\
    0x15, 0x00,                      /*LOGICAL_MINIMUM (0)*/\
    0x26, 0xff, 0x00,                /*LOGICAL_MAXIMUM (255)*/\
    0x75, 0x08,                      /*REPORT_SIZE (8)*/\
    0x95, 0x3f,                      /*REPORT_COUNT (63)*/\
    0x81, 0x02,                      /*INPUT (Data,Var,Abs)*/\
    0x09, 0x02,                      /*USAGE (Vendor Usage 2)*/\
    0x91, 0x02,                      /*OUTPUT (Data,Var,Abs)*/\
    0xc0,                           /*END_COLLECTION*/

#define REPORT_ID(n) 0x85, n ,

enum{
    KEYBOARD_REPORT_ID=1,
    DATA_REPORT1_ID,
    DATA_REPORT2_ID,
    REPORT_ID_COUNT
};

#define HID_DESCRIPTOR_LEN 32
#define HID_KEYBOARD_REPORT_DESC_SIZE (2+63 + 2+DATA_PORT_DR_SIZE + 2+DATA_PORT_DR_SIZE)

#define HID_INT_EP          0x81
#define HID_INT_EP_SIZE     64
#define HID_INT_EP_INTERVAL 10

#define HID_OUT_EP          0x01
#define HID_OUT_EP_SIZE     64
#define HID_OUT_EP_INTERVAL 10

void smk_hid_usb_init();

void smk_usb_hid_daemon_task(void *pvParameters);

#define DATA_PORT_DR_SIZE 25



#endif