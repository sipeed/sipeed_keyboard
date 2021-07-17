#ifndef __SMK_USB_H
#define __SMK_USB_H

#include "hal_usb.h"
#include "usbd_core.h"
#include "usbd_msc.h"


void usbd_msc_get_cap(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
int usbd_msc_sector_read(uint32_t sector, uint8_t *buffer, uint32_t length);
int usbd_msc_sector_write(uint32_t sector, uint8_t *buffer, uint32_t length);
void usb_init();

#endif //__SMK_USB_H