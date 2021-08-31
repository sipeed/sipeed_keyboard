#ifndef __SMK_HID_H
#define __SMK_HID_H

#ifndef EN_HID_DEBUG
#define EN_HID_DEBUG 0
#endif

#if EN_HID_DEBUG
#include "bflb_platform.h"
#define HID_DEBUG(fmt, ...) MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define HID_DEBUG(fmt, ...)
#endif

//ARG:With or Without Report ID
#define STANDERD_KAYBOARD_DR_SIZE 63
#define STANDERD_KAYBOARD_RD(...) \
    0x05, 0x01, /*USAGE_PAGE (Generic Desktop)*/\
    0x09, 0x06, /*USAGE (Keyboard)*/\
    0xa1, 0x01, /*COLLECTION (Application)*/    \
    __VA_ARGS__                              \
    0x05, 0x07, /*USAGE_PAGE (Keyboard)*/\
    0x19, 0xe0, /*USAGE_MINIMUM (Keyboard LeftControl)*/\
    0x29, 0xe7, /*USAGE_MAXIMUM (Keyboard Right GUI)*/\
    0x15, 0x00,     /*LOGICAL_MINIMUM (0)*/\
    0x25, 0x01,     /*LOGICAL_MAXIMUM (1)*/\
    0x75, 0x01,     /*REPORT_SIZE (1)*/\
    0x95, 0x08,     /*REPORT_COUNT (8)*/\
    0x81, 0x02,     /*INPUT (Data,Var,Abs)*/\
    0x95, 0x01,     /*REPORT_COUNT (1)*/\
    0x75, 0x08,     /*REPORT_SIZE (8)*/\
    0x81, 0x03,     /*INPUT (Cnst,Var,Abs)*/\
    0x95, 0x05,     /*REPORT_COUNT (5)*/\
    0x75, 0x01,     /*REPORT_SIZE (1)*/\
    0x05, 0x08,     /*USAGE_PAGE (LEDs)*/\
    0x19, 0x01,     /*USAGE_MINIMUM (Num Lock)*/\
    0x29, 0x05,     /*USAGE_MAXIMUM (Kana)*/\
    0x91, 0x02,     /*OUTPUT (Data,Var,Abs)*/\
    0x95, 0x01,     /*REPORT_COUNT (1)*/\
    0x75, 0x03,     /*REPORT_SIZE (3)*/\
    0x91, 0x03,     /*OUTPUT (Cnst,Var,Abs)*/\
    0x95, 0x06,     /*REPORT_COUNT (6)*/\
    0x75, 0x08,     /*REPORT_SIZE (8)*/\
    0x15, 0x00,     /*LOGICAL_MINIMUM (0)*/\
    0x25, 0xFF,     /*LOGICAL_MAXIMUM (255)*/\
    0x05, 0x07,     /*USAGE_PAGE (Keyboard)*/\
    0x19, 0x00,     /*USAGE_MINIMUM (Reserved (no event indicated))*/\
    0x29, 0x65,     /*USAGE_MAXIMUM (Keyboard Application)*/\
    0x81, 0x00,     /*INPUT (Data,Ary,Abs)*/\
    0xc0,       /*END_COLLECTION*/


#define DATA_PORT_DR_SIZE 25
#define DATA_PORT_DR(...) \
    0x06, 0x00, 0xff,              /*USAGE_PAGE (Vendor Defined Page 1)*/\
    0x09, 0x01,                    /*USAGE (Vendor Usage 1)*/\
    0xa1, 0x01,                    /*COLLECTION (Application)*/          \
    __VA_ARGS__                              \
    0x09, 0x02,                      /*USAGE (Vendor Usage 2)*/\
    0x15, 0x00,                      /*LOGICAL_MINIMUM (0)*/\
    0x26, 0xff, 0x00,                /*LOGICAL_MAXIMUM (255)*/\
    0x75, 0x08,                      /*REPORT_SIZE (8)*/\
    0x95, 0x3f,                      /*REPORT_COUNT (63)*/\
    0x81, 0x02,                      /*INPUT (Data,Var,Abs)*/\
    0x09, 0x02,                      /*USAGE (Vendor Usage 2)*/\
    0x91, 0x02,                      /*OUTPUT (Data,Var,Abs)*/\
    0xc0,                           /*END_COLLECTION*/

#define NKRO_KAYBOARD_DR_SIZE (57-18)
#define NKRO_KAYBOARD_RD(...) \
    0x05, 0x01, /*USAGE_PAGE (Generic Desktop)*/\
    0x09, 0x06, /*USAGE (Keyboard)*/\
    0xa1, 0x01, /*COLLECTION (Application)*/\
    __VA_ARGS__                              \
    0x05, 0x07, /*USAGE_PAGE (Keyboard)*/\
    0x19, 0xe0, /*USAGE_MINIMUM (Keyboard LeftControl)*/\
    0x29, 0xe7, /*USAGE_MAXIMUM (Keyboard Right GUI)*/\
    0x15, 0x00, /*LOGICAL_MINIMUM (0)*/\
    0x25, 0x01, /*LOGICAL_MAXIMUM (1)*/\
    0x75, 0x01, /*REPORT_SIZE (1)*/\
    0x95, 0x08, /*REPORT_COUNT (8)*/\
    0x81, 0x02, /*INPUT (Data,Var,Abs)*/\
    /*0x95, 0x05,*/ /*REPORT_COUNT (5)*/\
    /*0x75, 0x01,*/ /*REPORT_SIZE (1)*/\
    /*0x05, 0x08,*/ /*USAGE_PAGE (LEDs)*/\
    /*0x19, 0x01,*/ /*USAGE_MINIMUM (Num Lock)*/\
    /*0x29, 0x05,*/ /*USAGE_MAXIMUM (Kana)*/\
    /*0x91, 0x02,*/ /*OUTPUT (Data,Var,Abs)*/\
    /*0x95, 0x01,*/ /*REPORT_COUNT (1)*/\
    /*0x75, 0x03,*/ /*REPORT_SIZE (3)*/\
    /*0x91, 0x03,*/ /*OUTPUT (Cnst,Var,Abs)*/\
    0x05, 0x07, /*USAGE_PAGE (Keyboard)*/\
    0x15, 0x00, /*LOGICAL_MINIMUM (0)*/\
    0x25, 0x01, /*LOGICAL_MAXIMUM (1)*/\
    0x19, 0x00, /*USAGE_MINIMUM (Reserved (no event indicated))*/\
    0x29, 0x65, /*USAGE_MAXIMUM (Keyboard Application)*/\
    0x95, 0x78, /*REPORT_COUNT (120)*/\
    0x75, 0x01, /*REPORT_SIZE (1)*/\
    0x81, 0x02, /*INPUT (Data,Var,Abs)*/\
    0xc0,        /*END_COLLECTION*/

