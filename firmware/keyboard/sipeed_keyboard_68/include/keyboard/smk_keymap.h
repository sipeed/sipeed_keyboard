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

smk_keyboard_map_type * smk_keymap_init(const smk_keyboard_hardware_type * const hardware, QueueHandle_t queue_in, QueueHandle_t queue_out);

void smk_keymap_convert_to_keycode(smk_keyboard_map_type *map, const smk_event_type *event_keypos, smk_event_type *event_keycode);
void smk_keymap_handle_keycode(smk_keyboard_map_type *map, smk_event_type *event_keycode);

void smk_keymap_task(void *pvParameters);

#endif
