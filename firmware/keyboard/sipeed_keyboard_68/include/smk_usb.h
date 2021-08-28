#ifndef __SMK_USB_H
#define __SMK_USB_H

#include "hal_usb.h"
#include "usbd_core.h"
#include "usbd_msc.h"
#include "usbd_cdc.h"
#include "smk_msc.h"
#include "smk_cdc.h"
#include "smk_hid.h"


void usb_init();

#endif //__SMK_USB_H