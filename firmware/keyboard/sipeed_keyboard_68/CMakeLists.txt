set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_LIBS freertos ble usb_stack mbedtls shell)
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(EASYFLASH_DIR ${CMAKE_CURRENT_SOURCE_DIR}/easyflash)
list(APPEND ADD_INCLUDE ${EASYFLASH_DIR}/inc)
set(TARGET_REQUIRED_SRCS
                         ${CMAKE_CURRENT_LIST_DIR}/smk_ble_hog_service.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_ble.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_battery.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_usb.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_hid.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_cdc.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_msc.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_spirgb.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_event_manager.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_config_manager.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_endpoints.c
                         ${CMAKE_CURRENT_LIST_DIR}/rgbeffect/smk_rgb_color.c
                         ${CMAKE_CURRENT_LIST_DIR}/rgbeffect/smk_rgb_effect.c
                         ${CMAKE_CURRENT_LIST_DIR}/rgbeffect/smk_rgb_layout.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_hid_protocol.c
                         ${CMAKE_CURRENT_LIST_DIR}/keyboard/smk_keyscan.c
                         ${CMAKE_CURRENT_LIST_DIR}/keyboard/smk_keymap.c
                         ${CMAKE_CURRENT_LIST_DIR}/keyboard/smk_tapengine.c
                         ${CMAKE_CURRENT_LIST_DIR}/keyboard/smk_debounce_defer_g.c
                         ${CMAKE_CURRENT_LIST_DIR}/keyboard/smk_debounce_eager_defer_pk.c
                         ${CMAKE_CURRENT_LIST_DIR}/events/system_init_event.c
                         ${CMAKE_CURRENT_LIST_DIR}/events/rgb_switch_mode_event.c
                         ${CMAKE_CURRENT_LIST_DIR}/events/battery_update_event.c
                         ${CMAKE_CURRENT_LIST_DIR}/events/ble_active_profile_changed.c
                         ${CMAKE_CURRENT_LIST_DIR}/bas.c
                         ${BSP_COMMON_DIR}/usb/usb_dc.c
                         ${BSP_COMMON_DIR}/usb/uart_interface.c
                         ${BSP_COMMON_DIR}/spi_sd/bsp_spi_sd.c
                         ${EASYFLASH_DIR}/src/easyflash.c
                         ${EASYFLASH_DIR}/src/ef_env.c
                         ${EASYFLASH_DIR}/src/ef_utils.c
                         ${EASYFLASH_DIR}/port/ef_port.c)

list(APPEND TARGET_REQUIRED_SRCS ${CMAKE_CURRENT_LIST_DIR}/config/smk_rgb_config.c)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/config/custom/custom_keycode.c)
    list(APPEND TARGET_REQUIRED_SRCS ${CMAKE_CURRENT_LIST_DIR}/config/custom/custom_keycode.c)
else()
    list(APPEND TARGET_REQUIRED_SRCS ${CMAKE_CURRENT_LIST_DIR}/config/sipeed_mechanical_keyboard.c)
endif()
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/usb ${BSP_COMMON_DIR}/spi_sd)
set(mains main.c)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/bl702_flash_ble.ld)
add_definitions(-DCONFIG_BT_DEVICE_NAME="SMK68_BLE")
set(CONFIG_BT_DIS_SERVER on)
add_definitions(-DSMK_CONFIG_ENABLE_BLE=1)
add_definitions(-DSMK_CONFIG_ENABLE_USB=1)
add_definitions(-DCONFIG_BT_GATT_DIS_PNP=1)
# add_definitions(-DCONFIG_BT_BAS_SERVER=1)
add_definitions(-DCONFIG_BLE_CLEAR_BOUNDS_ON_START=0)
add_definitions(-DCONFIG_BT_SETTINGS=1)
add_definitions(-DCONFIG_BAS_REPORT_TIME_MS=1000)

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("debug mode")
    add_definitions(-DEN_MAP_DEBUG=1)
    add_definitions(-DEN_HID_DEBUG=1)
    add_definitions(-DEN_USB_DEBUG=1)
    add_definitions(-DEN_SCAN_DEBUG=1)
    add_definitions(-DEN_EM_DEBUG=1)
    add_definitions(-DEN_BAT_DEBUG=1)
    add_definitions(-DEN_EP_DEBUG=1)
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

generate_bin()
