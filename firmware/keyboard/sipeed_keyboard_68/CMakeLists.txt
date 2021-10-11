set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_LIBS freertos ble usb_stack mbedtls)
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TARGET_REQUIRED_SRCS
                        #  ${CMAKE_CURRENT_LIST_DIR}/ble_peripheral_tp_server.c
                        #  ${CMAKE_CURRENT_LIST_DIR}/smk_ble.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_usb.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_hid.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_cdc.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_msc.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_shell.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_spirgb.c
                         ${CMAKE_CURRENT_LIST_DIR}/rgbeffect/smk_rgb_color.c
                         ${CMAKE_CURRENT_LIST_DIR}/rgbeffect/smk_rgb_effect.c
                         ${CMAKE_CURRENT_LIST_DIR}/rgbeffect/smk_rgb_layout.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_hid_protocol.c
                         ${CMAKE_CURRENT_LIST_DIR}/keyboard/smk_keyscan.c
                         ${CMAKE_CURRENT_LIST_DIR}/keyboard/smk_keymap.c
                         ${CMAKE_CURRENT_LIST_DIR}/keyboard/smk_tapengine.c
                         ${CMAKE_CURRENT_LIST_DIR}/keyboard/smk_debounce_defer_g.c
                         ${CMAKE_CURRENT_LIST_DIR}/keyboard/smk_debounce_eager_defer_pk.c
                         ${BSP_COMMON_DIR}/usb/usb_dc.c
                         ${BSP_COMMON_DIR}/usb/uart_interface.c
                         ${BSP_COMMON_DIR}/spi_sd/bsp_spi_sd.c)

list(APPEND TARGET_REQUIRED_SRCS ${CMAKE_CURRENT_LIST_DIR}/config/smk_rgb_config.c)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/config/custom/custom_keycode.c)
    list(APPEND TARGET_REQUIRED_SRCS ${CMAKE_CURRENT_LIST_DIR}/config/custom/custom_keycode.c)
else()
    list(APPEND TARGET_REQUIRED_SRCS ${CMAKE_CURRENT_LIST_DIR}/config/sipeed_mechanical_keyboard.c)
endif()
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/usb ${BSP_COMMON_DIR}/spi_sd)
set(mains main.c)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/bl702_flash_ble.ld)
generate_bin()

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("debug mode")
    add_definitions(-DEN_MAP_DEBUG=1)
    add_definitions(-DEN_HID_DEBUG=1)
    add_definitions(-DEN_USB_DEBUG=1)
    add_definitions(-DEN_SCAN_DEBUG=1)
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

