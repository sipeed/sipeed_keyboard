#include "smk_ble.h"
#include "conn.h"
#include "smk_ble_hog_service.h"
#include "dis.h"
#include "bas.h"
#include "smk_ble_profile.h"
#include "log.h"
#include "smk_event_manager.h"
#include "events/battery_update_event.h"
#include "events/ble_active_profile_changed.h"
#include "easyflash.h"
#include "hal_common.h"

static uint8_t isRegister = 0;
struct bt_conn *default_conn = NULL;

static struct bt_conn *auth_passkey_entry_conn;
static uint8_t passkey_entries[6] = {0, 0, 0, 0, 0, 0};
static uint8_t passkey_digit = 0;
static char device_name[16] = {0};

enum advertising_type {
    SMK_ADV_NONE,
    SMK_ADV_DIR,
    SMK_ADV_CONN,
} advertising_status;

#define CURR_ADV(adv) (adv << 4)

#define SMK_ADV_CONN_NAME                                                                          \
    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME, BT_GAP_ADV_FAST_INT_MIN_2, \
                    BT_GAP_ADV_FAST_INT_MAX_2, NULL)

static struct bt_le_adv_param adv_param = {
    .options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_ONE_TIME),
    .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
    .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
};

static struct smk_ble_profile profiles[PROFILE_COUNT];
static uint8_t active_profile;
#define ENV_BLE_ACTIVE_PROFILE "BLE_ACTIVE_PROFILE"
#define ENV_BLE_ID_SAVE_FLAG   "ble/id_saved"

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data smk_ble_ad[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, device_name, DEVICE_NAME_LEN + 5),
    BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, 0xC1, 0x03),

    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_SOME,
                  0x12, 0x18, /* HID Service */
                  0x0f, 0x18 /* Battery Service */
                  ),
};


static void raise_profile_changed_event() {
    SMK_EVENT_RAISE(new_smk_ble_active_profile_changed((struct smk_ble_active_profile_changed){
        .index = active_profile, .profile = &profiles[active_profile]
    }));
}

static void raise_profile_changed_event_callback(struct k_work *work) {
    raise_profile_changed_event();
}
K_WORK_DEFINE(raise_profile_changed_event_work, raise_profile_changed_event_callback);

bool smk_ble_active_profile_is_open()
{
    return !bt_addr_le_cmp(&profiles[active_profile].peer, BT_ADDR_LE_ANY);
}

void set_profile_address(uint8_t index, const bt_addr_le_t *addr) {
    char setting_name[15];
    char addr_str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));

    memcpy(&profiles[index].peer, addr, sizeof(bt_addr_le_t));
    sprintf(setting_name, "ble/profiles/%d", index);
    BLE_DEBUG("[BLE] Setting profile addr for %s to %s\r\n", log_strdup(setting_name), log_strdup(addr_str));
    ef_set_env_blob(setting_name, &profiles[index], sizeof(struct smk_ble_profile));

    k_work_submit(&raise_profile_changed_event_work);
}

static void load_profiles(){
    char setting_name[15];
    int i;
    for (i = 0; i < PROFILE_COUNT; i++) {
        memset(&profiles[i], 0, sizeof(struct smk_ble_profile));
        sprintf(setting_name, "ble/profiles/%d", i);
        size_t len;
        ef_get_env_blob(setting_name, &profiles[i], sizeof(struct smk_ble_profile), &len);
        if (len == 0)
        {
            BLE_DEBUG("[BLE] No profile found for %s\r\n", log_strdup(setting_name));
        }else{
            BLE_DEBUG("[BLE] Profile found for %s\r\n", log_strdup(setting_name));
        }
    }
}

bool smk_ble_active_profile_is_connected()
{
    struct bt_conn *conn;
    bt_addr_le_t *addr = smk_ble_active_profile_addr();
    if (!bt_addr_cmp(addr, BT_ADDR_LE_ANY)) {
        return false;
    } else if ((conn = bt_conn_lookup_addr_le(BT_ID_DEFAULT, addr)) == NULL) {
        return false;
    }

    bt_conn_unref(conn);
    return true;
}

