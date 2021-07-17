set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_LIBS freertos ble usb_stack)
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TARGET_REQUIRED_SRCS ${CMAKE_CURRENT_LIST_DIR}/ble_peripheral_tp_server.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_ble.c
                         ${CMAKE_CURRENT_LIST_DIR}/smk_usb.c
                         ${BSP_COMMON_DIR}/usb/usb_dc.c 
                         ${BSP_COMMON_DIR}/usb/uart_interface.c 
                         ${BSP_COMMON_DIR}/spi_sd/bsp_spi_sd.c)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/usb ${BSP_COMMON_DIR}/spi_sd)
set(mains main.c)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/examples/ble/bl702_flash_ble.ld)
generate_bin()


