#include "keyboard/smk_keymap.h"

void smk_keymap_init_tapengine(smk_keyboard_map_type *map)
{
    const smk_keyboard_hardware_type *hardware = map->hardware;
    smk_keyboard_tapengine_type *engine = map->tapengine;
    
    for (uint8_t id = 0; id < hardware->map.tapping_key_cnt; ++id) {
        engine[id] = (smk_keyboard_tapengine_type){
            .state     = SMK_KEYBOARD_TAPENGINE_IDLE,
            .count     = 0U,
            .keycode   = (smk_keycode_type)0U,
            .timestamp = (TickType_t)0U
        };
    }

    // Search for tapping keys
    uint8_t engine_id = 0;
    const smk_keycode_type *cur_layer = hardware->map.keymaps;
    for (uint8_t layer = 0; layer < hardware->map.layer_cnt; ++layer) {
        for (uint8_t layout_id = 0; layout_id < hardware->layout.total_cnt; ++layout_id) {
            smk_keycode_type keycode = cur_layer[layout_id];
            if (smk_keymap_is_tapping_key(map, keycode)) {
                // Firstly, search if keycode has appeared before
                uint8_t prev_id;
                for (prev_id = 0; prev_id < engine_id; ++prev_id) {
                    if (engine[prev_id].keycode == keycode) {
                        break;
                    }
                }
                if (prev_id < engine_id) {
                    break;
                }
                // Before add keycode, check if TapEngine has space
                if (engine_id >= hardware->map.tapping_key_cnt) {
                    map->tapping_key_cnt = engine_id;
                    goto tapengine_full;
                }
                engine[engine_id++].keycode = keycode;
            }
        }
        cur_layer += hardware->layout.total_cnt;
    }
    map->tapping_key_cnt = engine_id;

    return;

tapengine_full:
    MAP_DEBUG("[SMK][KeyMap] Warning: tapping keys is more than defined (%u), some of keys are ignored\r\n", hardware->map.tapping_key_cnt);
}

uint32_t smk_keymap_is_tapping_key(smk_keyboard_map_type *map, smk_keycode_type keycode)
{
    return TAP_TOGGLE_LAYER <= keycode && keycode <= TAP_TOGGLE_LAYER_MAX;
}

uint8_t smk_keymap_get_tapping_key_id(smk_keyboard_map_type *map, smk_keycode_type keycode)
{
    smk_keyboard_tapengine_type *engine = map->tapengine;

    for (uint8_t id = 0; id < map->tapping_key_cnt; ++id) {
        if (engine[id].keycode == keycode) {
            MAP_DEBUG("[SMK][KeyMap] tapping key %u has id %u\r\n", keycode, id);
            return id;
        }
    }

    MAP_DEBUG("[SMK][KeyMap] Warning: cannot find tapping key %u\r\n", keycode);
    return 0xFF;
}

void smk_keymap_convert_to_tapengine(smk_keyboard_map_type *map, const smk_event_type *event_keycode, smk_event_type *event_tapping)
{
    const smk_keyboard_hardware_type *hardware = map->hardware;
    smk_keyboard_tapengine_type *engine = map->tapengine;

    smk_keycode_type keycode = (smk_keycode_type)event_keycode->data;
    uint8_t tapping_id = smk_keymap_get_tapping_key_id(map, keycode);

    if (tapping_id >= hardware->map.tapping_key_cnt) {
        *event_tapping = (smk_event_type){
            .class     = SMK_EVENT_NONE,
            .subclass  = 0U,
            .data      = (smk_event_data_type)0U,
            .timestamp = event_keycode->timestamp
        };
        return;
    }

    switch (event_keycode->subclass) {
    case SMK_EVENT_KEYCODE_ADD:
        if (engine[tapping_id].state == SMK_KEYBOARD_TAPENGINE_IDLE) {
            engine[tapping_id].state     = SMK_KEYBOARD_TAPENGINE_TAPPING;
            engine[tapping_id].keycode   = keycode;
            engine[tapping_id].timestamp = event_keycode->timestamp;
            *event_tapping = (smk_event_type){
                .class     = SMK_EVENT_TAPENGINE,
                .subclass  = SMK_EVENT_TAPENGINE_TAPPING_BEGIN,
                .data      = tapping_id,
                .timestamp = event_keycode->timestamp
            };
            MAP_DEBUG("[SMK][KeyMap] tapping key %u begin tapping @%u\r\n", tapping_id, event_tapping->timestamp);
        }
        break;

    case SMK_EVENT_KEYCODE_REMOVE:
        switch (engine[tapping_id].state) {
        case SMK_KEYBOARD_TAPENGINE_TAPPING:
            engine[tapping_id].state = SMK_KEYBOARD_TAPENGINE_POST_TAP;
            engine[tapping_id].count += 1U;
            *event_tapping = (smk_event_type){
                .class     = SMK_EVENT_TAPENGINE,
                .subclass  = SMK_EVENT_TAPENGINE_TAPPING_END,
                .data      = tapping_id,
                .timestamp = event_keycode->timestamp
            };
            MAP_DEBUG("[SMK][KeyMap] tapping key %u end tapping, count = %u @%u\r\n", tapping_id, engine[tapping_id].count, event_tapping->timestamp);
            break;

        case SMK_KEYBOARD_TAPENGINE_HOLDING:
            engine[tapping_id].state = SMK_KEYBOARD_TAPENGINE_POST_HOLD;
            *event_tapping = (smk_event_type){
                .class     = SMK_EVENT_TAPENGINE,
                .subclass  = SMK_EVENT_TAPENGINE_HOLDING_END,
                .data      = tapping_id,
                .timestamp = event_keycode->timestamp
            };
            MAP_DEBUG("[SMK][KeyMap] tapping key %u end holding @%u\r\n", tapping_id, event_tapping->timestamp);
            break;
        }
        break;
    }
}