#define ADVANCE_MOUSE_DR_SIZE 67
#define ADVANCE_MOUSE_RD(...) \
    0x05, 0x01,                        /*Usage Page (Generic Desktop)       */\
    0x09, 0x02,                        /*Usage (Mouse)                      */\
    0xa1, 0x01,                        /*Collection (Application)           */\
    __VA_ARGS__                              \
    0x09, 0x01,                        /* Usage (Pointer)                   */\
    0xa1, 0x00,                        /* Collection (Physical)             */\
    0x05, 0x09,                        /*  Usage Page (Button)              */\
    0x19, 0x01,                        /*  Usage Minimum (1)                */\
    0x29, 0x10,                        /*  Usage Maximum (16)               */\
    0x15, 0x00,                        /*  Logical Minimum (0)              */\
    0x25, 0x01,                        /*  Logical Maximum (1)              */\
    0x95, 0x10,                        /*  Report Count (16)                */\
    0x75, 0x01,                        /*  Report Size (1)                  */\
    0x81, 0x02,                        /*  Input (Data,Var,Abs)             */\
    0x05, 0x01,                        /*  Usage Page (Generic Desktop)     */\
    0x16, 0x01, 0xf8,                  /*  Logical Minimum (-2047)          */\
    0x26, 0xff, 0x07,                  /*  Logical Maximum (2047)            */\
    0x75, 0x0c,                        /*  Report Size (12)                  */\
    0x95, 0x02,                        /*  Report Count (2)                  */\
    0x09, 0x30,                        /*  Usage (X)                         */\
    0x09, 0x31,                        /*  Usage (Y)                         */\
    0x81, 0x06,                        /*  Input (Data,Var,Rel)              */\
    0x15, 0x81,                        /*  Logical Minimum (-127)            */\
    0x25, 0x7f,                        /*  Logical Maximum (127)             */\
    0x75, 0x08,                        /*  Report Size (8)                   */\
    0x95, 0x01,                        /*  Report Count (1)                  */\
    0x09, 0x38,                        /*  Usage (Wheel)                     */\
    0x81, 0x06,                        /*  Input (Data,Var,Rel)              */\
    0x05, 0x0c,                        /*  Usage Page (Consumer Devices)     */\
    0x0a, 0x38, 0x02,                  /*  Usage (AC Pan)                    */\
    0x95, 0x01,                        /*  Report Count (1)                  */\
    0x81, 0x06,                        /*  Input (Data,Var,Rel)              */\
    0xc0,                              /* End Collection                     */\
    0xc0,                              /*End Collection                     */

#define REPORT_ID(n) 0x85, n ,

enum{
    DATA_REPORT1_ID=1,
    DATA_REPORT2_ID,
    DATA_REPORT3_ID,
    DATA_REPORT4_ID,
    DATA_REPORT_ID_COUNT
};

enum{
    NKRO_REPORT_ID=1,
    MOUSE_REPORT_ID,
    NKRO_REPORT_ID_COUNT
};

#define HID_KEYBOARD_DESCRIPTOR_LEN 25
#define HID_KEYBOARD_REPORT_DESC_SIZE (STANDERD_KAYBOARD_DR_SIZE )

#define HID_DATA_DESCRIPTOR_LEN 32
#define HID_DATA_REPORT_DESC_SIZE (DATA_PORT_DR_SIZE+2 + DATA_PORT_DR_SIZE+2 + DATA_PORT_DR_SIZE+2 + DATA_PORT_DR_SIZE+2 )

#define HID_NKRO_DESCRIPTOR_LEN 25
#define HID_NKRO_REPORT_DESC_SIZE (NKRO_KAYBOARD_DR_SIZE+2 + ADVANCE_MOUSE_DR_SIZE+2 )

#define HID_KB_INT_EP          0x81
#define HID_KB_INT_EP_SIZE     8
#define HID_KB_INT_EP_INTERVAL 1

#define HID_DATA_INT_EP          0x86
#define HID_DATA_INT_EP_SIZE     64
#define HID_DATA_INT_EP_INTERVAL 1

#define HID_DATA_OUT_EP          0x02
#define HID_DATA_OUT_EP_SIZE     64
#define HID_DATA_OUT_EP_INTERVAL 1

#define HID_NKRO_INT_EP          0x87
#define HID_NKRO_INT_EP_SIZE     17
#define HID_NKRO_INT_EP_INTERVAL 1

void smk_hid_usb_init();

void smk_usb_hid_daemon_task(void *pvParameters);





#endif