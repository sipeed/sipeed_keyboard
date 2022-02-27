//
// Created by hailin on 2021/8/26.
//

#ifndef SIPEED_KEYBOARD_SMK_HID_PROTOCOL_H
#define SIPEED_KEYBOARD_SMK_HID_PROTOCOL_H

#include <stdint.h>

#define MAX_PROTOCOL_MAP_ENTRIES 20

typedef enum {
    map_type_data=0,
    map_type_func=1
}map_type_t;
typedef enum {
    data_type_fixed=0,
    data_type_ext=1
}data_type_t;
typedef struct {
    uint32_t base;
    uint32_t size;
    map_type_t maptype;
    data_type_t datatype;
    uint8_t * data;
    int (*write)(uint32_t addr,uint8_t* data,uint8_t len);
    int (*read)(uint32_t addr,uint8_t* data,uint8_t len);
}hid_data_reg_t;

#define MAX_PAK_DATALEN 56
typedef struct __packed{
    uint8_t reportID;
    uint8_t datalen;
    uint16_t packageID;
    uint32_t addr;
    uint8_t data[MAX_PAK_DATALEN];
}hid_data_report_t;

void hid_data_protocol_init();
int hid_data_protocal_reg(hid_data_reg_t* pro);
int hid_data_protocal_callback(hid_data_report_t *report,hid_data_reg_t *out);

#endif //SIPEED_KEYBOARD_SMK_HID_PROTOCOL_H