#define CHECKED_ADV_STOP()                                                                         \
    err = bt_le_adv_stop();                                                                        \
    advertising_status = SMK_ADV_NONE;                                                             \
    if (err) {                                                                                     \
        BLE_DEBUG("Failed to stop advertising (err %d)\r\n", err);                                       \
        return err;                                                                                \
    }

#define CHECKED_DIR_ADV()                                                                          \
    addr = smk_ble_active_profile_addr();                                                          \
    conn = bt_conn_lookup_addr_le(BT_ID_DEFAULT, addr);                                            \
    if (conn != NULL) { /* TODO: Check status of connection */                                     \
        BLE_DEBUG("Skipping advertising, profile host is already connected\r\n");                        \
        bt_conn_unref(conn);                                                                       \
        return 0;                                                                                  \
    }                                                                                              \
    err = bt_le_adv_start(&adv_param, smk_ble_ad, ARRAY_SIZE(smk_ble_ad),   \
                          NULL, 0);                                                                \
    if (err) {                                                                                     \
        BLE_DEBUG("Advertising failed to start (err %d)\r\n", err);                                      \
        return err;                                                                                \
    }                                                                                              \
    advertising_status = SMK_ADV_DIR;

#define CHECKED_OPEN_ADV()                                                                         \
    err = bt_le_adv_start(&adv_param, smk_ble_ad, ARRAY_SIZE(smk_ble_ad), NULL, 0);         \
    if (err) {                                                                                     \
        BLE_DEBUG("Advertising failed to start (err %d)\r\n", err);                                      \
        return err;                                                                                \
    }                                                                                              \
    advertising_status = SMK_ADV_CONN;

int update_advertising(){
    int err = 0;
    bt_addr_le_t *addr;
    struct bt_conn *conn;
    enum advertising_type desired_adv = SMK_ADV_NONE;

    if (smk_ble_active_profile_is_open()) {
        desired_adv = SMK_ADV_CONN;
    } else if (!smk_ble_active_profile_is_connected()) {
        desired_adv = SMK_ADV_CONN;
    }

    BLE_DEBUG("[BLE] Advertising status: %d, desired: %d\r\n", advertising_status, desired_adv);

    switch (desired_adv + CURR_ADV(advertising_status)) {
        case SMK_ADV_NONE + CURR_ADV(SMK_ADV_DIR):
        case SMK_ADV_NONE + CURR_ADV(SMK_ADV_CONN):
            CHECKED_ADV_STOP();
            break;
        case SMK_ADV_DIR + CURR_ADV(SMK_ADV_DIR):
        case SMK_ADV_DIR + CURR_ADV(SMK_ADV_CONN):
            CHECKED_ADV_STOP();
            CHECKED_DIR_ADV();
            break;
        case SMK_ADV_DIR + CURR_ADV(SMK_ADV_NONE):
            CHECKED_DIR_ADV();
            break;
        case SMK_ADV_CONN + CURR_ADV(SMK_ADV_DIR):
            CHECKED_ADV_STOP();
            CHECKED_OPEN_ADV();
            break;
        case SMK_ADV_CONN + CURR_ADV(SMK_ADV_NONE):
            CHECKED_OPEN_ADV();
            break;
    }
    return 0;
}
static void update_advertising_callback(struct k_work *work) {
    update_advertising();
}
K_WORK_DEFINE(update_advertising_work, update_advertising_callback);

int smk_ble_clear_bonds() {
    BLE_DEBUG("[BLE] Clearing all bonds... \r\n");
    if (bt_addr_le_cmp(&profiles[active_profile].peer, BT_ADDR_LE_ANY)) {
        BLE_DEBUG("[BLE] Unpairing! \r\n");
        bt_unpair(BT_ID_DEFAULT, &profiles[active_profile].peer);
        set_profile_address(active_profile, BT_ADDR_LE_ANY);
    }

    for (int i = 0; i < PROFILE_COUNT; i++) {
        char setting_name[15];
        sprintf(setting_name, "ble/profiles/%d", i);

        int err = settings_delete(setting_name);
        if (err) {
            BLE_DEBUG("Failed to delete setting: %d", err);
        }
    }

    update_advertising();
    hal_system_reset(); // FIXME:NOW Unable to restore SC so we reset the system.
    return 0;
}

