#include "smk_endpoints.h"

#include "smk_usb.h"
#include "smk_ble.h"
#include "easyflash.h"
#include "FreeRTOS.h"
#include "hal_common.h"

#define SMK_ENV_ENDPOINT_SELECTED "ENDPOINT_SELECTED"

static uint8_t endpoint_init_flag = 0;
smk_endpoint_type_t last_selected_endpoint = SMK_ENDPOINT_USB;
smk_endpoint_type_t current_selected_endpoint = SMK_ENDPOINT_USB;
QueueHandle_t endpoint_switch_queue = NULL;


#if SMK_CONFIG_ENABLE_BLE
extern bool ef_ready_flag;
#else
bool ef_ready_flag = false;
#endif
int check_if_ef_ready()
{
    int err = 0;

    if (!ef_ready_flag) {
        err = easyflash_init();
        if (!err)
            ef_ready_flag = true;
    }

    return err;
}

int smk_endpoint_init()
{
    if(endpoint_init_flag)
        return 0;
    usb_init();
    endpoint_init_flag = 1;
    EP_DEBUG("Check if ef ready\r\n");
    check_if_ef_ready();

    if(!ef_get_env_blob(SMK_ENV_ENDPOINT_SELECTED, &last_selected_endpoint, sizeof(last_selected_endpoint), NULL))
    {
        EP_DEBUG("Get last selected endpoint from flash failed, set as default\r\n");
        ef_set_env_blob(SMK_ENV_ENDPOINT_SELECTED, &last_selected_endpoint, sizeof(last_selected_endpoint));
    }
    current_selected_endpoint = last_selected_endpoint;
}

static void smk_endpoint_select(smk_endpoint_type_t endpoint)
{
    EP_DEBUG("Select endpoint: %d\r\n", endpoint);
    current_selected_endpoint = endpoint;
    if(last_selected_endpoint == endpoint)
        return;
    ef_set_env_blob(SMK_ENV_ENDPOINT_SELECTED, &endpoint, sizeof(endpoint));
    last_selected_endpoint = endpoint;
    if (endpoint == SMK_ENDPOINT_USB) {
        hal_system_reset(); // FIXME: usb can't reopen so we reset the system.
        EP_DEBUG("BLE DEINIT\r\n");
        smk_ble_deinit();
        EP_DEBUG("USB INIT\r\n");
        smk_usb_open();
        EP_DEBUG("USB INIT DONE\r\n");
    } else if (endpoint == SMK_ENDPOINT_BLE) {
        EP_DEBUG("USB DEINIT\r\n");
        smk_usb_close();
        EP_DEBUG("BLE INIT\r\n");
        smk_ble_init_task();
        EP_DEBUG("BLE INIT DONE\r\n");
    }
}

void smk_endpoint_switch_task(void *pvParameters)
{
    smk_endpoint_init();
    if(last_selected_endpoint == SMK_ENDPOINT_USB)
        smk_usb_open();
    else if(last_selected_endpoint == SMK_ENDPOINT_BLE)
        smk_ble_init_task();
    endpoint_switch_queue = xQueueCreate(2, sizeof(smk_endpoint_type_t));
    if (endpoint_switch_queue == NULL) {
        EP_DEBUG("Create endpoint switch queue failed\r\n");
        vTaskDelete(NULL);
        return;
    }

    while (1)
    {
        xQueueReceive(endpoint_switch_queue, &current_selected_endpoint, portMAX_DELAY);
        EP_DEBUG("Receive endpoint switch: %d\r\n", current_selected_endpoint);
        smk_endpoint_select(current_selected_endpoint);
        EP_DEBUG("Endpoint switch done\r\n");
        vTaskDelay(100);
    }
}

void bl_irq_disable()
{
    cpu_global_irq_disable();
}