#ifndef __SMK_BLE_PROFILE_H
#define __SMK_BLE_PROFILE_H

#include "addr.h"

#define SMK_BLE_PROFILE_NAME_MAX 15

struct smk_ble_profile {
    char name[SMK_BLE_PROFILE_NAME_MAX];
    bt_addr_le_t peer;
};

#endif