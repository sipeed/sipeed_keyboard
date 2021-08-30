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

#include "smk_hid_protocol.h"

volatile int current_data_interface =DATA_REPORT1_ID;
volatile int current_nkro_interface =NKRO_REPORT_ID;

static atomic_t kb_isupdate=0;
static int kb_idle=0;
static int use_nkro=0;
int kb_configured=0;
int force_basic_keyboard=0;

static const uint8_t hid_keyboard_report_desc[HID_KEYBOARD_REPORT_DESC_SIZE] = {
    STANDERD_KAYBOARD_RD()
};
static const uint8_t hid_data_report_desc[HID_DATA_REPORT_DESC_SIZE] = {
    DATA_PORT_DR(REPORT_ID(1))
    DATA_PORT_DR(REPORT_ID(2))
    DATA_PORT_DR(REPORT_ID(3))
    DATA_PORT_DR(REPORT_ID(4))
};
static const uint8_t hid_nkro_report_desc[HID_NKRO_REPORT_DESC_SIZE] = {
    NKRO_KAYBOARD_RD(REPORT_ID(1))
    ADVANCE_MOUSE_RD(REPORT_ID(2))
};
extern struct device *usb_fs;

static usbd_class_t hid_class;
static usbd_interface_t hid_intf_kb;
static usbd_interface_t hid_intf_data;
static usbd_interface_t hid_intf_nkro;