int smk_ble_active_profile_index() {return active_profile;}

static int ble_save_profile(){
    ef_set_env_blob(ENV_BLE_ACTIVE_PROFILE, &active_profile, sizeof(active_profile));
    return 0;
}

int smk_ble_prof_select(uint8_t index){
    if (index >= PROFILE_COUNT) {
        return -1;
    }

    BLE_DEBUG("[BLE] Selecte profile %d \r\n", index);
    if (index == active_profile) {
        return 0;
    }
    active_profile = index;
    ble_save_profile();

    update_advertising();
    raise_profile_changed_event();
    return 0;
}

int smk_ble_prof_next() {
    BLE_DEBUG("[BLE] Select next profile \r\n");
    return smk_ble_prof_select((active_profile + 1) % PROFILE_COUNT);
}

int smk_ble_prof_prev() {
    BLE_DEBUG("[BLE] Select previous profile \r\n");
    return smk_ble_prof_select((active_profile + PROFILE_COUNT - 1) % PROFILE_COUNT);
}

bt_addr_le_t *smk_ble_active_profile_addr() {
    return &profiles[active_profile].peer;
}

char *smk_ble_active_profile_name() {
    return profiles[active_profile].name;
}

static bool is_conn_active_profile(const struct bt_conn *conn) {
    return bt_addr_le_cmp(bt_conn_get_dst(conn), &profiles[active_profile].peer) == 0;
}

static void smk_ble_connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr)); 

    advertising_status = SMK_ADV_NONE;

    if (err) {
        BLE_DEBUG("[BLE] Failed to connect to %s (%u) \r\n", log_strdup(addr), err);
        update_advertising();
        return;
    }

    BLE_DEBUG("[BLE] Connected: %s \r\n", log_strdup(addr));

    err = bt_conn_le_param_update(conn, BT_LE_CONN_PARAM(0x0006, 0x000c, 30, 400));
    if (err) {
        BLE_DEBUG("[BLE] Failed to update LE parameters (err %d)\r\n", err);
    }

    if (bt_conn_set_security(conn, BT_SECURITY_L2)) {
        BLE_DEBUG("[BLE] Failed to set security\r\n");
    }

    update_advertising();

    if (is_conn_active_profile(conn)){
        BLE_DEBUG("[BLE] Connected to active profile\r\n");
        k_work_submit(&raise_profile_changed_event_work);
    }

}

static void smk_ble_disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    BLE_DEBUG("[BLE] Disconnected: %s (reason %u) \r\n", log_strdup(addr), reason);

    k_work_submit(&update_advertising_work);
    if (is_conn_active_profile(conn)){
        BLE_DEBUG("[BLE] Disconnected from active profile\r\n");
        if (reason == BT_HCI_ERR_REMOTE_USER_TERM_CONN){
            BLE_DEBUG("[BLE] Disconnected from active profile due to user termination\r\n");
            bt_unpair(smk_ble_active_profile_index(), NULL);
            char setting_name[15];
            sprintf(setting_name, "ble/profiles/%d", smk_ble_active_profile_index());

            int err = settings_delete(setting_name);
            if (err) {
                BLE_DEBUG("Failed to delete setting: %d", err);
            }
        }
        k_work_submit(&raise_profile_changed_event_work);
    }
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

static void auth_cancel(struct bt_conn *conn) {
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (auth_passkey_entry_conn) {
        bt_conn_unref(auth_passkey_entry_conn);
        auth_passkey_entry_conn = NULL;
    }

    passkey_digit = 0;

    BLE_DEBUG("[BLE] Pairing cancelled: %s", log_strdup(addr));
}

