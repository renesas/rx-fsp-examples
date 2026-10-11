/***********************************************************************************************************************
 * File Name    : usb_host_vendor_thread_entry.h
 * Description  : Contains macros definitions.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_HOST_VENDOR_THREAD_ENTRY_H_
#define USB_HOST_VENDOR_THREAD_ENTRY_H_

#define BUF_SIZE               (20)                /* Buffer size */
#define REQ_SIZE               (20)                /* Request buffer size */
#define ADDRESS                (1)                 /* USB address */
#define START_PIPE             (1)                 /* Start pipe number */
#define END_PIPE               (USB_PIPE9 + 1)     /* Total pipe */

/* For Vendor Class Request */
#define USB_SET_VENDOR_NO_DATA (0x0000U)
#define USB_SET_VENDOR         (0x0100U)
#define USB_GET_VENDOR         (0x0200U)
#define SET_VENDOR_NO_DATA     (USB_SET_VENDOR_NO_DATA | USB_HOST_TO_DEV | USB_VENDOR | USB_INTERFACE)
#define SET_VENDOR             (USB_SET_VENDOR | USB_HOST_TO_DEV | USB_VENDOR | USB_INTERFACE)
#define GET_VENDOR             (USB_GET_VENDOR | USB_DEV_TO_HOST | USB_VENDOR | USB_INTERFACE)
#define DELAY                  (10U)               /* Delay for print */
#define USB_VALUE_FF           (0xFF)              /* FF macro */
#define USB_APL_MXPS           (512U)              /* Max packet size for High Speed */
#define EP_INFO "\r\nThe project demonstrates the basic functionalities of USB Host Vendor class driver\r\n"\
                "on Renesas RX MCUs based on Renesas FSP. In this example, the application will configure\r\n"\
                "the MCU as a Vendor Host device. This host device will be connected to the USB Peripheral\r\n"\
                "Vendor device which is another RX board. After the enumeration is completed, the Vendor\r\n"\
                "Host board will write 15 bytes of data to the Vendor Peripheral board and read back the\r\n"\
                "same data from the Vendor Peripheral board. The Vendor Host and Vendor Peripheral applications\r\n"\
                "are designed to continuously transfer data between both the boards. The user will be able\r\n"\
                "to see all the operation sequence and status on the Terminal Application.\r\n"

#endif /* USB_HOST_VENDOR_THREAD_ENTRY_H_ */
