/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stddef.h>
// #include <kernel.h>
#include <zephyr/types.h>
#include "bl_port.h"
#include "gcc.h"

#ifndef EN_EM_DEBUG
#define EN_EM_DEBUG 0
#endif

#if EN_EM_DEBUG
#include "bflb_platform.h"
#define EM_DEBUG(fmt, ...) MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define EM_DEBUG(fmt, ...)
#endif


struct smk_event_type {
    const char *name;
};

typedef struct {
    const struct smk_event_type *event;
    uint8_t last_listener_index;
} smk_event_t;

#define SMK_EV_EVENT_BUBBLE 0
#define SMK_EV_EVENT_HANDLED 1
#define SMK_EV_EVENT_CAPTURED 2

typedef int (*smk_listener_callback_t)(const smk_event_t *eh);
struct smk_listener {
    smk_listener_callback_t callback;
};

struct smk_event_subscription {
    const struct smk_event_type *event_type;
    const struct smk_listener *listener;
};

#define SMK_EVENT_DECLARE(event_type)                                                              \
    struct event_type##_event {                                                                    \
        smk_event_t header;                                                                        \
        struct event_type data;                                                                    \
    };                                                                                             \
    struct event_type##_event *new_##event_type(struct event_type);                                \
    struct event_type *as_##event_type(const smk_event_t *eh);                                     \
    extern const struct smk_event_type smk_event_##event_type;

#define SMK_EVENT_IMPL(event_type)                                                                 \
    const struct smk_event_type smk_event_##event_type = {.name = STRINGIFY(event_type)};          \
    const struct smk_event_type *smk_event_ref_##event_type __used                                 \
        __attribute__((__section__(".event_type"))) = &smk_event_##event_type;                     \
    struct event_type##_event *new_##event_type(struct event_type data) {                          \
        struct event_type##_event *ev =                                                            \
            (struct event_type##_event *)k_malloc(sizeof(struct event_type##_event));              \
        ev->header.event = &smk_event_##event_type;                                                \
        ev->data = data;                                                                           \
        return ev;                                                                                 \
    };                                                                                             \
    struct event_type *as_##event_type(const smk_event_t *eh) {                                    \
        return (eh->event == &smk_event_##event_type) ? &((struct event_type##_event *)eh)->data   \
                                                      : NULL;                                      \
    };

#define SMK_LISTENER(mod, cb) const struct smk_listener smk_listener_##mod = {.callback = cb};

#define SMK_SUBSCRIPTION(mod, ev_type)                                                             \
    const Z_DECL_ALIGN(struct smk_event_subscription)                                              \
        _CONCAT(_CONCAT(smk_event_sub_, mod), ev_type) __used                                      \
        __attribute__((__section__(".event_subscription"))) = {                                    \
            .event_type = &smk_event_##ev_type,                                                    \
            .listener = &smk_listener_##mod,                                                       \
    };

#define SMK_EVENT_RAISE(ev) smk_event_manager_raise((smk_event_t *)ev);

#define SMK_EVENT_RAISE_AFTER(ev, mod)                                                             \
    smk_event_manager_raise_after((smk_event_t *)ev, &smk_listener_##mod);

#define SMK_EVENT_RAISE_AT(ev, mod)                                                                \
    smk_event_manager_raise_at((smk_event_t *)ev, &smk_listener_##mod);

#define SMK_EVENT_RELEASE(ev) smk_event_manager_release((smk_event_t *)ev);

#define SMK_EVENT_FREE(ev) k_free((void *)ev);

int smk_event_manager_raise(smk_event_t *event);
int smk_event_manager_raise_after(smk_event_t *event, const struct smk_listener *listener);
int smk_event_manager_raise_at(smk_event_t *event, const struct smk_listener *listener);
int smk_event_manager_release(smk_event_t *event);