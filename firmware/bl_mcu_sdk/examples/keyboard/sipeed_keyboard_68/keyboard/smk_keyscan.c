#include "keyboard/smk_keyscan.h"
#include "keyboard/smk_debounce.h"

#include "task.h"
#include "timers.h"

static inline int smk_keyscan_init_debounce(smk_keyboard_scan_type *scan, smk_keyboard_debounce_algo_type algo)
{
    int ret;

    if (algo < SMK_KEYBOARD_DEBOUNCE_ALGO_COUNT) {
        ret = smk_keyscan_init_debounce_func[algo](scan);
    } else {
        ret = -pdFREERTOS_ERRNO_EINVAL;
    }

    return ret;
}

smk_keyboard_scan_type * smk_keyscan_init(const smk_keyboard_hardware_type * const hardware, QueueHandle_t queue_out)
{
    smk_keyboard_scan_type *scan = pvPortMalloc(sizeof(smk_keyboard_scan_type));
    if (scan == NULL) {
        goto keyscan_malloc_fail;
    }

    scan->hardware = hardware;

    scan->timestamp = (TickType_t)0U;

    scan->key_raw  = NULL;
    scan->key_next = NULL;
    scan->key_last = NULL;
    
    scan->debounce_opaque  = NULL;
    scan->debounce_func    = NULL;
    scan->key_next_updated = 0U;

    scan->queue_out = queue_out;

    // Ensure all the key array to be aligned to 32-bit
    const size_t key_size = ((hardware->matrix.total_cnt + 31U) / 32U) * sizeof(uint32_t);

    uint8_t *key_cache = pvPortMalloc(key_size * 3);
    if (key_cache == NULL) {
        goto keyscan_init_fail;
    }

    for (size_t i = 0; i < key_size * 3 / sizeof(uint32_t); ++i) {
        ((uint32_t *)key_cache)[i] = 0U;
    }
    scan->key_raw  = key_cache;
    scan->key_last = key_cache + key_size;
    scan->key_next = key_cache + key_size * 2;

    if (smk_keyscan_init_debounce(scan, hardware->scan.debounce_algo)) {
        goto keyscan_init_fail;
    }

    if (smk_keyscan_init_gpio(hardware)) {
        goto keyscan_init_fail;
    }

    SCAN_DEBUG("[SMK][KeyScan] init is successful!\r\n");
    SCAN_DEBUG("    Matrix: %ux%u, Layout: %ux%u\r\n", hardware->matrix.col_cnt, hardware->matrix.row_cnt,
        hardware->layout.col_cnt, hardware->layout.row_cnt);
    SCAN_DEBUG("    Scan Period: %ums, Max Jitter: %ums\r\n", hardware->scan.scan_period_ms, hardware->scan.max_jitter_ms);

    return scan;

keyscan_init_fail:
    vPortFree(key_cache);
    vPortFree(scan);
keyscan_malloc_fail:

    SCAN_DEBUG("[SMK][KeyScan] init is failed!\r\n");

    return NULL;
}

__attribute__ ((weak)) uint32_t smk_keyscan_read_entire_row(const smk_keyboard_hardware_type *hardware)
{
    uint32_t ret = 0U;

    for (uint8_t row = 0U; row < hardware->matrix.row_cnt; ++row) {
        if (smk_keyscan_read_row_gpio(hardware, row) == 0U) {
            // Keypress detected
            ret |= 1U << row;
        }
    }

    return ret;
}

void smk_keyscan_read_raw(smk_keyboard_scan_type *scan)
{
    const smk_keyboard_hardware_type *hardware = scan->hardware;
    const uint8_t row_cnt = hardware->matrix.row_cnt;
    const size_t key_size = ((hardware->matrix.total_cnt + 31U) / 32U) * sizeof(uint32_t);
    uint32_t *raw = (uint32_t *)scan->key_raw;

    for (size_t i = 0; i < key_size / sizeof(uint32_t); ++i) {
        raw[i] = 0U;
    }

    uint32_t offset = 0U;
    for (uint8_t col = 0U; col < hardware->matrix.col_cnt; ++col) {
        smk_keyscan_select_col(hardware, col);
        smk_keyscan_select_col_delay(hardware);

        uint32_t row = smk_keyscan_read_entire_row(hardware);

        *raw |= row << offset;
        offset += row_cnt;
        if (offset >= 32U) {
            *++raw = row >> (32U - (offset - row_cnt));
            offset -= 32U;
        }

        smk_keyscan_unselect_col(hardware, col);
        smk_keyscan_unselect_col_delay(hardware);
    }
}

