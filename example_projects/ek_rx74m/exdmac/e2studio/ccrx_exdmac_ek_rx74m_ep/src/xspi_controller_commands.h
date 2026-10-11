/***********************************************************************************************************************
 * File Name    : xspi_controller_commands.h
 * Description  : Contains data structures and functions used in xspi_controller_ep.c
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef XSPI_CONTROLLER_COMMANDS_H_
#define XSPI_CONTROLLER_COMMANDS_H_

#include "hal_data.h"

/* Macros for XSPI Controller command code */
#define XSPI_CONTROLLER_COMMAND_WRITE_ENABLE_SPI             (0x06)
#define XSPI_CONTROLLER_COMMAND_WRITE_ENABLE_OPI             (0x06F9)
#define XSPI_CONTROLLER_COMMAND_WCR2_SPI                     (0x72)
#define XSPI_CONTROLLER_COMMAND_WCR2_OPI                     (0x728D)
#define XSPI_CONTROLLER_COMMAND_WRITE_REGISTER_SPI           (XSPI_CONTROLLER_COMMAND_WCR2_SPI)
#define XSPI_CONTROLLER_COMMAND_WRITE_REGISTER_OPI           (XSPI_CONTROLLER_COMMAND_WCR2_OPI)
#define XSPI_CONTROLLER_COMMAND_READ_STATUS_SPI              (0x05)
#define XSPI_CONTROLLER_COMMAND_READ_STATUS_OPI              (0x05FA)
#define XSPI_CONTROLLER_COMMAND_RCR2_SPI                     (0x71)
#define XSPI_CONTROLLER_COMMAND_RCR2_OPI                     (0x718E)
#define XSPI_CONTROLLER_COMMAND_READ_REGISTER_SPI            (XSPI_CONTROLLER_COMMAND_RCR2_SPI)
#define XSPI_CONTROLLER_COMMAND_READ_REGISTER_OPI            (XSPI_CONTROLLER_COMMAND_RCR2_OPI)

/* Macros for OSPI command length */
#define XSPI_CONTROLLER_COMMAND_LENGTH_SPI                   (1U)
#define XSPI_CONTROLLER_COMMAND_LENGTH_OPI                   (2U)

/* Macros for OSPI transfer address */
#define XSPI_CONTROLLER_ADDRESS_DUMMY                        (0U)
#define XSPI_CONTROLLER_ADDRESS_LENGTH_ZERO                  (0U)
#define XSPI_CONTROLLER_ADDRESS_LENGTH_FOUR                  (4U)

/* Macros for OSPI transfer data */
#define XSPI_CONTROLLER_DATA_DUMMY                           (0U)
#define XSPI_CONTROLLER_DATA_LENGTH_ZERO                     (0U)
#define XSPI_CONTROLLER_DATA_LENGTH_ONE                      (1U)
#define XSPI_CONTROLLER_DATA_LENGTH_TWO                      (2U)

/* Macros for OSPI transfer dummy cycles */
#define XSPI_CONTROLLER_DUMMY_CYCLE_WRITE_SPI                (0U)
#define XSPI_CONTROLLER_DUMMY_CYCLE_WRITE_OPI                (0U)
#define XSPI_CONTROLLER_DUMMY_CYCLE_READ_STATUS_SPI          (0U)
#define XSPI_CONTROLLER_DUMMY_CYCLE_READ_STATUS_OPI          (4U)
#define XSPI_CONTROLLER_DUMMY_CYCLE_READ_REGISTER_SPI        (0U)
#define XSPI_CONTROLLER_DUMMY_CYCLE_READ_REGISTER_OPI        (4U)

/* Macros for flash device register address */
#define XSPI_CONTROLLER_ADDRESS_CR2_000H_REGISTER            (0x00000000)
#define XSPI_CONTROLLER_ADDRESS_CR2_300H_REGISTER            (0x00000300)

/* Macros for configure flash device */
#define XSPI_CONTROLLER_DATA_SET_CR2_300H                    (0x05)

typedef enum e_XSPI_CONTROLLER_transfer
{
    XSPI_CONTROLLER_TRANSFER_WRITE_ENABLE_SPI = 0,

    XSPI_CONTROLLER_TRANSFER_WRITE_CR2_300H_SPI,
    XSPI_CONTROLLER_TRANSFER_READ_STATUS_SPI,
    XSPI_CONTROLLER_TRANSFER_READ_CR2_300H_SPI,

    XSPI_CONTROLLER_TRANSFER_WRITE_ENABLE_OPI,
    XSPI_CONTROLLER_TRANSFER_READ_STATUS_OPI,

    XSPI_CONTROLLER_TRANSFER_MAX
} xspi_controller_transfer_t;

#endif /* XSPI_CONTROLLER_COMMANDS_H_ */
