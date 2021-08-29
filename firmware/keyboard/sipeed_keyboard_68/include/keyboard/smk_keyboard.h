#ifndef __SMK_KEYBOARD_H
#define __SMK_KEYBOARD_H

#include <stdint.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "smk_keycode.h"

struct smk_keyboard_scan;
typedef uint32_t (*smk_keyboard_debounce_type)(struct smk_keyboard_scan *);

typedef struct {
    struct {
        uint8_t total_cnt;
        uint8_t col_cnt;
        uint8_t row_cnt;

        const uint8_t *col_pins;
        const uint8_t *row_pins;
        const uint8_t *layout_from; // [matrix_id] to layout_id
    } matrix;

    struct {
        uint8_t total_cnt;
        uint8_t col_cnt;
        uint8_t row_cnt;
    } layout;

    struct {
        uint16_t scan_period_ms;
        uint16_t max_jitter_ms;
        uint8_t  debounce_algo;
    } scan;

    struct {
        uint8_t layer_cnt;
        uint8_t default_layer;
        const smk_keycode_type *keymaps; // [layer_id][layout_id] to keycode
        uint16_t tapping_key_cnt;
        uint16_t tapping_term_ms;
        uint16_t tapping_interval_ms;
    } map;
} smk_keyboard_hardware_type;

typedef struct smk_keyboard_scan {
    const smk_keyboard_hardware_type *hardware;

    TickType_t timestamp;

    uint8_t *key_raw;
    uint8_t *key_next;
    uint8_t *key_last;

    // debounce
    void                       *debounce_opaque;
    smk_keyboard_debounce_type  debounce_func;
    uint8_t                     key_next_updated;

    QueueHandle_t queue_out;
} smk_keyboard_scan_type;

typedef struct {
    const smk_keyboard_hardware_type *hardware;

    uint8_t *layer_active; // [layer_id]
    uint8_t *layer_cache;  // [layout_id] to layer_id
    uint8_t  default_layer;
    
    uint8_t  commit_required;

    void    *tapengine;
    uint8_t  tapping_key_cnt;

    QueueHandle_t queue_in;
    QueueHandle_t queue_out;
} smk_keyboard_map_type;

const smk_keyboard_hardware_type * smk_keyboard_get_hardware(void);

int smk_keyscan_init_gpio(const smk_keyboard_hardware_type *hardware);
void smk_keyscan_select_col(const smk_keyboard_hardware_type *hardware, uint32_t col);
void smk_keyscan_unselect_col(const smk_keyboard_hardware_type *hardware, uint32_t col);
void smk_keyscan_select_col_delay(const smk_keyboard_hardware_type *hardware);
void smk_keyscan_unselect_col_delay(const smk_keyboard_hardware_type *hardware);
uint32_t smk_keyscan_read_entire_row(const smk_keyboard_hardware_type *hardware);
uint32_t smk_keyscan_read_row_gpio(const smk_keyboard_hardware_type *hardware, uint32_t row);

#endif
