#include "smk_usb.h"
#include "usbd_core.h"
#include "usbd_hid.h"
#include "hal_usb.h"
#include "smk_hid.h"

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

// #define USB_CONFIG_HID_KEYBOARD_SIZE (9 + HID_DESCRIPTOR_LEN)
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN + HID_KEYBOARD_DESCRIPTOR_LEN +HID_DATA_DESCRIPTOR_LEN + HID_NKRO_DESCRIPTOR_LEN)

USB_DESC_SECTION const uint8_t sipeed_keyboard_descriptor[] = { // single hid keyboard device desc
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    // USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x05, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    // USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_HID_KEYBOARD_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, 0x02),
    // MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, 0x02),
        /************** Descriptor of Joystick Mouse interface ****************/
        /* 09 */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x02,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x01,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x01,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x01,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
        /******************** Descriptor of Joystick Mouse HID ********************/
        /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                          /* bCountryCode: Hardware target country */
    0x01,                          /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                          /* bDescriptorType */
    HID_KEYBOARD_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
        /******************** Descriptor of Mouse endpoint ********************/
        /* 27 */

    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HID_KB_INT_EP,                   /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    HID_KB_INT_EP_SIZE,              /* wMaxPacketSize: 4 Byte max */
    0x00,
    HID_KB_INT_EP_INTERVAL, /* bInterval: Polling Interval */

        /************** Descriptor of Joystick Mouse interface ****************/
        /* 09 */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x03,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x02,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x00,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
        /******************** Descriptor of Joystick Mouse HID ********************/
        /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                          /* bCountryCode: Hardware target country */
    0x01,                          /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                          /* bDescriptorType */
    HID_DATA_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
        /******************** Descriptor of Mouse endpoint ********************/
        /* 27 */
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HID_DATA_OUT_EP,                   /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    HID_DATA_OUT_EP_SIZE,              /* wMaxPacketSize: 4 Byte max */
    0x00,
    HID_DATA_OUT_EP_INTERVAL, /* bInterval: Polling Interval */

    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HID_DATA_INT_EP,                   /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    HID_DATA_INT_EP_SIZE,              /* wMaxPacketSize: 4 Byte max */
    0x00,
    HID_DATA_INT_EP_INTERVAL, /* bInterval: Polling Interval */

    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x04,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x01,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x00,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x01,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                          /* bCountryCode: Hardware target country */
    0x01,                          /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                          /* bDescriptorType */
    HID_NKRO_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Mouse endpoint ********************/
    /* 27 */

    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HID_NKRO_INT_EP,                   /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    HID_NKRO_INT_EP_SIZE,              /* wMaxPacketSize: 4 Byte max */
    0x00,
    HID_NKRO_INT_EP_INTERVAL, /* bInterval: Polling Interval */

        ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x0e,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'S', 0x00,                  /* wcChar0 */
    'i', 0x00,                  /* wcChar1 */
    'p', 0x00,                  /* wcChar2 */
    'e', 0x00,                  /* wcChar3 */
    'e', 0x00,                  /* wcChar4 */
    'd', 0x00,                  /* wcChar5 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x20,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'S', 0x00,                  /* wcChar0 */
    'i', 0x00,                  /* wcChar1 */
    'p', 0x00,                  /* wcChar2 */
    'e', 0x00,                  /* wcChar3 */
    'e', 0x00,                  /* wcChar4 */
    'd', 0x00,                  /* wcChar5 */
    ' ', 0x00,                  /* wcChar6 */
    'K', 0x00,                  /* wcChar7 */
    'e', 0x00,                  /* wcChar8 */
    'y', 0x00,                  /* wcChar9 */
    'b', 0x00,                  /* wcChar10 */
    'o', 0x00,                  /* wcChar11 */
    'a', 0x00,                  /* wcChar12 */
    'r', 0x00,                  /* wcChar13 */
    'd', 0x00,                  /* wcChar14 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '1', 0x00,                  /* wcChar3 */
    '0', 0x00,                  /* wcChar4 */
    '3', 0x00,                  /* wcChar5 */
    '1', 0x00,                  /* wcChar6 */
    '0', 0x00,                  /* wcChar7 */
    '0', 0x00,                  /* wcChar8 */
    '0', 0x00,                  /* wcChar9 */
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,

    0x00
};

struct device *usb_fs;

extern struct device *usb_dc_init(void);

void usb_init(){ //task init
    usbd_desc_register(sipeed_keyboard_descriptor);
    smk_cdc_init();
    smk_hid_usb_init();

    usb_fs = usb_dc_init();

    if (usb_fs) {
        device_control(usb_fs, DEVICE_CTRL_SET_INT, (void *)(USB_EP1_DATA_IN_IT |USB_EP2_DATA_OUT_IT | USB_EP3_DATA_OUT_IT | USB_EP5_DATA_IN_IT| USB_EP6_DATA_IN_IT |USB_EP7_DATA_IN_IT));
    }

    while (!usb_device_is_configured()) {
        vTaskDelay(100);
    }

    // while (1) {
    // }
}