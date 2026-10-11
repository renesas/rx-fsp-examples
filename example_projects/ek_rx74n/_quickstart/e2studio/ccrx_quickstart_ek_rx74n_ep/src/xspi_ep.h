/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : xspi_ep.h
 * Version      : 1.0
 * Description  : Contains data structures and functions used in xspi_ep
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 19.02.2026 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/**********************************************************************************************************************
Macro definitions
**********************************************************************************************************************/
#ifndef XSPI_EP_H_
#define XSPI_EP_H_

/* Macro for flash device */
#define XSPI_DEVICE_ID                    (0x3A86C2)

/* Flash device sector size */
#define XSPI_SECTOR_SIZE                  (0x1000)

/* Flash device address space mapping */
#define XSPI_APP_ADDRESS(sector_no)    ((uint8_t *)(XSPI_CS0_START_ADDRESS + ((sector_no) * XSPI_SECTOR_SIZE_4K)))
#define XSPI_CS0_START_ADDRESS         (0x30000000)

/* Flash device timing */
#define XSPI_TIME_UNIT                 (BSP_DELAY_UNITS_MICROSECONDS)
#define XSPI_TIME_ERASE_4K             (800000U)                        /*  Max 4KB sector is 400ms */
#define XSPI_TIME_WRITE                (1500U)                           /*  Max 4KB sector is 750us */
#define XSPI_TIME_RESET_SETUP          (20U)                            /*  Min 10us */
#define XSPI_TIME_RESET_PULSE          (70U)                            /*  Min 35us */

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern fsp_err_t xspi_init ();
extern fsp_err_t xspi_wait_operation ();

#endif /* XSPI_EP_H_ */
