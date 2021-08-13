#include "smk_cdc.h"
#include "smk_shell.h"
#include "usbd_cdc.h"
#include "usbd_core.h"
#include "hal_usb.h"
#include "stdint.h"

extern struct device *usb_fs;
extern struct usbd_interface_cfg usb_cdc;

void usbd_cdc_acm_bulk_out(uint8_t ep)
{
    uint32_t actual_read_length = 0;
    uint8_t out_buffer[64];
    uint8_t data;

    if (usbd_ep_read(ep, out_buffer, 64, &actual_read_length) < 0) {
        USBD_LOG_DBG("Read DATA Packet failed\r\n");
        usbd_ep_set_stall(ep);
        return;
    }
    usbd_ep_read(ep, NULL, 0, NULL);
    data = *(uint8_t *)out_buffer;
    shell_handler(data);
}

void usbd_cdc_acm_bulk_in(uint8_t ep)
{
}

usbd_class_t cdc_class;
usbd_interface_t cdc_cmd_intf;
usbd_interface_t cdc_data_intf;

usbd_endpoint_t cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

usbd_endpoint_t cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = NULL
};

void smk_cdc_init()
{
    usbd_cdc_add_acm_interface(&cdc_class, &cdc_cmd_intf);
    usbd_cdc_add_acm_interface(&cdc_class, &cdc_data_intf);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_out_ep);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_in_ep);
}