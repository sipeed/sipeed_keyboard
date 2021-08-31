#ifndef __SMK_KEYSCAN_H
#define __SMK_KEYSCAN_H

#include "smk_keyboard.h"
#include "smk_event.h"
#include "smk_debounce.h"

#include "queue.h"

#ifndef EN_SCAN_DEBUG
#define EN_SCAN_DEBUG 0
#endif

#if EN_SCAN_DEBUG
#include "bflb_platform.h"
#define SCAN_DEBUG(fmt, ...) MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define SCAN_DEBUG(fmt, ...)
#endif

smk_keyboard_scan_type * smk_keyscan_init(const smk_keyboard_hardware_type * const hardware, QueueHandle_t queue_out);

void smk_keyscan_read_raw(smk_keyboard_scan_type *scan);
void smk_keyscan_debounce(smk_keyboard_scan_type *scan);
void smk_keyscan_commit(smk_keyboard_scan_type *scan);

void smk_keyscan_task(void *pvParameters);

#endif
