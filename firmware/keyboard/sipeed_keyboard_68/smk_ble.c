#include "smk_ble.h"
#include "conn.h"
#include "smk_ble_hog_service.h"
#include "dis.h"
#include "bas.h"
#include "smk_ble_profile.h"
#include "log.h"
#include "smk_event_manager.h"
#include "events/battery_update_event.h"

static uint8_t isRegister = 0;
struct bt_conn *default_conn = NULL;

static struct bt_conn *auth_passkey_entry_conn;
static uint8_t passkey_entries[6] = {0, 0, 0, 0, 0, 0};
static uint8_t passkey_digit = 0;

static struct smk_ble_profile profiles[PROFILE_COUNT];
static uint8_t active_profile;

bool smk_ble_activate_profile_is_open()
{
    return !bt_addr_le_cmp(&profiles[active_profile].peer, BT_ADDR_LE_ANY);
}

void set_profile_address(uint8_t index, const bt_addr_le_t *addr) {
    char setting_name[15];
    char addr_str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));

    memcpy(&profiles[index].peer, addr, sizeof(bt_addr_le_t));
    sprintf(setting_name, "ble/profiles/%d", index);
    BLE_DEBUG("[BLE] Setting profile addr for %s to %s", log_strdup(setting_name), log_strdup(addr_str));
    settings_save_one(setting_name, &profiles[index], sizeof(struct smk_ble_profile));
    // k_work_submit(&raise_profile_changed_event_work);
}

static void smk_ble_connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr)); // TODO: save addr / add wl

    if (err) {
        BLE_DEBUG("[BLE] Failed to connect to %s (%u) \r\n", addr, err);
        return;
    }
    BLE_DEBUG("[BLE] Connected: %s \r\n", addr);

    // int tx_octets = 0x00fb;
    // int tx_time = 0x0848;

    // err = bt_le_set_data_len(conn, tx_octets, tx_time);
    // if (!err) {
    //     BLE_DEBUG("[BLE] ble set data length success\r\n");
    // } else {
    //     BLE_DEBUG("[BLE] ble set data length failure, err: %d\r\n", err);
    // }


    err = bt_conn_le_param_update(conn, BT_LE_CONN_PARAM(0x0006, 0x000c, 30, 400));
    if (err) {
        BLE_DEBUG("[BLE] Failed to update LE parameters (err %d)\r\n", err);
    }

    if (bt_conn_set_security(conn, BT_SECURITY_L2)) {
        BLE_DEBUG("[BLE] Failed to set security\r\n");
    }


    if (!default_conn) {
        default_conn = conn;
    }

}

static void smk_ble_disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    BLE_DEBUG("[BLE] Disconnected: %s (reason %u) \r\n", addr, reason);

    if (default_conn == conn) {
        default_conn = NULL;
    }
    smk_ble_start_adv(); 
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err) {
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        BLE_DEBUG("[BLE] Security changed: %s level %u\r\n", log_strdup(addr), level);
    } else {
        BLE_DEBUG("[BLE] Security failed: %s level %u err %d\r\n", log_strdup(addr), level, err);
    }
}

static void le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency,
                             uint16_t timeout) {
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    BLE_DEBUG("[BLE] %s: interval %d latency %d timeout %d\r\n", log_strdup(addr), interval, latency, timeout);
}

static struct bt_conn_cb conn_callbacks = {
    .connected = smk_ble_connected,
    .disconnected = smk_ble_disconnected,
    .security_changed = security_changed,
    .le_param_updated = le_param_updated,
};

// static void auth_cancel(struct bt_conn *conn) {
//     char addr[BT_ADDR_LE_STR_LEN];

//     bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

//     if (auth_passkey_entry_conn) {
//         bt_conn_unref(auth_passkey_entry_conn);
//         auth_passkey_entry_conn = NULL;
//     }

//     passkey_digit = 0;

//     BLE_DEBUG("[BLE] Pairing cancelled: %s", log_strdup(addr));
// }

static enum bt_security_err auth_pairing_accept(struct bt_conn *conn,
                                                const struct bt_conn_pairing_feat *const feat) {
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);

    // BLE_DEBUG("[BLE] role %d, open? %s\r\n", info.role, smk_ble_active_profile_is_open() ? "yes" : "no");
    // if (info.role == BT_CONN_ROLE_SLAVE && !smk_ble_active_profile_is_open()) {
    //     BLE_DEBUG("[BLE] Rejecting pairing request to taken profile %d\r\n", active_profile);
    //     return BT_SECURITY_ERR_PAIR_NOT_ALLOWED;
    // }
    BLE_DEBUG("[BLE] auth pairing accept \r\n");

    return BT_SECURITY_ERR_SUCCESS;
};


