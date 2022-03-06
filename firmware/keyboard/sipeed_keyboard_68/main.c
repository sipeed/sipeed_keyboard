/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "hal_uart.h"
#include <FreeRTOS.h>
#include <zephyr.h>
#include "semphr.h"
#include "bl702.h"
#include "bl702_sec_eng.h"
#include "smk_ble.h"
#include "smk_usb.h"
#include "shell.h"
#include "smk_command.h"
#include "smk_spirgb.h"
#include "smk_spirgb_command.h"
#include "smk_battery.h"
#include "smk_oled.h"
#include "smk_endpoints.h"

#include "keyboard/smk_keyscan.h"
#include "keyboard/smk_keymap.h"

#include "smk_event_manager.h"
#include "smk_config_manager.h"
#include "events/system_init_event.h"

#include "easyflash.h"

extern uint8_t _heap_start;
extern uint8_t _heap_size; // @suppress("Type cannot be resolved")
extern uint8_t _heap2_start;
extern uint8_t _heap2_size;
static HeapRegion_t xHeapRegions[] = {
    { &_heap2_start, (unsigned int)&_heap2_size}, 
    { &_heap_start, (unsigned int)&_heap_size },
    { NULL, 0 }  /* Terminates the array. */
};

uint8_t sharedBuf[16];
void user_vAssertCalled(void) __attribute__((weak, alias("vAssertCalled")));
void vAssertCalled(void)
{
    MSG("vAssertCalled\r\n");

    while (1)
        ;
}

void vApplicationTickHook(void)
{
    //MSG("vApplicationTickHook\r\n");
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    MSG("vApplicationStackOverflowHook\r\n");

    if (pcTaskName) {
        MSG("Stack name %s\r\n", pcTaskName);
    }

    while (1)
        ;
}

void vApplicationMallocFailedHook(void)
{
    MSG("vApplicationMallocFailedHook\r\n");

    while (1)
        ;
}
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

static void raise_event()
{
    MSG("[SMK main] raise_event \n");

    // static sturct system_init_event new_system_init_event = {.data = 0xaa};

    SMK_EVENT_RAISE(new_system_init_event((struct system_init_event)
    {
        .data = 0xa0
    }
    ));
}

static void ble_init_task(void *pvParameters)
{
    smk_ble_init_task();
    vTaskDelete(NULL);
}

static void usb_init_task(void *pvParameters) //FIXME
{
    // vTaskDelay(1000);
    usb_init();
    vTaskDelete(NULL);
}



static void raise_event_callback(struct k_work *work){
    raise_event();
}
K_WORK_DEFINE(raise_event_work, raise_event_callback);
int init_event_listener(const smk_event_t *eh){
    const struct system_init_event *ev = as_system_init_event(eh);
    if (ev)
    {
        MSG("[EVENT]: system init. 0x%x\n", ev->data);
    }
    return 0;
}
SMK_LISTENER(init_event_listener, init_event_listener);
SMK_SUBSCRIPTION(init_event_listener, system_init_event);

static void test_env(void) {
    uint32_t i_boot_times = NULL;
    char *c_old_boot_times, c_new_boot_times[11] = {0};
    /* get the boot count number from Env */
    c_old_boot_times = ef_get_env("boot_times");
    if (!c_old_boot_times) {
        i_boot_times = 0;
    } else {
        i_boot_times = atoi(c_old_boot_times);
    }
    /* boot count +1 */
    i_boot_times ++;
    MSG("The system now boot %d times\n\r", i_boot_times);
    /* interger to string */
    sprintf(c_new_boot_times,"%ld", i_boot_times);
    /* set and store the boot count number to Env */
    ef_set_env("boot_times", c_new_boot_times);
    ef_save_env();
}


int main(void)
{
    static StackType_t endpoints_switch_stack[1024];
    static StaticTask_t endpoints_switch_task_h;
    uint32_t tmpVal = 0;

    bflb_platform_init(0);
    shell_init();
    shell_set_prompt(SHELL_NAME);
    shell_set_print(acm_printf);
    
    MSG("Sipeed Machine Keyboard start...\r\n");
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
    
    flash_init();
    MSG("[SMK] flash init done\r\n");
    smk_config_init();
    MSG("[SMK] smk_config_init done\r\n");
    check_if_ef_ready();
    test_env();


    //Set capcode
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_IN_AON, 33);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_OUT_AON, 33);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    Sec_Eng_Trng_Enable();

    vPortDefineHeapRegions(xHeapRegions);

    MSG("[SMK] Device init...\r\n");
    xTaskCreateStatic(
        smk_endpoint_switch_task, 
        (char *)"endpoint_switch_task", 
        sizeof(endpoints_switch_stack) / 4,
        NULL, 
        10, 
        endpoints_switch_stack, 
        &endpoints_switch_task_h);


    xTaskCreate(
        rgb_loop_task, 
        (char *)"rgb_loop", 
        256, 
        NULL, 
        10, 
        NULL);
    MSG("[SMK] Free Heap:%d\r\n", (int)xPortGetFreeHeapSize());

    const smk_keyboard_hardware_type *hardware = smk_keyboard_get_hardware();
    QueueHandle_t queue_keypos = xQueueCreate(
        128,                   // uxQueueLength
        sizeof(smk_event_type) // uxItemSize
    );
    QueueHandle_t queue_keycode = xQueueCreate(
        128,                   // uxQueueLength
        sizeof(smk_event_type) // uxItemSize
    );

    smk_keyboard_scan_type *scan = smk_keyscan_init (hardware, queue_keypos);
    smk_keyboard_map_type  *map  = smk_keymap_init  (hardware, queue_keypos, queue_keycode);

    xTaskCreate(
        smk_keyscan_task, // pxTaskCode
        "KeyScan Task",   // pcName
        512,              // usStackDepth
        scan,             // pvParameters
        10,               // uxPriority
        NULL              // pxCreatedTask
    );
    xTaskCreate(
        smk_keymap_task, // pxTaskCode
        "KeyMap Task",   // pcName
        768,             // usStackDepth
        map,             // pvParameters
        10,   // uxPriority
        NULL             // pxCreatedTask
    );
    xTaskCreate(
        smk_usb_hid_daemon_task, // pxTaskCode
        "USB HID Task",          // pcName
        256,                     // usStackDepth
        queue_keycode,           // pvParameters
        10,    // uxPriority
        NULL                     // pxCreateTask
    );

    xTaskCreate(
        smk_battery_update_task, // pxTaskCode
        "Battery Update Task",   // pcName
        384,                     // usStackDepth
        NULL,                    // pvParameters
        10,    // uxPriority
        NULL                     // pxCreateTask
    );
    xTaskCreate(
        smk_oled_demon_task, // pxTaskCode
        "OLED Demon Task",   // pcName
        1024,                     // usStackDepth
        NULL,                    // pvParameters
        10,    // uxPriority
        NULL                     // pxCreateTask
    );
    MSG("[SMK] Free Heap:%d\r\n", (int)xPortGetFreeHeapSize());
    MSG("[SMK] Start task scheduler...\r\n");
    vTaskStartScheduler();
    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}