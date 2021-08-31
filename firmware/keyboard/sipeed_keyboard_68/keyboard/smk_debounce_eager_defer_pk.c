#include "keyboard/smk_debounce.h"

#include "task.h"

/*
    Debounce Algorithm: EAGER when key-down, DEFER when key-up

    Debounce Structure:

    * state[1:0] (stored {s1, s0}, 2-bit): debounce state
        * 2'b00: idle, neither key-down nor key-up (including pressed and not pressed)
        * 2'b01: key-down, EAGER state
        * 2'b10: key-up, DEFER state, not pressed currently
        * 2'b11: key-up, DEFER state, pressed currently
    * timestamp (32-bit): last state transition time
*/

uint32_t smk_keyscan_debounce_eager_defer_pk(smk_keyboard_scan_type *scan);

int smk_keyscan_init_debounce_eager_defer_pk(smk_keyboard_scan_type *scan)
{
    const smk_keyboard_hardware_type *hardware = scan->hardware;
    const size_t key_size = ((hardware->matrix.total_cnt + 31U) / 32U) * sizeof(uint32_t);

    size_t total_size = hardware->matrix.total_cnt * sizeof(TickType_t); // timestamp
    total_size += 2 * key_size; // state

    void *debounce = pvPortMalloc(total_size);
    if (!debounce) {
        return -pdFREERTOS_ERRNO_ENOMEM;
    }

    for (size_t i = 0; i < total_size / sizeof(uint32_t); ++i) {
        // Simply clear state and timestamp to 0
        ((uint32_t *)debounce)[i] = 0U;
    }

    scan->debounce_opaque = debounce;
    scan->debounce_func = smk_keyscan_debounce_eager_defer_pk;

    return 0;
}

uint32_t smk_keyscan_debounce_eager_defer_pk(smk_keyboard_scan_type *scan)
{
    const smk_keyboard_hardware_type *hardware = scan->hardware;
    const size_t key_size = ((hardware->matrix.total_cnt + 31U) / 32U) * sizeof(uint32_t);
    
    const uint32_t *raw = (uint32_t *)scan->key_raw;
    uint32_t *next = (uint32_t *)scan->key_next;

    TickType_t *timestamp = scan->debounce_opaque;
    uint32_t *s0 = (uint32_t *)(timestamp + hardware->matrix.total_cnt);
    uint32_t *s1 = s0 + key_size / sizeof(uint32_t);

    uint32_t key_changed = 0U;

    uint8_t key_pos = 0;
    for (uint8_t i = 0; i < key_size / sizeof(uint32_t); ++i) {
        {
            // First, check if keys are changed in idle state
            uint32_t sig = ~(s0[i] | s1[i]) & (raw[i] ^ next[i]);
            // If raw = 1'b0 (thus next = 1'b1), set state to 2'b10
            s1[i] |= sig & next[i];
            // If raw = 1'b1 (thus next = 1'b0), set state to 2'b01
            s0[i] |= sig & raw[i];
            // Only key-down will take effect immediately
            next[i] |= sig & raw[i];
            key_changed |= sig & raw[i];
            while (sig) {
                uint32_t mask = sig & (-sig);
                uint32_t offset = (uint32_t)__builtin_ctz(mask);
                // Update timestamp for all the cases
                timestamp[key_pos + offset] = scan->timestamp;
                sig -= mask;
            }
        }

        {
            // Second, check if keys are stable after debounce time (EAGER state)
            uint32_t sig = s0[i] & ~s1[i];
            while (sig) {
                uint32_t mask = sig & (-sig);
                uint32_t offset = (uint32_t)__builtin_ctz(mask);
                if (timestamp[key_pos + offset] - scan->timestamp >= pdMS_TO_TICKS(hardware->scan.max_jitter_ms)) {
                    // Set state to 2'b00
                    s0[i] &= ~mask;
                    // If raw = 1'b0, key state is NOT changed during jitter (because EAGER state is for key-down)
                    next[i] &= ~(mask & ~raw[i]);
                    key_changed |= mask & ~raw[i];
                }
                sig -= mask;
            }
        }

        {
            // Last, check if keys are stable after debounce time (DEFER state)

            // Deal with unstable circumstance
            uint32_t sig = s1[i] & (raw[i] ^ s0[i]);
            while (sig) {
                uint32_t mask = sig & (-sig);
                uint32_t offset = (uint32_t)__builtin_ctz(mask);
                // Use raw to update s0
                s0[i] ^= mask;
                timestamp[key_pos + offset] = scan->timestamp;
                sig -= mask;
            }

            // Deal with stable circumstance
            sig = s1[i] & ~(raw[i] ^ s0[i]);
            while (sig) {
                uint32_t mask = sig & (-sig);
                uint32_t offset = (uint32_t)__builtin_ctz(mask);
                if (timestamp[key_pos + offset] - scan->timestamp >= pdMS_TO_TICKS(hardware->scan.max_jitter_ms)) {
                    // Set state to 2'b00
                    s1[i] &= ~mask;
                    s0[i] &= ~mask;
                    // If raw = 1'b0, key state is changed after jitter (because DEFER state is for key-up)
                    next[i] &= ~(mask & ~raw[i]);
                    key_changed |= mask & ~raw[i];
                }
                sig -= mask;
            }
        }

        key_pos += 32U;
    }

    return key_changed ? 1U : 0U;
}
