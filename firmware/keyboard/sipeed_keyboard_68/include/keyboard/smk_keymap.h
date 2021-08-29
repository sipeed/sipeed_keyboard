#ifndef __SMK_KEYMAP_H
#define __SMK_KEYMAP_H

#include "smk_keyboard.h"
#include "smk_event.h"

#if 1
#include "bflb_platform.h"
#define MAP_DEBUG(fmt, ...) MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define MAP_DEBUG(fmt, ...)
#endif

typedef enum {
    SMK_KEYBOARD_TAPENGINE_IDLE,
    SMK_KEYBOARD_TAPENGINE_TAPPING,
    SMK_KEYBOARD_TAPENGINE_HOLDING,
    SMK_KEYBOARD_TAPENGINE_POST_TAP,
    SMK_KEYBOARD_TAPENGINE_POST_HOLD
} smk_keyboard_tapengine_state_type;

typedef struct {
    uint8_t          state;
    uint8_t          count;
    smk_keycode_type keycode;
    TickType_t       timestamp;
} smk_keyboard_tapengine_type;

static inline void smk_keymap_send_event(smk_keyboard_map_type *map, const smk_event_type *event)
{
    xQueueSend(
        map->queue_out, // xQueue
        event,          // pvItemToQueue
        portMAX_DELAY   // xTIcksToWait
    );
}

smk_keyboard_map_type * smk_keymap_init(const smk_keyboard_hardware_type * const hardware, QueueHandle_t queue_in, QueueHandle_t queue_out);

void smk_keymap_convert_to_keycode(smk_keyboard_map_type *map, const smk_event_type *event_keypos, smk_event_type *event_keycode);
void smk_keymap_handle_keycode(smk_keyboard_map_type *map, const smk_event_type *event);

void smk_keymap_task(void *pvParameters);

// TapEngine
void smk_keymap_init_tapengine(smk_keyboard_map_type *map);

uint32_t smk_keymap_is_tapping_key(smk_keyboard_map_type *map, smk_keycode_type keycode);
uint8_t smk_keymap_get_tapping_key_id(smk_keyboard_map_type *map, smk_keycode_type keycode);
void smk_keymap_convert_to_tapengine(smk_keyboard_map_type *map, const smk_event_type *event_keycode, smk_event_type *event_tapping);
void smk_keymap_handle_tapengine(smk_keyboard_map_type *map, const smk_event_type *event);

#endif
