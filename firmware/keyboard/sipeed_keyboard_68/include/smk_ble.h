#ifndef __SMK_BLE_H
#define __SMK_BLE_H

#include "bluetooth.h"
#include "gap.h"
#include "bl702_glb.h"
#include "ble_peripheral_tp_server.h"
#include "ble_lib_api.h"
#include "hci_driver.h"

int ble_start_adv(void);
void bt_enable_cb(int err);
void ble_stack_start(void);
void ble_init(void);

#endif //__SMK_BLE_H