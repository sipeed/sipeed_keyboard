#ifndef __SMK_BLE_H
#define __SMK_BLE_H

#include "bluetooth.h"
#include "gap.h"
#include "bl702_glb.h"
#include "ble_lib_api.h"
#include "hci_driver.h"

#ifndef EN_BLE_DEBUG
#define EN_BLE_DEBUG 0
#endif

#if EN_BLE_DEBUG
#include "bflb_platform.h"
#define BLE_DEBUG(fmt, ...) MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define BLE_DEBUG(fmt, ...)
#endif

#ifndef SMK_CONFIG_BLE_DEVICE_NAME
#define SMK_CONFIG_BLE_DEVICE_NAME    CONFIG_BT_DEVICE_NAME
#endif

#ifndef SMK_CONFIG_BT_MAX_PAIRED
#define SMK_CONFIG_BT_MAX_PAIRED 3
#endif

#define PROFILE_COUNT SMK_CONFIG_BT_MAX_PAIRED

void smk_ble_services_init();
int smk_ble_hid_notify(uint8_t *data);


int smk_ble_start_adv(void);
void bt_enable_cb(int err);
void smk_ble_stack_start(void);
void smk_ble_init_task(void);

#endif //__SMK_BLE_H