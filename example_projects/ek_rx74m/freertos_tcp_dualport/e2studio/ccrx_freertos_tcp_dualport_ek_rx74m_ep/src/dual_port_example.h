/*******************************************************************************************************************//**
 * File Name    : dual_port_example.h
 * Description  : This file contains macros, data structures, and function prototypes for the dual port Ethernet example.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef DUAL_PORT_EXAMPLE_H_
#define DUAL_PORT_EXAMPLE_H_

#include "hal_data.h"

#define SERVER_PORT_ETH0 5000
#define SERVER_PORT_ETH1 5001

#if defined (BOARD_RX74M_EK)
#define KIT_NAME                        "EK-RX74M"
#endif

#define EP_INFO \
"This example demonstrates dual Ethernet interfaces on the RX MCU using the FreeRTOS+TCP stack.\r\n" \
"Each interface (ETH0 and ETH1) obtains its network configuration via DHCP.\r\n" \
"After the network is established, the system resolves www.freertos.org using DNS\r\n" \
"and sends a ping to verify connectivity.\r\n" \
"The application then runs two TCP echo servers on different ports.\r\n" \
"On the PC side, a Python script acts as a TCP client, allowing the user to select the IP\r\n" \
"address and port to send data and receive the echoed response.\r\n"

#define IP_BYTES(x)  (x)&0xFF, ((x)>>8)&0xFF, ((x)>>16)&0xFF, ((x)>>24)&0xFF

void vInitializeNetworkInterfaces(void);
void vInitializeNetworkInterfaces_1(void);
void vPrintAllNetworkInfo(void);
void dual_port_example_thread_entry(void *pvParameters);

#endif /* DUAL_PORT_EXAMPLE_H_ */
