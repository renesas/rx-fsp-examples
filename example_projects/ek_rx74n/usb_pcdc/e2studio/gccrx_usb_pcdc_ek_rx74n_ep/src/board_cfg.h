/***********************************************************************************************************************
 * File Name    : board_cfg.h
 * Description  : Board specific/configuration data.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"

#define USB_EP_PACKET_SIZE         (512U)
#define OFF                        (0U)
#define ON                         (1U)

#define BLUE                       (IOPORT_PORT_15_PIN_05)
#define GREEN                      (IOPORT_PORT_08_PIN_07)
#define RED                        (IOPORT_PORT_06_PIN_04)
#define LED_COUNT                  (3U)

#define TURN_RED_ON                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) RED, BSP_IO_LEVEL_HIGH);\
		                           R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) GREEN, BSP_IO_LEVEL_LOW);\
		                           R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) BLUE , BSP_IO_LEVEL_LOW);

#define KIT_NAME_MACRO             "EK-RX74N"
#define KIT_PARTNUM_MACRO          "RTK5EK74N0S00001BJ"
#define KIT_LANDING_URL            "renesas.com/rx/ek-rx74n"
#define PRODUCT_INFO_URL           "renesas.com/rx"
#define PRODUCT_SUPPORT_URL        "renesas.com/rx/forum"
#define RENESAS_SUPPORT_URL        "renesas.com/support"
