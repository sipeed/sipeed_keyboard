#include "smk_hid.h"
#include "hal_usb.h"
#include "usbd_core.h"
#include "usbd_hid.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "keyboard/smk_event.h"
#include "keyboard/smk_keycode.h"

volatile int current_interface =KEYBOARD_REPORT_ID;

static const uint8_t hid_keyboard_report_desc[HID_KEYBOARD_REPORT_DESC_SIZE] = {
    REPORT_ID(1)
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x06, // USAGE (Keyboard)
    0xa1, 0x01, // COLLECTION (Application)
    0x05, 0x07, // USAGE_PAGE (Keyboard)
    0x19, 0xe0, // USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00, // LOGICAL_MINIMUM (0)
    0x25, 0x01, // LOGICAL_MAXIMUM (1)
    0x75, 0x01, // REPORT_SIZE (1)
    0x95, 0x08, // REPORT_COUNT (8)
    0x81, 0x02, // INPUT (Data,Var,Abs)
    0x95, 0x01, // REPORT_COUNT (1)
    0x75, 0x08, // REPORT_SIZE (8)
    0x81, 0x03, // INPUT (Cnst,Var,Abs)
    0x95, 0x05, // REPORT_COUNT (5)
    0x75, 0x01, // REPORT_SIZE (1)
    0x05, 0x08, // USAGE_PAGE (LEDs)
    0x19, 0x01, // USAGE_MINIMUM (Num Lock)
    0x29, 0x05, // USAGE_MAXIMUM (Kana)
    0x91, 0x02, // OUTPUT (Data,Var,Abs)
    0x95, 0x01, // REPORT_COUNT (1)
    0x75, 0x03, // REPORT_SIZE (3)
    0x91, 0x03, // OUTPUT (Cnst,Var,Abs)
    0x95, 0x06, // REPORT_COUNT (6)
    0x75, 0x08, // REPORT_SIZE (8)
    0x15, 0x00, // LOGICAL_MINIMUM (0)
    0x25, 0xFF, // LOGICAL_MAXIMUM (255)
    0x05, 0x07, // USAGE_PAGE (Keyboard)
    0x19, 0x00, // USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65, // USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00, // INPUT (Data,Ary,Abs)
    0xc0        // END_COLLECTION
    ,
    REPORT_ID(2)
    DATA_PORT_DR
    REPORT_ID(3)
    DATA_PORT_DR
};

extern struct device *usb_fs;

static usbd_class_t hid_class;
static usbd_interface_t hid_intf;

typedef struct {
    uint8_t buf[2][8];
    uint8_t flag;
} smk_usb_hid_type;

static smk_usb_hid_type hid_usb = {
    .buf = { { 0 }, { 0 } },
    .flag = 0
};

static void smk_usb_hid_add_key(smk_usb_hid_type *hid_usb, keycode_type keycode)
{
    uint8_t *buf = hid_usb->buf[hid_usb->flag ^ 1];

    if (IS_MOD_KEYS(keycode)) {
        buf[0] |= 1U << (keycode - KC_LCTRL);
    } else {
        if (buf[7] != KC_NO) {
            return;
        }

        uint8_t idx;
        for (idx = 2; idx < 8; ++idx) {
            if (buf[idx] == KC_NO || buf[idx] == keycode) {
                break;
            } else if (buf[idx] > keycode) {
                for (uint8_t i = 7; i > idx; --i) {
                    buf[i] = buf[i - 1];
                }
                break;
            }
        }
        buf[idx] = keycode;
    }
}

static void smk_usb_hid_remove_key(smk_usb_hid_type *hid_usb, keycode_type keycode)
{
    uint8_t *buf = hid_usb->buf[hid_usb->flag ^ 1];

    if (IS_MOD_KEYS(keycode)) {
        buf[0] &= ~(1U << (keycode - KC_LCTRL));
    } else {
        for (uint8_t idx = 2; idx < 8; ++idx) {
            if (buf[idx] == keycode) {
                for (uint8_t i = idx + 1; i < 8; ++i) {
                    buf[i - 1] = buf[i];
                }
                buf[7] = KC_NO;
                break;
            }
        }
    }
}

static void smk_usb_hid_commit(smk_usb_hid_type *hid_usb)
{
    hid_usb->flag ^= 1;
    for (uint8_t idx = 0; idx < 8; ++idx) {
        hid_usb->buf[hid_usb->flag ^ 1][idx] = hid_usb->buf[hid_usb->flag][idx];
    }
}

void usbd_hid_int_callback(uint8_t ep)
{
    static uint8_t inbuffer[64];
    if(current_interface==KEYBOARD_REPORT_ID)
    {
        inbuffer[0]=KEYBOARD_REPORT_ID;
        memcpy(inbuffer+1,hid_usb.buf[hid_usb.flag],8);
        usbd_ep_write(HID_INT_EP,inbuffer , 9, NULL);
    }

}
void usbd_hid_out_callback(uint8_t ep)
{
//    static uint8_t outbuffer[64];
//    uint32_t readlen;
//    usbd_ep_read(HID_OUT_EP,outbuffer,64,&readlen);
//    if(readlen==0)
//        return;
//    acm_printf("readed:%d bytes report_id: %d\r\n", readlen,outbuffer[0]);//for debug
//    for(int i=0;i<63;i++){
//        acm_printf("0x%02x,", outbuffer[i+1]);//for debug
//    } acm_printf("\r\n");
   // usbd_ep_write(HID_INT_EP, hid_usb.buf[hid_usb.flag], 8, NULL);
}
static usbd_endpoint_t hid_in_ep = {
    .ep_cb = usbd_hid_int_callback,
    .ep_addr = 0x81
};
static usbd_endpoint_t hid_out_ep = {
    .ep_cb = usbd_hid_out_callback,
    .ep_addr = 0x01
};

extern struct device *usb_dc_init(void);

void smk_hid_usb_init()
{
    usbd_hid_add_interface(&hid_class, &hid_intf);
    usbd_interface_add_endpoint(&hid_intf, &hid_in_ep);
    usbd_interface_add_endpoint(&hid_intf, &hid_out_ep);
    usbd_hid_report_descriptor_register(hid_intf.intf_num, hid_keyboard_report_desc, HID_KEYBOARD_REPORT_DESC_SIZE);
}

void smk_usb_hid_daemon_task(void *pvParameters)
{
    QueueHandle_t queue = pvParameters;
    smk_event_type event;

    HID_DEBUG("[SMK][HID] USB HID daemon start...\r\n");

    for (;;) {
        xQueueReceive(
            queue, // xQueue
            &event, // pvBuffer
            portMAX_DELAY // xTicksToWait
        );

        if (event.class != SMK_EVENT_KEYCODE) {
            continue;
        }

        switch (event.subclass) {
        case SMK_EVENT_KEYCODE_ADD:
            smk_usb_hid_add_key(&hid_usb, (keycode_type)event.data);
            HID_DEBUG("[SMK][HID] KeyCode %u add\r\n", event.data);
            break;

        case SMK_EVENT_KEYCODE_REMOVE:
            smk_usb_hid_remove_key(&hid_usb, (keycode_type)event.data);
            HID_DEBUG("[SMK][HID] KeyCode %u remove\r\n", event.data);
            break;

        case SMK_EVENT_KEYCODE_COMMIT:
            smk_usb_hid_commit(&hid_usb);
            HID_DEBUG("[SMK][HID] KeyCode commit\r\n", event.data);
            break;
        }
    }
}