static enum bt_security_err auth_pairing_accept(struct bt_conn *conn,
                                                const struct bt_conn_pairing_feat *const feat) {
    struct bt_conn_info info;
    bt_conn_get_info(conn, &info);

    BLE_DEBUG("[BLE] role %d, open? %s\r\n", info.role, smk_ble_active_profile_is_open() ? "yes" : "no");
    if (info.role == BT_CONN_ROLE_SLAVE && !smk_ble_active_profile_is_open()) {
        BLE_DEBUG("[BLE] Rejecting pairing request to taken profile %d\r\n", active_profile);
        return BT_SECURITY_ERR_PAIR_NOT_ALLOWED;
    }
    BLE_DEBUG("[BLE] auth pairing accept \r\n");

    return BT_SECURITY_ERR_SUCCESS;
};


static void auth_pairing_complete(struct bt_conn *conn, bool bonded) {
    struct bt_conn_info info;
    char addr[BT_ADDR_LE_STR_LEN];
    const bt_addr_le_t *dst = bt_conn_get_dst(conn);

    bt_addr_le_to_str(dst, addr, sizeof(addr));
    bt_conn_get_info(conn, &info);

    if (info.role != BT_CONN_ROLE_SLAVE) {
        BLE_DEBUG("[BLE] SKIPPING FOR ROLE %d\r\n", info.role);
        return;
    }

    if (!smk_ble_active_profile_is_open()) {
        BLE_DEBUG("[BLE] Pairing completed but current profile is not open: %s\r\n", log_strdup(addr));
        bt_unpair(BT_ID_DEFAULT, dst);
        return;
    }

    BLE_DEBUG("[BLE] Pairing completed open: %s\r\n", log_strdup(addr));

    set_profile_address(active_profile, dst);
    update_advertising();
};

static struct bt_conn_auth_cb smk_ble_auth_cb_display = {
    .pairing_confirm = auth_pairing_accept,
    .pairing_complete = auth_pairing_complete,
    .cancel = auth_cancel,
};

int smk_ble_hid_notify(uint8_t *data)
{
    bt_addr_le_t *addr;
    struct bt_conn *conn;
    int err;
    addr = smk_ble_active_profile_addr();                                                          \
    conn = bt_conn_lookup_addr_le(BT_ID_DEFAULT, addr);
    if (conn)
    {
        err = smk_hog_service_notify(conn, data);
        bt_conn_unref(conn);
    }
    else
        err = -1;
    return err;
}

static void smk_ble_read_local_address()
{
    bt_addr_le_t local_pub_addr;
    bt_addr_le_t local_ram_addr;
    char le_addr[BT_ADDR_LE_STR_LEN];

    bt_get_local_public_address(&local_pub_addr);
    bt_addr_le_to_str(&local_pub_addr, le_addr, sizeof(le_addr));
    sprintf(device_name, "%s_%.2s%.2s", DEVICE_NAME, le_addr, le_addr + 3);
    BLE_DEBUG("[BLE] Local public addr : %s\r\n", le_addr);

    bt_get_local_ramdon_address(&local_ram_addr);
    bt_addr_le_to_str(&local_ram_addr, le_addr, sizeof(le_addr));
    BLE_DEBUG("[BLE] Local random addr : %s\r\n", le_addr);

    bt_set_name(device_name);
    BLE_DEBUG("[BLE] Device name : %s\r\n", device_name);
}


static void smk_ble_ready(int err)
{
    BLE_DEBUG("[BLE] smk_ble_ready err: %d\r\n", err);
    if (err) {
        BLE_DEBUG("[BLE] Failed to enable Bluetooth\r\n");
        return;
    }
    update_advertising();
}

