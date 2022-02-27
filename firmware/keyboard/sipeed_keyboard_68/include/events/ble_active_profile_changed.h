#pragma once

#include "smk_event_manager.h"
#include "smk_ble_profile.h"

struct smk_ble_active_profile_changed {
    uint8_t index;
    struct smk_ble_profile *profile;
};

SMK_EVENT_DECLARE(smk_ble_active_profile_changed);