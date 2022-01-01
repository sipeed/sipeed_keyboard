#ifndef __SMK_BATTERY_H
#define __SMK_BATTERY_H

#ifndef EN_BAT_DEBUG
#define EN_BAT_DEBUG 0
#endif

#if EN_BAT_DEBUG
#include "bflb_platform.h"
#define BAT_DEBUG(fmt, ...) MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define BAT_DEBUG(fmt, ...)
#endif

#define BATTERY_ADC_CHANNEL ADC_CHANNEL2

int smk_battery_init(void);
int smk_battery_get_voltage(void);
void smk_battery_update_task(void *pvParameters);


#endif //__SMK_BATTERY_H