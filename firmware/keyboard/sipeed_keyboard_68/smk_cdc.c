#include "smk_cdc.h"
#include "shell.h"
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "hal_usb.h"
#include "stdint.h"

extern struct device *usb_fs;
extern struct usbd_interface_cfg usb_cdc;

void usbd_cdc_acm_bulk_out(uint8_t ep)
{
    uint32_t actual_read_length = 0;
    uint8_t out_buffer[64];

    if (usbd_ep_read(ep, out_buffer, 64, &actual_read_length) < 0) {
        MSG("Read DATA Packet failed\r\n");
        usbd_ep_set_stall(ep);
        return;
    }
    for (uint32_t i = 0; i < actual_read_length; i++) {
        shell_handler(out_buffer[i]);
    }
    usbd_ep_read(ep, NULL, 0, NULL);
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

void acm_printf(char *fmt, ...)
{
    char print_buf[64];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(print_buf, sizeof(print_buf) - 1, fmt, ap);
    va_end(ap);
    usbd_ep_write(CDC_IN_EP, (uint8_t *)print_buf, strlen(print_buf), NULL);
}

void smk_cdc_init()
{
    usbd_cdc_add_acm_interface(&cdc_class, &cdc_cmd_intf);
    usbd_cdc_add_acm_interface(&cdc_class, &cdc_data_intf);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_out_ep);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_in_ep);
}