void smk_keymap_handle_tapengine(smk_keyboard_map_type *map, const smk_event_type *event)
{
    const smk_keyboard_hardware_type *hardware = map->hardware;
    smk_keyboard_tapengine_type *engine = map->tapengine;

    if (event->class == SMK_EVENT_KEYPOS) {
        // event_tapping->subclass must be SMK_EVENT_KEYPOS_SCAN_END
        for (uint8_t id = 0; id < map->tapping_key_cnt; ++id) {
            switch (engine[id].state) {
            case SMK_KEYBOARD_TAPENGINE_IDLE:
                // Check if tapping interval is more than requirement
                if (event->timestamp - engine[id].timestamp >= pdMS_TO_TICKS(hardware->map.tapping_interval_ms)) {
                    engine[id].count = 0;
                    engine[id].timestamp = event->timestamp;
                }
                break;

            case SMK_KEYBOARD_TAPENGINE_TAPPING:
                if (event->timestamp - engine[id].timestamp >= pdMS_TO_TICKS(hardware->map.tapping_term_ms)) {
                    // Pressed time is more than tapping term, start holding
                    engine[id].state = SMK_KEYBOARD_TAPENGINE_HOLDING;
                    MAP_DEBUG("[SMK][KeyMap] tapping key %u begin holding @%u\r\n", id, event->timestamp);
                    smk_event_type event_tapping = {
                        .class     = SMK_EVENT_TAPENGINE,
                        .subclass  = SMK_EVENT_TAPENGINE_HOLDING_BEGIN,
                        .data      = id,
                        .timestamp = event->timestamp
                    };
                    smk_keymap_handle_tapengine(map, &event_tapping);
                }
                break;

            case SMK_KEYBOARD_TAPENGINE_POST_TAP:
                engine[id].state = SMK_KEYBOARD_TAPENGINE_IDLE;
                break;

            case SMK_KEYBOARD_TAPENGINE_POST_HOLD:
                engine[id].state = SMK_KEYBOARD_TAPENGINE_IDLE;
                // Clear tapping count when holding ends
                engine[id].count = 0U;
                break;
            }
        }
    } else if (event->class == SMK_EVENT_TAPENGINE) {
        uint8_t engine_id = (uint8_t)event->data;
        smk_keycode_type keycode = (smk_keycode_type)engine[engine_id].keycode;

        if (TAP_TOGGLE_LAYER <= keycode && keycode <= TAP_TOGGLE_LAYER_MAX) {
            uint8_t count = TT_COUNT(keycode);
            uint8_t layer_id = TT_LAYER(keycode);

            switch (event->subclass) {
            case SMK_EVENT_TAPENGINE_TAPPING_BEGIN:
            case SMK_EVENT_TAPENGINE_HOLDING_BEGIN:
                map->layer_active[layer_id] = 1U;
                MAP_DEBUG("[SMK][KeyMap] layer %u activated momentarily @%u\r\n", layer_id, event->timestamp);
                break;

            case SMK_EVENT_TAPENGINE_TAPPING_END:
                if (engine[engine_id].count >= count) {
                    // Not changing layer_active, because next keycode event will use activated layer
                    engine[engine_id].count = 0U;
                    MAP_DEBUG("[SMK][KeyMap] layer %u activated permanently @%u\r\n", layer_id, event->timestamp);
                } else {
                    map->layer_active[layer_id] = 0U;
                    MAP_DEBUG("[SMK][KeyMap] layer %u deactivated @%u\r\n", layer_id, event->timestamp);
                }
                break;

            case SMK_EVENT_TAPENGINE_HOLDING_END:
                map->layer_active[layer_id] = 0U;
                MAP_DEBUG("[SMK][KeyMap] layer %u deactivated @%u\r\n", layer_id, event->timestamp);
                break;
            }
        }
    }
}
