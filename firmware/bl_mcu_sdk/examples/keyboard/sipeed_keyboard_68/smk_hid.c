#include "atomic.h"

#include "smk_hid.h"
#include "hal_usb.h"
#include "usbd_core.h"
#include "usbd_hid.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "keyboard/smk_event.h"
#include "keyboard/smk_keycode.h"

volatile int current_interface =DATA_REPORT1_ID;
static atomic_t kb_isupdate=0;
static const uint8_t hid_keyboard_report_desc[HID_KEYBOARD_REPORT_DESC_SIZE] = {
    STANDERD_KAYBOARD_RD()
};
static const uint8_t hid_data_report_desc[HID_DATA_REPORT_DESC_SIZE] = {
    DATA_PORT_DR(REPORT_ID(1))
    DATA_PORT_DR(REPORT_ID(2))
    DATA_PORT_DR(REPORT_ID(3))
    DATA_PORT_DR(REPORT_ID(4))
};
extern struct device *usb_fs;

static usbd_class_t hid_class;
static usbd_interface_t hid_intf_kb;
static usbd_interface_t hid_intf_data;

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

void usbd_hid_kb_int_callback(uint8_t ep)
{
    if(atomic_set(&kb_isupdate,0))
        usbd_ep_write(HID_KB_INT_EP,hid_usb.buf[hid_usb.flag] , 8, NULL);
}
static uint8_t data_in_buffer[64];
static uint8_t data_out_buffer[64];
atomic_t dataget=0;
void usbd_hid_data_int_callback(uint8_t ep)
{
    if(current_interface==DATA_REPORT1_ID&&atomic_set(&dataget,0))
    {
        data_in_buffer[0]=DATA_REPORT1_ID;
        usbd_ep_write(HID_DATA_INT_EP,data_in_buffer , 64, NULL);
    }
}
void usbd_hid_data_out_callback(uint8_t ep)
{

    static uint32_t actual_read_length;
    if (usbd_ep_read(HID_DATA_OUT_EP, data_out_buffer, 64, &actual_read_length) < 0) {
        USBD_LOG_DBG("[HID]  Read DATA Packet failed\r\n");
        usbd_ep_set_stall(HID_DATA_OUT_EP);
        return;
    }
    USBD_LOG_DBG("data_get_data:%d\r\n",actual_read_length);
    if(actual_read_length)
    {
        memcpy(data_in_buffer,data_out_buffer,64);
        dataget=1;
    }
    usbd_ep_read(HID_DATA_OUT_EP, NULL, 0, NULL);
}
static usbd_endpoint_t hid_kb_in_ep = {
    .ep_cb = usbd_hid_kb_int_callback,
    .ep_addr = HID_KB_INT_EP
};
static usbd_endpoint_t hid_data_in_ep = {
    .ep_cb = usbd_hid_data_int_callback,
    .ep_addr = HID_DATA_INT_EP
};
static usbd_endpoint_t hid_data_out_ep = {
    .ep_cb = usbd_hid_data_out_callback,
    .ep_addr = HID_DATA_OUT_EP
};

extern struct device *usb_dc_init(void);

void keyboard_led_cb(void* data,int len){
    USBD_LOG_DBG("get_data:%d\r\n",len);
}

void smk_hid_usb_init()
{
    usbd_hid_add_interface(&hid_class, &hid_intf_kb);
    usbd_interface_add_endpoint(&hid_intf_kb, &hid_kb_in_ep);
    usbd_hid_set_report_callback_register(keyboard_led_cb);
    usbd_hid_report_descriptor_register(hid_intf_kb.intf_num, hid_keyboard_report_desc, HID_KEYBOARD_REPORT_DESC_SIZE);

    usbd_hid_add_interface(&hid_class, &hid_intf_data);
    usbd_interface_add_endpoint(&hid_intf_data, &hid_data_in_ep);
    usbd_interface_add_endpoint(&hid_intf_data, &hid_data_out_ep);
    usbd_hid_report_descriptor_register(hid_intf_data.intf_num, hid_data_report_desc, HID_DATA_REPORT_DESC_SIZE);
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
            kb_isupdate=true;
            HID_DEBUG("[SMK][HID] KeyCode commit\r\n", event.data);
            break;
        }
    }
}
