#include "keyboard/smk_debounce.h"

#include "task.h"

typedef enum {
    DEBOUNCE_STABLE,
    DEBOUNCE_UNSTABLE
} smk_keyboard_debounce_state_type;

typedef struct {
    smk_keyboard_debounce_state_type state;
    TickType_t tick_changed;
} smk_keyboard_debounce_t;

int smk_keyscan_init_debounce_defer_g(smk_keyboard_scan_type *scan)
{
    smk_keyboard_debounce_t *debounce = pvPortMalloc(sizeof(smk_keyboard_debounce_t));
    if (!debounce) {
        return -pdFREERTOS_ERRNO_ENOMEM;
    }

    debounce->state = DEBOUNCE_STABLE;
    debounce->tick_changed = (TickType_t)0U;

    scan->debounce_opaque = debounce;
    scan->debounce_func = smk_keyscan_debounce_defer_g;

    return 0;
}

uint32_t smk_keyscan_debounce_defer_g(smk_keyboard_scan_type *scan)
{
    const smk_keyboard_hardware_type *hardware = scan->hardware;
    size_t key_size = (hardware->matrix.total_cnt + 31U) / 32U * sizeof(uint32_t);

    uint32_t *raw  = (uint32_t *)scan->key_raw;
    uint32_t *next = (uint32_t *)scan->key_next;
    
    smk_keyboard_debounce_t *debounce = scan->debounce_opaque;

    // Check if keyboard matrix changed
    uint32_t diff = 0;
    for (uint8_t i = 0; i < key_size / sizeof(uint32_t); ++i) {
        diff |= raw[i] ^ next[i];
    }

    uint32_t ret = 0U;

    switch (debounce->state) {
    case DEBOUNCE_STABLE:
        if (diff) {
            debounce->state = DEBOUNCE_UNSTABLE;
            debounce->tick_changed = scan->timestamp;
            for (uint8_t i = 0; i < key_size / sizeof(uint32_t); ++i) {
                next[i] = raw[i];
            }
        }
        break;

    case DEBOUNCE_UNSTABLE:
        if (diff) {
            debounce->tick_changed = scan->timestamp;
            for (uint8_t i = 0; i < key_size / sizeof(uint32_t); ++i) {
                next[i] = raw[i];
            }
        } else {
            TickType_t max_jitter_tick = pdMS_TO_TICKS(hardware->scan.max_jitter_ms);
            if (scan->timestamp - debounce->tick_changed >= max_jitter_tick) {
                debounce->state = DEBOUNCE_STABLE;
                ret = 1U;
            }
        }
        break;
    }

    return ret;
}
