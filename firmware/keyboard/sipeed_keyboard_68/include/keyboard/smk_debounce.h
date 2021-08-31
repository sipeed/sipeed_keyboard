#ifndef __SMK_DEBOUNCE_H
#define __SMK_DEBOUNCE_H

#include "smk_keyboard.h"

typedef enum {
    SMK_KEYBOARD_DEBOUNCE_ALGO_DEFER_G,
    SMK_KEYBOARD_DEBOUNCE_ALGO_EAGER_DEFER_PK,
    SMK_KEYBOARD_DEBOUNCE_ALGO_COUNT
} smk_keyboard_debounce_algo_type;

int smk_keyscan_init_debounce_defer_g(smk_keyboard_scan_type *scan);
int smk_keyscan_init_debounce_eager_defer_pk(smk_keyboard_scan_type *scan);

typedef int (*smk_keyscan_init_debounce_type)(smk_keyboard_scan_type *);

static const smk_keyscan_init_debounce_type smk_keyscan_init_debounce_func[SMK_KEYBOARD_DEBOUNCE_ALGO_COUNT] = {
    [SMK_KEYBOARD_DEBOUNCE_ALGO_DEFER_G]        = smk_keyscan_init_debounce_defer_g,
    [SMK_KEYBOARD_DEBOUNCE_ALGO_EAGER_DEFER_PK] = smk_keyscan_init_debounce_eager_defer_pk
};

#endif