void bt_enable_cb(int err)
{
    BLE_DEBUG("[BLE] bt_enable_cb err: %d\r\n", err);
    bt_conn_cb_register(&conn_callbacks);
    bt_conn_auth_cb_register(&smk_ble_auth_cb_display);

    smk_ble_ready(0);
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
    // bt_enable(bt_enable_cb);
    int err = bt_enable(bt_enable_cb);
    if (err) {
        BLE_DEBUG("[BLE] bt_enable failed: %d\r\n", err);
    }

#ifndef SMK_CONFIG_BLE_CLEAR_BOUNDS_ON_START
#define SMK_CONFIG_BLE_CLEAR_BOUNDS_ON_START 0
#endif

#if SMK_CONFIG_BLE_CLEAR_BOUNDS_ON_START
    // Clear all bounds
    BLE_DEBUG("[BLE] Clear all bounds...\r\n");
    for (int i = 0; i < 10; i++) {
        bt_unpair(i, NULL);
    }
    for (int i = 0; i < PROFILE_COUNT; i++) {
        char setting_name[15];
        sprintf(setting_name, "ble/profiles/%d", i);

        err = settings_delete(setting_name);
        if (err) {
            BLE_DEBUG("Failed to delete setting: %d", err);
        }
    }
#endif // SMK_CONFIG_BLE_CLEAR_BOUNDS_ON_START

#ifndef SMK_CONFIG_BLE_RESET_ID 
#define SMK_CONFIG_BLE_RESET_ID 0
#endif

    bool id_saved_flag = false;
#if !SMK_CONFIG_BLE_RESET_ID 
    if(!ef_get_env_blob(ENV_BLE_ID_SAVE_FLAG, &id_saved_flag, sizeof(id_saved_flag), NULL))
    {
        ef_set_env_blob(ENV_BLE_ID_SAVE_FLAG, &id_saved_flag, sizeof(id_saved_flag));
    }
#endif // !SMK_CONFIG_BLE_RESET_ID
    if(!id_saved_flag)
    {
        bt_id_create(NULL, NULL);
        bt_settings_save_id();
        id_saved_flag = true;
        ef_set_env_blob(ENV_BLE_ID_SAVE_FLAG, &id_saved_flag, sizeof(id_saved_flag));
    }
    smk_ble_read_local_address();
    load_profiles();
    if(!ef_get_env_blob(ENV_BLE_ACTIVE_PROFILE, &active_profile, sizeof(active_profile), NULL))
    {
        active_profile = 0;
    }
    BLE_DEBUG("[BLE] active_profile: %d\r\n", active_profile);
}

void smk_ble_init_task(void)
{
    ble_stack_start();

    // test macro
    BLE_DEBUG("[BLE MACRO TEST] CONFIG_BT_BREDR: %d \r\n",IS_ENABLED(CONFIG_BT_BREDR));
    BLE_DEBUG("[BLE MACRO TEST] CONFIG_BT_SMP: %d \r\n",IS_ENABLED(CONFIG_BT_SMP));
    BLE_DEBUG("[BLE MACRO TEST] CONFIG_BT_SETTINGS: %d \r\n",IS_ENABLED(CONFIG_BT_SETTINGS));
    BLE_DEBUG("[BLE MACRO TEST] CONFIG_BT_BONDABLE: %d \r\n",IS_ENABLED(CONFIG_BT_BONDABLE));

    // services init
    bas_init();
    smk_hog_service_init();
}

void smk_ble_deinit(void)
{
    // services deinit
    bas_deinit();
    smk_hog_service_deinit();
    // BLE deinit
    bt_disable();
    ble_controller_deinit();
}

void smk_ble_clear_bond(int id)
{
    BLE_DEBUG("[BLE] Clear bound %d\r\n", id);
    bt_unpair(BT_ID_DEFAULT, NULL);
    char setting_name[15];
    sprintf(setting_name, "ble/profiles/%d", id);

    int err = settings_delete(setting_name);
    if (err) {
        BLE_DEBUG("Failed to delete setting: %d", err);
    }
}

int smk_ble_unpair_all()
{
    int resp = 0;
    for (int i = BT_ID_DEFAULT; i < CONFIG_BT_ID_MAX; i++){
        int err = bt_unpair(BT_ID_DEFAULT, NULL);
        if (err){
            resp = err;
            BLE_DEBUG("[BLE] Failed to unpair devices (err %d)\r\n", err);
        }
    }
    return resp;
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
