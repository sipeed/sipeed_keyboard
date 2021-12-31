#pragma once

#include "smk_event_manager.h"

struct rgb_switch_mode_event
{
    int8_t direction;
};


SMK_EVENT_DECLARE(rgb_switch_mode_event);