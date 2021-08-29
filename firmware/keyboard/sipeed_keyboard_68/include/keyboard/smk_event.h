#ifndef __SMK_EVENT_H
#define __SMK_EVENT_H

#include <stdint.h>
#include "FreeRTOS.h"

typedef enum {
    SMK_EVENT_NONE,
    SMK_EVENT_KEYPOS,
    SMK_EVENT_KEYCODE,
    SMK_EVENT_TAPENGINE,
    SMK_EVENT_COUNT
} smk_event_class_type;

typedef enum {
    SMK_EVENT_KEYPOS_SCAN_END,
    SMK_EVENT_KEYPOS_PRESS,
    SMK_EVENT_KEYPOS_RELEASE,
    SMK_EVENT_KEYPOS_COUNT
} smk_event_keypress_subclass_type;

typedef enum {
    SMK_EVENT_KEYCODE_COMMIT,
    SMK_EVENT_KEYCODE_ADD,
    SMK_EVENT_KEYCODE_REMOVE,
    SMK_EVENT_KEYCODE_COUNT
} smk_event_keycode_subclass_type;

typedef enum {
    SMK_EVENT_TAPENGINE_NONE,
    SMK_EVENT_TAPENGINE_TAPPING_BEGIN,
    SMK_EVENT_TAPENGINE_TAPPING_END,
    SMK_EVENT_TAPENGINE_HOLDING_BEGIN,
    SMK_EVENT_TAPENGINE_HOLDING_END,
    SMK_EVENT_TAPENGINE_COUNT
} smk_event_tapengine_subclass_type;

typedef uint16_t smk_event_data_type;

typedef struct {
    uint8_t             class;
    uint8_t             subclass;
    smk_event_data_type data;
    TickType_t          timestamp;
} smk_event_type;

#endif
