#include "keyboard/smk_keymap.h"

static inline void smk_keymap_send_event(smk_keyboard_map_type *map, uint8_t class, uint8_t subclass, smk_event_data_type data, TickType_t timestamp)
{
    smk_event_type event = {
        .class     = class,
        .subclass  = subclass,
        .data      = data,
        .timestamp = timestamp
    };
    xQueueSend(
        map->queue_out, // xQueue
        &event,         // pvItemToQueue
        portMAX_DELAY   // xTIcksToWait
    );
}

static inline uint8_t smk_keymap_get_layout_id(const smk_keyboard_hardware_type * const hardware, uint8_t matrix_id)
{
    if (matrix_id >= hardware->matrix.total_cnt) {
        return 0xFFU;
    }
    uint8_t layout_id = hardware->matrix.layout_from[matrix_id];
    if (layout_id >= hardware->layout.total_cnt) {
        return 0xFFU;
    }
    return layout_id;
}

static inline void smk_keymap_convert_from_keypos_press(smk_keyboard_map_type *map, const smk_event_type *event_keypos, smk_event_type *event_keycode)
{
    const smk_keyboard_hardware_type *hardware = map->hardware;
    uint8_t matrix_id = (uint8_t)event_keypos->data;
    uint8_t layout_id = smk_keymap_get_layout_id(hardware, matrix_id);

    if (layout_id >= hardware->layout.total_cnt) {
        goto keypos_press_no_event;
    }
    if (map->layer_cache[layout_id] < hardware->map.layer_cnt) {
        // Key at the same position is pressed and not released yet, so simply do nothing
        goto keypos_press_no_event;
    }

    uint8_t layer_effective = 0;
    smk_keycode_type keycode = KC_NO;

    const smk_keycode_type *cur_keymap = hardware->map.keymaps;
    for (uint8_t layer = 0U; layer < hardware->map.layer_cnt; ++layer) {
        if (map->layer_active[layer]) {
            if (cur_keymap[layout_id] != KC_TRANS) {
                layer_effective = layer;
                keycode = cur_keymap[layout_id];
            }
        }
        cur_keymap += hardware->layout.total_cnt;
    }

    if (keycode == KC_NO) {
        goto keypos_press_no_event;
    }

    map->layer_cache[layout_id] = layer_effective;
    *event_keycode = (smk_event_type){
        .class     = SMK_EVENT_KEYCODE,
        .subclass  = SMK_EVENT_KEYCODE_ADD,
        .data      = (smk_event_data_type)keycode,
        .timestamp = event_keypos->timestamp
    };

    MAP_DEBUG("[SMK][KeyMap] KeyPos %u is pressed \r\n", matrix_id);
    MAP_DEBUG("    and mapped to KeyCode %u of Layer %u @%u\r\n", keycode, layer_effective, event_keycode->timestamp);

    return;

keypos_press_no_event:
    *event_keycode = (smk_event_type){
        .class     = SMK_EVENT_NONE,
        .subclass  = 0U,
        .data      = (smk_event_data_type)0U,
        .timestamp = event_keypos->timestamp
    };
}

static inline void smk_keymap_convert_from_keypos_release(smk_keyboard_map_type *map, const smk_event_type *event_keypos, smk_event_type *event_keycode)
{
    const smk_keyboard_hardware_type *hardware = map->hardware;
    uint8_t matrix_id = (uint8_t)event_keypos->data;
    uint8_t layout_id = smk_keymap_get_layout_id(hardware, matrix_id);

    if (layout_id >= hardware->layout.total_cnt) {
        goto keypos_release_no_event;
    }
    if (map->layer_cache[layout_id] >= hardware->map.layer_cnt) {
        // Key at the same position is not pressed yet, so simply do nothing
        goto keypos_release_no_event;
    }

    uint8_t layer_pressed = map->layer_cache[layout_id];
    const smk_keycode_type *cur_keymap = hardware->map.keymaps + layer_pressed * hardware->layout.total_cnt;
    smk_keycode_type keycode = cur_keymap[layout_id];

    map->layer_cache[layout_id] = 0xFF;
    *event_keycode = (smk_event_type){
        .class     = SMK_EVENT_KEYCODE,
        .subclass  = SMK_EVENT_KEYCODE_REMOVE,
        .data      = (smk_event_data_type)keycode,
        .timestamp = event_keypos->timestamp
    };

    MAP_DEBUG("[SMK][KeyMap] KeyPos %u is released \r\n", matrix_id);
    MAP_DEBUG("    which was mapped to KeyCode %u of Layer %u @%u\r\n", keycode, layer_pressed, event_keycode->timestamp);

    return;

keypos_release_no_event:
    *event_keycode = (smk_event_type){
        .class = SMK_EVENT_NONE,
        .subclass = 0U,
        .data = (smk_event_data_type)0U,
        .timestamp = event_keypos->timestamp
    };
}

