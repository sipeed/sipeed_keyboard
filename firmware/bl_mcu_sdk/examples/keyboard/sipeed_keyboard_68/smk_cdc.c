#include "smk_cdc.h"
#include "usbd_cdc.h"
#include "usbd_core.h"
#include "uart_interface.h"
#include "hal_usb.h"
#include "stdint.h"

extern struct device *usb_fs;
extern struct usbd_interface_cfg usb_cdc;

void usbd_cdc_acm_bulk_out(uint8_t ep)
{
    usb_dc_receive_to_ringbuffer(usb_fs, &usb_rx_rb, ep);
}

void usbd_cdc_acm_bulk_in(uint8_t ep)
{
    usb_dc_send_from_ringbuffer(usb_fs, &uart1_rx_rb, ep);
}
void usbd_cdc_acm_set_line_coding(uint32_t baudrate, uint8_t databits, uint8_t parity, uint8_t stopbits)
{
    uart1_config(baudrate, databits, parity, stopbits);
}

void usbd_cdc_acm_set_dtr(bool dtr)
{
    dtr_pin_set(dtr);
}

void usbd_cdc_acm_set_rts(bool rts)
{
    rts_pin_set(rts);
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
    .ep_cb = usbd_cdc_acm_bulk_in
};

void smk_cdc_init()
{
    uart_ringbuffer_init();
    uart1_init();
    uart1_dtr_init();
    uart1_rts_init();

    usbd_cdc_add_acm_interface(&cdc_class, &cdc_cmd_intf);
    usbd_cdc_add_acm_interface(&cdc_class, &cdc_data_intf);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_out_ep);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_in_ep);
}