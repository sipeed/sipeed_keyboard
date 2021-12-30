#ifndef __SMK_BLE_HOG_SERVICE_H
#define __SMK_BLE_HOG_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "conn.h"
#include <types.h>
#define HID_PAGE_KBD  0x07
#define HID_PAGE_CONS 0x0C

#define BT_CHAR_BLE_HID_INFO_ATTR_VAL_INDEX       (2)
#define BT_CHAR_BLE_HID_REPORT_MAP_ATTR_VAL_INDEX (4)
#define BT_CHAR_BLE_HID_REPORT_ATTR_VAL_INDEX     (6)
#define BT_CHAR_BLE_HID_CTRL_POINT_ATTR_VAL_INDEX (10)

void smk_hog_service_init(void);
int smk_hog_service_notify(struct bt_conn *conn, uint8_t *keyboard_data);

#ifdef __cplusplus
}
#endif
#endif