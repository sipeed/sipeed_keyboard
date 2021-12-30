#include "events/system_init_event.h"

SMK_EVENT_IMPL(system_init_event);
// const struct smk_event_type smk_event_system_init_event = {.name = STRINGIFY(system_init_event)};
// const struct smk_event_type *smk_event_ref_system_init_event __used __attribute__((__section__(".event_type"))) = &smk_event_system_init_event;
// struct system_init_event_event *new_system_init_event(struct system_init_event data)
// {
//     struct system_init_event_event *ev = (struct system_init_event_event *)k_malloc(sizeof(struct system_init_event_event));
//     ev->header.event = &smk_event_system_init_event;
//     ev->data = data;
//     return ev;
// };
// struct system_init_event *as_system_init_event(const smk_event_t *eh) { return (eh->event == &smk_event_system_init_event) ? &((struct system_init_event_event *)eh)->data : NULL; };