static void auth_pairing_complete(struct bt_conn *conn, bool bonded) {
    struct bt_conn_info info;
    char addr[BT_ADDR_LE_STR_LEN];
    const bt_addr_le_t *dst = bt_conn_get_dst(conn);

    bt_addr_le_to_str(dst, addr, sizeof(addr));
    bt_conn_get_info(conn, &info);

    // if (info.role != BT_CONN_ROLE_SLAVE) {
    //     BLE_DEBUG("[BLE] SKIPPING FOR ROLE %d\r\n", info.role);
    //     return;
    // }

    // if (!smk_ble_active_profile_is_open()) {
    //     BLE_DEBUG("[BLE] Pairing completed but current profile is not open: %s\r\n", log_strdup(addr));
    //     bt_unpair(BT_ID_DEFAULT, dst);
    //     return;
    // }

    BLE_DEBUG("[BLE] Pairing completed open: %s\r\n", log_strdup(addr));

    // set_profile_address(active_profile, dst);
    // update_advertising();
};

static struct bt_conn_auth_cb smk_ble_auth_cb_display = {
    .pairing_confirm = auth_pairing_accept,
    .pairing_complete = auth_pairing_complete,
};

void smk_ble_services_init()
{
    if (!isRegister) {
        isRegister = 1;
        bt_conn_cb_register(&conn_callbacks);

        bt_conn_auth_cb_register(&smk_ble_auth_cb_display);

        // dis_init(0x02, 0xe502, 0xa111, 0x0210); //dis
        bas_init();
        smk_hog_service_init();
        
    }
    BLE_DEBUG("[BLE] Services init\r\n");
}

int smk_ble_hid_notify(uint8_t *data)
{
    if (default_conn)
        return smk_hog_service_notify(default_conn, data);
    else
        return -1;
}

int smk_ble_start_adv(void)
{
    struct bt_le_adv_param adv_param = {
        //options:3, connectable undirected, adv one time
        .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_ONE_TIME),
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
    };

    char *adv_name = SMK_CONFIG_BLE_DEVICE_NAME; // This name must be the same as adv_name in ble_central
    
    uint8_t data[1] = {(BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)};
    uint8_t data_uuid[2] = {0x12, 0x18};//0x1812
    uint8_t data_appearance[2] = {0xC1, 0x03};//0x03C1
    struct bt_data adv_data[] = {
        BT_DATA(BT_DATA_FLAGS, data, 1),
        BT_DATA(BT_DATA_UUID16_ALL, data_uuid, sizeof(data_uuid)),
        BT_DATA(BT_DATA_GAP_APPEARANCE, data_appearance, sizeof(data_appearance)),
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name)),
    };
    BLE_DEBUG("[BLE] ble_start_adv...\r\n");
    return bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
}

void bt_enable_cb(int err)
{
    BLE_DEBUG("[BLE] bt_enable_cb err: %d\r\n", err);
    BLE_DEBUG("[BLE] ble_tp_init...\r\n");
    // MSG("[BLE] Stack:%d\r\n", (int)uxTaskGetStackHighWaterMark(NULL));
    // MSG("[BLE] Heap:%d\r\n", (int)xPortGetFreeHeapSize());
    smk_ble_services_init(); //services init
    // MSG("[BLE] Stack:%d\r\n", (int)uxTaskGetStackHighWaterMark(NULL));
    // MSG("[BLE] Heap:%d\r\n", (int)xPortGetFreeHeapSize());
    BLE_DEBUG("[BLE] ble_start_adv...\r\n");
    smk_ble_start_adv(); 
    // MSG("[BLE] Stack:%d\r\n", (int)uxTaskGetStackHighWaterMark(NULL));
    // MSG("[BLE] Heap:%d\r\n", (int)xPortGetFreeHeapSize());
}

void ble_stack_start(void)
{
    BLE_DEBUG("[BLE] ble_controller_init...\r\n");
    GLB_Set_EM_Sel(GLB_EM_8KB);
    ble_controller_init(configMAX_PRIORITIES - 1);

    // Initialize BLE Host stack
    BLE_DEBUG("[BLE] hci_driver_init...\r\n");
    hci_driver_init();

    BLE_DEBUG("[BLE] bt_enable...\r\n");
    bt_enable(bt_enable_cb);
}

void smk_ble_init_task(void)
{
    ble_stack_start();
}

static int battery_update_event_listener(const smk_event_t *eh){
    const struct battery_update_event *ev = as_battery_update_event(eh);
    if (ev)
    {
        EM_DEBUG("[EVENT] battery_update_event: %d\r\n", ev->level);
        bt_gatt_bas_set_battery_level(ev->level);
    }
    return 0;
}

SMK_LISTENER(battery_update_event_listener, battery_update_event_listener);
SMK_SUBSCRIPTION(battery_update_event_listener, battery_update_event);
