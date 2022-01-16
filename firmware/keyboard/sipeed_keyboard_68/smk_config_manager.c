#include "smk_config_manager.h"

static smk_config_header_t* smk_config_header;
smk_system_config_t* smk_system_config;
smk_keyboard_config_t* smk_keyboard_config;
smk_ble_config_t* smk_ble_config;
smk_rgb_config_t* smk_rgb_config;
smk_usb_config_t* smk_usb_config;
smk_oled_config_t* smk_oled_config;

smk_config_t smk_global_config;

static void config_head_init()
{
    smk_config_header_t header = {
        .magic = SMK_CONFIG_MAGIC_NUMBER,
        .version = SMK_CONFIG_VERSION_NUMBER,
    };
    smk_global_config.header = header;
}

static void system_config_init()
{
    smk_system_config_t system_config = {
        .endpoint = SMK_ENDPOINT_USB,
    };
    smk_global_config.system = system_config;
}

static void rgb_config_init()
{
    smk_rgb_config_t rgb_config = {
        .rgb_mode = 0,
    };
    smk_global_config.rgb = rgb_config;
}

int smk_config_init()
{
    int ret = 0;
#if EN_EARSE_FLASH
    ret = flash_erase(SMK_CONFIG_FLASH_ADDR_START, SMK_CONFIG_FLASH_ADDR_LEN);
#endif // EN_EARSE_FLASH
    // read config
    ret = flash_read(SMK_CONFIG_FLASH_ADDR_START, (uint8_t*)&smk_global_config, sizeof(smk_config_t));
    // check magic number
    CM_DEBUG("[CM]smk_config_init: magic: 0x%x, version: 0x%x\n", smk_global_config.header.magic, smk_global_config.header.version);
    if (smk_global_config.header.magic != SMK_CONFIG_MAGIC_NUMBER) {
        CM_DEBUG("[CM]config magic number error\r\n");
        config_head_init();
        system_config_init();
        rgb_config_init();
        ret = flash_write(SMK_CONFIG_FLASH_ADDR_START, (uint8_t*)&smk_global_config, sizeof(smk_config_t));
        CM_DEBUG("[CM]config init ret: %d\r\n", ret);
    }
    // check version number
    if (smk_global_config.header.version != SMK_CONFIG_VERSION_NUMBER) {
        CM_DEBUG("[CM]config version number error\r\n");
        config_head_init();
        system_config_init();
        rgb_config_init();
        ret = flash_write(SMK_CONFIG_FLASH_ADDR_START, (uint8_t*)&smk_global_config, sizeof(smk_config_t));
        CM_DEBUG("[CM]config init ret: %d\r\n", ret);
    }
    smk_config_header = &smk_global_config.header;
    smk_system_config = &smk_global_config.system;
    smk_keyboard_config = &smk_global_config.keyboard;
    smk_ble_config = &smk_global_config.ble;
    smk_rgb_config = &smk_global_config.rgb;
    smk_usb_config = &smk_global_config.usb;
    smk_oled_config = &smk_global_config.oled;
    CM_DEBUG("[CM]RGB MODE: %d\r\n", smk_rgb_config->rgb_mode);
    return ret;
}

void smk_config_save()
{
    int ret = 0;
    ret = flash_erase(SMK_CONFIG_FLASH_ADDR_START, SMK_CONFIG_FLASH_ADDR_LEN);
    ret = flash_write(SMK_CONFIG_FLASH_ADDR_START, (uint8_t*)&smk_global_config, sizeof(smk_config_t));
    smk_config_load();
}

void smk_config_load()
{
    flash_read(SMK_CONFIG_FLASH_ADDR_START, (uint8_t*)&smk_global_config, sizeof(smk_config_t));
}   

void smk_config_erase()
{
    flash_erase(SMK_CONFIG_FLASH_ADDR_START, SMK_CONFIG_FLASH_ADDR_LEN);
}