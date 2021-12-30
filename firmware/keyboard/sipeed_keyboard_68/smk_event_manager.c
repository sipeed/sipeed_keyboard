
#include <zephyr.h>
#include "errno.h"
#include "bflb_platform.h"
#include <smk_event_manager.h>

extern struct smk_event_type *__event_type_start[];
extern struct smk_event_type *__event_type_end[];

extern struct smk_event_subscription __event_subscriptions_start[];
extern struct smk_event_subscription __event_subscriptions_end[];

int smk_event_manager_handle_from(smk_event_t *event, uint8_t start_index) {
    int ret = 0;
    uint8_t len = __event_subscriptions_end - __event_subscriptions_start;
    for (int i = start_index; i < len; i++) {
        struct smk_event_subscription *ev_sub = __event_subscriptions_start + i;
        if (ev_sub->event_type != event->event) {
            continue;
        }
        ret = ev_sub->listener->callback(event);
        switch (ret) {
        case SMK_EV_EVENT_BUBBLE:
            continue;
        case SMK_EV_EVENT_HANDLED:
            MSG_DBG("Listener handled the event\n");
            ret = 0;
            goto release;
        case SMK_EV_EVENT_CAPTURED:
            MSG_DBG("Listener captured the event\n");
            event->last_listener_index = i;
            // Listeners are expected to free events they capture
            return 0;
        default:
            MSG_DBG("Listener returned an error: %d\n", ret);
            goto release;
        }
    }

release:
    k_free(event);
    return ret;
}

int smk_event_manager_raise(smk_event_t *event) { return smk_event_manager_handle_from(event, 0); }

int smk_event_manager_raise_after(smk_event_t *event, const struct smk_listener *listener) {
    uint8_t len = __event_subscriptions_end - __event_subscriptions_start;
    for (int i = 0; i < len; i++) {
        struct smk_event_subscription *ev_sub = __event_subscriptions_start + i;

        if (ev_sub->event_type == event->event && ev_sub->listener == listener) {
            return smk_event_manager_handle_from(event, i + 1);
        }
    }

    MSG_ERR("Unable to find where to raise this after event\n");

    return -EINVAL;
}

int smk_event_manager_raise_at(smk_event_t *event, const struct smk_listener *listener) {
    uint8_t len = __event_subscriptions_end - __event_subscriptions_start;
    for (int i = 0; i < len; i++) {
        struct smk_event_subscription *ev_sub = __event_subscriptions_start + i;

        if (ev_sub->event_type == event->event && ev_sub->listener == listener) {
            return smk_event_manager_handle_from(event, i);
        }
    }

    MSG_ERR("Unable to find where to raise this event\n");

    return -EINVAL;
}

int smk_event_manager_release(smk_event_t *event) {
    return smk_event_manager_handle_from(event, event->last_listener_index + 1);
}
