#include "smk_ble.h"

int ble_start_adv(void)
{
    struct bt_le_adv_param adv_param = {
        //options:3, connectable undirected, adv one time
        .options = 3,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_3,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_3,
    };

    char *adv_name = "BL_TEST_01"; // This name must be the same as adv_name in ble_central
    struct bt_data adv_data[] = {
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name)),
    };

    return bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
}

void bt_enable_cb(int err)
{
    ble_tp_init();
    ble_start_adv();
}

void ble_stack_start(void)
{
    MSG("[OS] ble_controller_init...\r\n");
    GLB_Set_EM_Sel(GLB_EM_8KB);
    ble_controller_init(configMAX_PRIORITIES - 1);

    // Initialize BLE Host stack
    MSG("[OS] hci_driver_init...\r\n");
    hci_driver_init();

    MSG("[OS] bt_enable...\r\n");
    bt_enable(bt_enable_cb);
}

void ble_init(void)
{
    ble_stack_start();
}
