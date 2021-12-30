#include "smk_ble_hog_service.h"

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <byteorder.h>
#include <zephyr.h>

#include <bluetooth.h>
#include <conn.h>
#include <uuid.h>
#include <gatt.h>
#include "log.h"
#include "smk_hid.h"
#include "smk_ble.h"
enum
{
    HIDS_REMOTE_WAKE = BIT(0),
    HIDS_NORMALLY_CONNECTABLE = BIT(1),
};

struct hids_info
{
    uint16_t version; /* version number of base USB HID Specification */
    uint8_t code;     /* country HID Device hardware is localized for. */
    uint8_t flags;
} __packed;

struct hids_report
{
    uint8_t id;   /* report id */
    uint8_t type; /* report type */
} __packed;

struct hid_boot_report_body
{
    uint8_t modifiers;
    uint8_t _unused;
    uint8_t keys[6];
} __packed;

static struct hid_boot_report_body keyboard_report_body;

static struct hids_info info = {
    .version = 0x1101, // version
    .code = 0x00,      // country
    .flags = HIDS_NORMALLY_CONNECTABLE,
};

enum
{
    HIDS_INPUT = 0x01,
    HIDS_OUTPUT = 0x02,
    HIDS_FEATURE = 0x03,
};

static struct hids_report input = {
    .id = 0x01,
    .type = HIDS_INPUT,
};

static struct hids_report output = {
    .id = 0x01,
    .type = HIDS_OUTPUT,
};

static uint8_t simulate_input;
static uint8_t ctrl_point;
static uint8_t output_data;
static uint8_t protocal_mode = 0x1;

static const uint8_t hid_keyboard_report_desc[HID_KEYBOARD_REPORT_DESC_SIZE + 2] = {
    STANDERD_KAYBOARD_RD(0x85, 0x01,)}; // page need match hids_report
// static uint8_t report_map[] = {
//     0x05, 0x01, /* Usage Page (Generic Desktop Ctrls) */
//     0x09, 0x02, /* Usage (Mouse) */
//     0xA1, 0x01, /* Collection (Application) */
//     0x09, 0x01, /*   Usage (Pointer) */
//     0xA1, 0x00, /*   Collection (Physical) */
//     0x05, 0x09, /*     Usage Page (Button) */
//     0x19, 0x01, /*     Usage Minimum (0x01) */
//     0x29, 0x03, /*     Usage Maximum (0x03) */
//     0x15, 0x00, /*     Logical Minimum (0) */
//     0x25, 0x01, /*     Logical Maximum (1) */
//     0x95, 0x03, /*     Report Count (3) */
//     0x75, 0x01, /*     Report Size (1) */
//     0x81, 0x02, /*     Input (Data,Var,Abs,No Wrap,Linear,...) */
//     0x95, 0x01, /*     Report Count (1) */
//     0x75, 0x05, /*     Report Size (5) */
//     0x81, 0x03, /*     Input (Const,Var,Abs,No Wrap,Linear,...) */
//     0x05, 0x01, /*     Usage Page (Generic Desktop Ctrls) */
//     0x09, 0x30, /*     Usage (X) */
//     0x09, 0x31, /*     Usage (Y) */
//     0x15, 0x81, /*     Logical Minimum (129) */
//     0x25, 0x7F, /*     Logical Maximum (127) */
//     0x75, 0x08, /*     Report Size (8) */
//     0x95, 0x02, /*     Report Count (2) */
//     0x81, 0x06, /*     Input (Data,Var,Rel,No Wrap,Linear,...) */
//     0xC0,       /*   End Collection */
//     0xC0,       /* End Collection */
// };

static ssize_t read_info(struct bt_conn *conn,
                         const struct bt_gatt_attr *attr, void *buf,
                         uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
                             sizeof(struct hids_info));
}

static ssize_t read_report_map(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr, void *buf,
                               uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, hid_keyboard_report_desc,
                             sizeof(hid_keyboard_report_desc));
}

static ssize_t read_report(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr, void *buf,
                           uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
                             sizeof(struct hids_report));
}

static ssize_t read_protocolmode(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr, void *buf,
                                 uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
                             sizeof(uint8_t));
}

static void input_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    simulate_input = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
    BLE_DEBUG("[BLE][HOG SRV]simulate_input = [%d]\r\n", simulate_input);
}

static ssize_t read_input_report(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr, void *buf,
                                 uint16_t len, uint16_t offset)
{
    // struct hid_boot_report_body *report_body = &keyboard_report_body;
    // return bt_gatt_attr_read(conn, attr, buf, len, offset, report_body, sizeof(struct hid_boot_report_body));
    return bt_gatt_attr_read(conn, attr, buf, len, offset, NULL, 0);
}

static ssize_t write_output_report(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr, void *buf,
                                   uint16_t len, uint16_t offset)
{
    BLE_DEBUG("[BLE][HOG SRV] write output report len: %d\r\n", len);
    return len;
}

static ssize_t write_ctrl_point(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr,
                                const void *buf, uint16_t len, uint16_t offset,
                                uint8_t flags)
{
    uint8_t *value = attr->user_data;

    if (offset + len > sizeof(ctrl_point))
    {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);

    return len;
}

/* HID Service Declaration */
static struct bt_gatt_attr attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_HIDS), // hid service uuid

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_INFO, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, read_info, NULL, &info), // hid info

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT_MAP, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, read_report_map, NULL, NULL), // report map

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ_ENCRYPT,
                           read_input_report, NULL, NULL), // input report
    BT_GATT_CCC(input_ccc_changed,
                BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT), //2902
    BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ,
                       read_report, NULL, &input), // 2908

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT,
                           NULL, write_output_report, &output_data), // output report
    BT_GATT_CCC(NULL,
                BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT), // 2902
    BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                       read_report, NULL, &output), // 2908

    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_CTRL_POINT,
                           BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, write_ctrl_point, &ctrl_point), // hid control
    // BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_PROTOCOL_MODE,
    //                        BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_WRITE,
    //                        BT_GATT_PERM_READ_ENCRYPT|BT_GATT_PERM_WRITE_ENCRYPT,
    //                        NULL, read_protocolmode, &protocal_mode),

};

// struct hids_remote_key {
//     u8_t hid_page;
//     u16_t hid_usage;
// } __packed;

// static struct hids_remote_key remote_kbd_map_tab[] = {
//     { HID_PAGE_KBD, Key_a_or_A2 },
//     { HID_PAGE_KBD, Key_b_or_B },
//     { HID_PAGE_KBD, Key_c_or_C },
// };

int smk_hog_service_notify(struct bt_conn *conn, uint8_t *keyboard_data)
{
    struct bt_gatt_attr *attr = &attrs[BT_CHAR_BLE_HID_REPORT_ATTR_VAL_INDEX];
    // struct hids_remote_key *remote_key = NULL;
    u8_t len = sizeof(keyboard_report_body);
    memcpy(&keyboard_report_body, keyboard_data, sizeof(keyboard_report_body));
    return bt_gatt_notify(conn, attr, &keyboard_report_body, len);
}

struct bt_gatt_service smk_hog_srv = BT_GATT_SERVICE(attrs);

void smk_hog_service_init(void)
{
    bt_gatt_service_register(&smk_hog_srv);
}