smk_keyboard_map_type * smk_keymap_init(const smk_keyboard_hardware_type * const hardware, QueueHandle_t queue_in, QueueHandle_t queue_out)
{
    smk_keyboard_map_type *map = pvPortMalloc(sizeof(smk_keyboard_map_type));
    if (map == NULL) {
        goto keymap_malloc_fail;
    }

    map->hardware = hardware;

    map->layer_active  = NULL;
    map->layer_cache   = NULL;
    map->default_layer = hardware->map.default_layer < hardware->map.layer_cnt ? hardware->map.default_layer : 0U;

    map->commit_required = 0U;

    map->queue_in  = queue_in;
    map->queue_out = queue_out;

    if ((map->layer_active = pvPortMalloc(hardware->map.layer_cnt * sizeof(uint8_t))) == NULL) {
        goto keymap_init_fail;
    }

    if ((map->layer_cache = pvPortMalloc(hardware->layout.total_cnt * sizeof(uint8_t))) == NULL) {
        goto keymap_init_fail;
    }

    for (uint8_t layer = 0; layer < hardware->map.layer_cnt; ++layer) {
        map->layer_active[layer] = 0U;
    }
    map->layer_active[map->default_layer] = 1U;
    for (uint8_t layout_id = 0; layout_id < hardware->layout.total_cnt; ++layout_id) {
        map->layer_cache[layout_id] = 0xFFU;
    }

    MAP_DEBUG("[SMK][KeyMap] init is successful!\r\n");
    MAP_DEBUG("    Layer Count: %u\r\n", hardware->map.layer_cnt);

    return map;

keymap_init_fail:
    vPortFree(map->layer_cache);
    vPortFree(map->layer_active);
    vPortFree(map);
keymap_malloc_fail:

    MAP_DEBUG("[SMK][KeyMap] init is failed!\r\n");

    return NULL;
}

void smk_keymap_convert_to_keycode(smk_keyboard_map_type *map, const smk_event_type *event_keypos, smk_event_type *event_keycode)
{
    switch (event_keypos->subclass) {
    case SMK_EVENT_KEYPOS_PRESS:
        smk_keymap_convert_from_keypos_press(map, event_keypos, event_keycode);
        break;

    case SMK_EVENT_KEYPOS_RELEASE:
        smk_keymap_convert_from_keypos_release(map, event_keypos, event_keycode);
        break;

    default:
        *event_keycode = (smk_event_type){
            .class = SMK_EVENT_NONE,
            .subclass = 0U,
            .data = (smk_event_data_type)0U,
            .timestamp = event_keypos->timestamp
        };
    }
}

void smk_keymap_handle_keycode(smk_keyboard_map_type *map, smk_event_type *event_keycode)
{
    uint8_t layer_id;

    switch (LAYER_TYPE_MASK(event_keycode->data)) {
    case BASIC_KEYCODE:
        // Send keycode to queue_out directly
        xQueueSend(
            map->queue_out, // xQueue
            event_keycode,  // pvItemToQueue
            portMAX_DELAY   // xTicksToWait
        );
        map->commit_required = 1U;
        MAP_DEBUG("[SMK][KeyMap] basic keycode %u\r\n", event_keycode->data);
        break;

    case DEFAULT_LAYER:
        if (event_keycode->subclass == SMK_EVENT_KEYPOS_PRESS) {
            layer_id = DF_LAYER(event_keycode->data);
            if (layer_id >= map->hardware->map.layer_cnt) {
                break;
            }
            map->layer_active[map->default_layer] = 0U;
            map->default_layer = layer_id;
            map->layer_active[map->default_layer] = 1U;
            MAP_DEBUG("[SMK][KeyMap] switch to default layer %u\r\n", layer_id);
        }
        break;

    case MOMENTARILY_ACTIVES_LAYER:
        layer_id = MO_LAYER(event_keycode->data);
        if (layer_id >= map->hardware->map.layer_cnt) {
            break;
        }
        switch (event_keycode->subclass) {
        case SMK_EVENT_KEYPOS_PRESS:
            MAP_DEBUG("[SMK][KeyMap] layer %u activated\r\n", layer_id);
            map->layer_active[layer_id] = 1U;
            break;

        case SMK_EVENT_KEYPOS_RELEASE:
            MAP_DEBUG("[SMK][KeyMap] layer %u deactivated\r\n", layer_id);
            map->layer_active[layer_id] = 0U;
            break;
        }

        break;
    }
}

void smk_keymap_task(void *pvParameters)
{
    smk_keyboard_map_type *map = pvParameters;
    smk_event_type event;

    MAP_DEBUG("[SMK][KeyMap] task start...\r\n");

    for (;;) {
        xQueueReceive(
            map->queue_in, // xQueue
            &event,        // pvBuffer
            portMAX_DELAY  // xTicksToWait
        );

        if (event.class != SMK_EVENT_KEYPOS) {
            continue;
        }

        if (event.subclass == SMK_EVENT_KEYPOS_SCAN_END) {
            if (map->commit_required) {
                smk_keymap_send_event(map, SMK_EVENT_KEYCODE, SMK_EVENT_KEYCODE_COMMIT, 0U, event.timestamp);
                map->commit_required = 0U;
                continue;
            }
        }

        smk_keymap_convert_to_keycode(map, &event, &event);

        if (event.class != SMK_EVENT_KEYCODE) {
            continue;
        }

        smk_keymap_handle_keycode(map, &event);
    }
}
