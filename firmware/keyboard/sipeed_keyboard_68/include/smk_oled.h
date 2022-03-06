//
// Created by hailin on 2022/3/6.
//

#ifndef SIPEED_KEYBOARD_SMK_OLED_H
#define SIPEED_KEYBOARD_SMK_OLED_H

#define oled_rst_pin  GPIO_PIN_18
#define i2c_1_sda_pin GPIO_PIN_11
#define i2c_1_scl_pin GPIO_PIN_12
#define i2c_1_addr1   0x7a
#define i2c_1_addr2   0x78

#define i2c_2_sda_pin GPIO_PIN_9
#define i2c_2_scl_pin GPIO_PIN_10
#define i2c_2_addr1   0x7a
#define i2c_2_addr2   0x78

#define OLED_CMD  0 //写命令
#define OLED_DATA 1 //写数据
#define OLED_UNSAFE_PIN_RESET(pin) *(uint32_t *)(GLB_BASE+GLB_GPIO_OUTPUT_OFFSET)&=~(1<<pin)
#define OLED_UNSAFE_PIN_SET(pin) *(uint32_t *)(GLB_BASE+GLB_GPIO_OUTPUT_OFFSET)|=1<<pin
#ifndef GLB_GPIO_OUTPUT_OFFSET
#define GLB_GPIO_OUTPUT_OFFSET    0x188
#endif

#define OLED_I2C_KHz 10000

#ifdef __cplusplus
extern "C"
#endif
[[noreturn]] [[noreturn]] void smk_oled_demon_task(void *pvParameters);

#endif //SIPEED_KEYBOARD_SMK_OLED_H
