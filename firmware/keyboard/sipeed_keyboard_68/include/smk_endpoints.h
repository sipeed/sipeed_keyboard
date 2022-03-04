#ifndef __SMK_ENDPOINTS_H
#define __SMK_ENDPOINTS_H

#ifndef EN_EP_DEBUG
#define EN_EP_DEBUG 0
#endif

#if EN_EP_DEBUG
#include "bflb_platform.h"
#define EP_DEBUG(fmt, ...) MSG_DBG("[EP] ");MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define EP_DEBUG(fmt, ...)
#endif

#include "FreeRTOS.h"
#include "queue.h"

typedef enum {
    SMK_ENDPOINT_USB,
    SMK_ENDPOINT_BLE,
} smk_endpoint_type_t;

extern QueueHandle_t endpoint_switch_queue;

int check_if_ef_ready();
int smk_endpoint_init();
void smk_endpoint_switch_task(void *pvParameters);

#endif //  __SMK_ENDPOINTS_H