void smk_keyscan_debounce(smk_keyboard_scan_type *scan)
{
    if (scan->debounce_func(scan)) {
        scan->key_next_updated = 1U;
    }
}

void smk_keyscan_commit(smk_keyboard_scan_type *scan)
{
    smk_event_type event;

    if (scan->key_next_updated) {
        const smk_keyboard_hardware_type *hardware = scan->hardware;
        uint32_t *next = (uint32_t *)scan->key_next;
        uint32_t *last = (uint32_t *)scan->key_last;
        const size_t key_size = ((hardware->matrix.total_cnt + 31U) / 32U) * sizeof(uint32_t);

        uint8_t key_pos = 0;
        for (size_t i = 0; i < key_size / sizeof(uint32_t); ++i) {
            uint32_t diff = last[i] ^ next[i];

            while (diff) {
                uint32_t mask = diff & (-diff);
                uint32_t offset = (uint32_t)__builtin_ctz(mask);

                event = (smk_event_type){
                    .class     = SMK_EVENT_KEYPOS,
                    .subclass  = (next[i] & mask) ?
                        SMK_EVENT_KEYPOS_PRESS : SMK_EVENT_KEYPOS_RELEASE,
                    .data      = (smk_event_data_type)(key_pos + offset),
                    .timestamp = scan->timestamp
                };
                xQueueSend(
                    scan->queue_out, // xQueue
                    &event, // pvItemToQueue
                    portMAX_DELAY // xTicksToWait
                );

                SCAN_DEBUG("[SMK][KeyScan] KeyPos %u %s @%u\r\n", event.data, event.subclass == SMK_EVENT_KEYPOS_PRESS ?
                    "pressed" : "released", (uint32_t)event.timestamp);

                diff -= mask;
            }

            last[i] = next[i];
            key_pos += 32U;
        }

        scan->key_next_updated = 0U;
    }

    {
        event = (smk_event_type){
            .class     = SMK_EVENT_KEYPOS,
            .subclass  = SMK_EVENT_KEYPOS_SCAN_END,
            .data      = (smk_event_data_type)0U,
            .timestamp = scan->timestamp
        };
        xQueueSend(
            scan->queue_out, // xQueue
            &event, // pvItemToQueue
            portMAX_DELAY // xTicksToWait
        );
    }
}

static void smk_keyscan_daemon_handler(TimerHandle_t xTimer)
{
    TaskHandle_t task = pvTimerGetTimerID(xTimer);
    xTaskNotifyGive(task);
}

void smk_keyscan_task(void *pvParameters)
{
    smk_keyboard_scan_type *scan = pvParameters;

    SCAN_DEBUG("[SMK][KeyScan] task start...\r\n");

    // Set daemon task to handle keyscan periodically
    TimerHandle_t daemon = xTimerCreate(
        "KeyScan Daemon Task",                              // pcTimerName
        pdMS_TO_TICKS(scan->hardware->scan.scan_period_ms), // xTimerPeriodInTicks
        pdTRUE,                                             // uxAutoReload
        xTaskGetCurrentTaskHandle(),                        // pvTimerID
        smk_keyscan_daemon_handler                          // pxCallbackFunction
    );

    // Start timer
    xTimerStart(
        daemon,       // xTimer
        portMAX_DELAY // xTicksToWait
    );

    for (;;) {
        ulTaskNotifyTake(
            pdTRUE,       // xClearCountOnExit
            portMAX_DELAY // xTicksToWait
        );

        scan->timestamp = xTaskGetTickCount();

        smk_keyscan_read_raw(scan);
        smk_keyscan_debounce(scan);
        smk_keyscan_commit(scan);
    }

    vTaskDelete(NULL);
}
