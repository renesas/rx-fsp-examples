/***********************************************************************************************************************
 * File Name    : xspi_controller_ep.h
 * Description  : Contains data structures and functions used in xspi_controller_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef XSPI_CONTROLLER_EP_H_
#define XSPI_CONTROLLER_EP_H_

/* Flash device sector size */
#define XSPI_CONTROLLER_SECTOR_SIZE_4K               (0x1000)

/* Flash device timing */
#define XSPI_CONTROLLER_TIME_UNIT                    (BSP_DELAY_UNITS_MICROSECONDS)
#define XSPI_CONTROLLER_TIME_RESET_SETUP             (2U)             /*  Type 50ns */
#define XSPI_CONTROLLER_TIME_RESET_PULSE             (1000U)          /*  Type 500us */
#define XSPI_CONTROLLER_TIME_ERASE_4K                (100000U)        /*  Type 4KB sector is 95 KBps -> Type 0.042s */
#define XSPI_CONTROLLER_TIME_WRITE                   (10000U)         /*  Type 256B page (4KB/256KB) is 595/533 KBps -> Type */

/* Flash device status bit */
#define XSPI_CONTROLLER_WEN_BIT_MASK                 (0x00000002)

/* Flash device address space mapping */
#define XSPI_CONTROLLER_CS0_START_ADDRESS            (0x30000000)
#define XSPI_CONTROLLER_APP_ADDRESS(sector_no)       ((uint8_t *)(XSPI_CONTROLLER_CS0_START_ADDRESS + ((sector_no) * XSPI_CONTROLLER_SECTOR_SIZE_4K)))
#define XSPI_CONTROLLER_SECTOR_THREE                 (2U)

/* Function declarations*/
fsp_err_t xspi_controller_init(void);

#endif /* XSPI_CONTROLLER_EP_H_ */
