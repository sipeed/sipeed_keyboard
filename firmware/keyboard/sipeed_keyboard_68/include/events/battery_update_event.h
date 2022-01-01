#pragma once

#include "smk_event_manager.h"

struct battery_update_event
{
    uint8_t level;
};


SMK_EVENT_DECLARE(battery_update_event);