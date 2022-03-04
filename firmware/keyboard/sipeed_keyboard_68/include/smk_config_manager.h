#ifndef __SMK_CONFG_MANAGER_H
#define __SMK_CONFG_MANAGER_H

#include "hal_flash.h"
#include "smk_endpoints.h"

#ifndef EN_CM_DEBUG
#define EN_CM_DEBUG 0
#endif

#if EN_CM_DEBUG
#include "bflb_platform.h"
#define CM_DEBUG(fmt, ...) MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define CM_DEBUG(fmt, ...)
#endif //EN_CM_DEBUG

#ifndef EN_EARSE_FLASH
#define EN_EARSE_FLASH 0
#endif

#define SMK_CONFIG_FLASH_ADDR_START 0x00080000 //flash start from 0x00080000 512K
#define SMK_CONFIG_FLASH_ADDR_LEN 0x4000 //16K
#define SMK_CONFIG_FLASH_ADDR_END (SMK_CONFIG_FLASH_ADDR_START + SMK_CONFIG_FLASH_ADDR_LEN)

#define SMK_CONFIG_MAGIC_NUMBER 0x534D4B00  // "SMK"
// #define SMK_CONFIG_VERSION_NUMBER 0x00010000 // 1.0
#define SMK_CONFIG_VERSION_NUMBER 0x00000001 // V0.1

// config header
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t reserved[6];
} smk_config_header_t;

typedef enum{
    SMK_CONFIG_TYPE_NONE = 0,
    SMK_CONFIG_TYPE_SYSTEM,
    SMK_CONFIG_TYPE_KEYBOARD,
    SMK_CONFIG_TYPE_BLE,
    SMK_CONFIG_TYPE_RGB,
    SMK_CONFIG_TYPE_USB,
    SMK_CONFIG_TYPE_OLED,
    SMK_CONFIG_TYPE_MAX
} smk_config_type_t;

// typedef enum{
//     SMK_ENDPOINT_USB = 0,
//     SMK_ENDPOINT_BLE,
//     SMK_ENDPOINT_USB_BLE,
// } smk_endpoint_type_t;

// system config
typedef struct{
    smk_endpoint_type_t endpoint;
    uint32_t reserved[31];
} smk_system_config_t;

// keyboard config
typedef struct{
    uint32_t reserved[32];
} smk_keyboard_config_t;

// ble config
typedef struct{
    uint32_t reserved[32];
} smk_ble_config_t;

// rgb config
typedef struct{
    uint32_t rgb_mode;
    uint32_t reserved[31];
} smk_rgb_config_t;

// usb config
typedef struct{
    uint32_t reserved[32];
} smk_usb_config_t;

// oled config
typedef struct{
    uint32_t reserved[32];
} smk_oled_config_t;

// config
typedef struct{
    smk_config_header_t header;
    smk_system_config_t system;
    smk_keyboard_config_t keyboard;
    smk_ble_config_t ble;
    smk_rgb_config_t rgb;
    smk_usb_config_t usb;
    smk_oled_config_t oled;
} smk_config_t;

int smk_config_init();
void smk_config_save();
void smk_config_load();
void smk_config_erase();

extern smk_system_config_t* smk_system_config;
extern smk_keyboard_config_t* smk_keyboard_config;
extern smk_ble_config_t* smk_ble_config;
extern smk_rgb_config_t* smk_rgb_config;
extern smk_usb_config_t* smk_usb_config;
extern smk_oled_config_t* smk_oled_config;

#endif //__SMK_CONFG_MANAGER_H