typedef struct {
    uint8_t buf[2][8];
    uint8_t nkro_buf[2][16];
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
static void smk_usb_hid_add_key_nkro(smk_usb_hid_type *hid_usb, keycode_type keycode){
    uint8_t *buf = hid_usb->nkro_buf[hid_usb->flag ^ 1];

    if (IS_MOD_KEYS(keycode)) {
        buf[0] |= 1U << (keycode - KC_LCTRL);
    } else {
        buf[1 + keycode / 8] |= 1U << (keycode % 8);
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
static void smk_usb_hid_remove_key_nkro(smk_usb_hid_type *hid_usb, keycode_type keycode){

    uint8_t * buf = hid_usb->nkro_buf[hid_usb->flag ^ 1];

    if (IS_MOD_KEYS(keycode)) {
        buf[0] &= ~(1U << (keycode - KC_LCTRL));
    } else {
        buf[1 + keycode / 8] &= ~(1U << (keycode % 8));
    }
}

static void smk_usb_hid_commit(smk_usb_hid_type *hid_usb)
{
    hid_usb->flag ^= 1;
    for (uint8_t idx = 0; idx < 8; ++idx) {
        hid_usb->buf[hid_usb->flag ^ 1][idx] = hid_usb->buf[hid_usb->flag][idx];
    }
    for (uint8_t idx = 0; idx < 16; ++idx) {
        hid_usb->nkro_buf[hid_usb->flag ^ 1][idx] = hid_usb->nkro_buf[hid_usb->flag][idx];
    }
}
static uint32_t times[4]={0};
void usbd_hid_kb_int_callback(uint8_t ep)
{
    times[0]++;
    if(use_nkro&&!force_basic_keyboard)
        return;
    if((!kb_idle)||atomic_set(&kb_isupdate,0))
        usbd_ep_write(HID_KB_INT_EP,hid_usb.buf[hid_usb.flag] , 8, NULL);
}
void usbd_hid_nkro_int_callback(uint8_t ep)
{
    times[1]++;
    static uint8_t inttbuffer[HID_NKRO_INT_EP_SIZE];
    if (use_nkro == 0 ||force_basic_keyboard)
        return;
    if (current_nkro_interface == NKRO_REPORT_ID)
        if (atomic_set(&kb_isupdate, 0)) {
            USBD_LOG_DBG("times:%u,%u,%u,%u\r\n",times[0],times[1],times[2],times[3]);
            inttbuffer[0]=NKRO_REPORT_ID;
            memcpy(inttbuffer + 1, hid_usb.nkro_buf[hid_usb.flag], 16);
            usbd_ep_write(HID_NKRO_INT_EP, inttbuffer, 16 + 1, NULL);
        }
}

static uint8_t data_in_buffer[64];
static uint8_t data_out_buffer[64];
atomic_t dataget=0;
void usbd_hid_data_int_callback(uint8_t ep)
{
    times[2]++;
    if(current_data_interface==DATA_REPORT1_ID&&atomic_set(&dataget,0))
    {
        data_in_buffer[0]=DATA_REPORT1_ID;
        usbd_ep_write(HID_DATA_INT_EP,data_in_buffer , 64, NULL);
    }
}
void usbd_hid_data_out_callback(uint8_t ep)
{
    times[3]++;
    static uint32_t actual_read_length;
    if (usbd_ep_read(HID_DATA_OUT_EP, data_out_buffer, 64, &actual_read_length) < 0) {
        USBD_LOG_DBG("[HID]  Read DATA Packet failed\r\n");
        usbd_ep_set_stall(HID_DATA_OUT_EP);
        return;
    }
    USBD_LOG_DBG("data_get_data:%d\r\n",actual_read_length);
    if(actual_read_length)
    {
        hid_data_protocal_callback(data_out_buffer,data_in_buffer);
        dataget=1;
    }
    usbd_ep_read(HID_DATA_OUT_EP, NULL, 0, NULL);
}
static usbd_endpoint_t hid_kb_in_ep = {
    .ep_cb = usbd_hid_kb_int_callback,
    .ep_addr = HID_KB_INT_EP
};

static usbd_endpoint_t hid_nkro_in_ep = {
    .ep_cb = usbd_hid_nkro_int_callback,
    .ep_addr = HID_NKRO_INT_EP
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

void keyboard_led_cb(uint8_t* data,uint32_t len){
    USBD_LOG_DBG("get_led_data:%d\r\n",len);
}

void nkro_set_idle_callback(uint8_t reportid, uint8_t duration){
    USBD_LOG_DBG("nkro_set_idle_callback:%d,%d\r\n",reportid,duration);
    USBD_LOG_DBG("switch to nkro mode\r\n");
    use_nkro=1;
}

void kb_set_idle_callback(uint8_t reportid, uint8_t duration){
    USBD_LOG_DBG("kb_set_idle_callback:%d,%d\r\n",reportid,duration);
    kb_idle=duration==0;
}

void smk_reset_callback(){
    USBD_LOG_DBG("switch to normal mode\r\n");
    use_nkro=0;
    kb_configured=0;
}

void smk_configured_callback(){
    USBD_LOG_DBG("usb configured\r\n");
    kb_configured=1;
}

void smk_hid_usb_init()
{
    usbd_hid_add_interface(&hid_class, &hid_intf_kb);
    usbd_interface_add_endpoint(&hid_intf_kb, &hid_kb_in_ep);
    usbd_hid_callback_register(hid_intf_kb.intf_num,keyboard_led_cb,NULL,kb_set_idle_callback,NULL,NULL,NULL,smk_reset_callback,smk_configured_callback);
    usbd_hid_report_descriptor_register(hid_intf_kb.intf_num, hid_keyboard_report_desc, HID_KEYBOARD_REPORT_DESC_SIZE);

    usbd_hid_add_interface(&hid_class, &hid_intf_data);
    usbd_interface_add_endpoint(&hid_intf_data, &hid_data_in_ep);
    usbd_interface_add_endpoint(&hid_intf_data, &hid_data_out_ep);
    usbd_hid_report_descriptor_register(hid_intf_data.intf_num, hid_data_report_desc, HID_DATA_REPORT_DESC_SIZE);

    usbd_hid_add_interface(&hid_class, &hid_intf_nkro);
    usbd_interface_add_endpoint(&hid_intf_nkro, &hid_nkro_in_ep);
    usbd_hid_callback_register(hid_intf_nkro.intf_num,keyboard_led_cb,NULL,nkro_set_idle_callback,NULL,NULL,NULL,smk_reset_callback,NULL);
    usbd_hid_report_descriptor_register(hid_intf_nkro.intf_num, hid_nkro_report_desc, HID_NKRO_REPORT_DESC_SIZE);

    hid_data_protocol_init();
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
            smk_usb_hid_add_key_nkro(&hid_usb, (keycode_type)event.data);
            HID_DEBUG("[SMK][HID] KeyCode %u add\r\n", event.data);
            break;

        case SMK_EVENT_KEYCODE_REMOVE:
            smk_usb_hid_remove_key(&hid_usb, (keycode_type)event.data);
            smk_usb_hid_remove_key_nkro(&hid_usb, (keycode_type)event.data);
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
