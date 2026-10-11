/*
`* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : xspi_commands.h
 * Description  : Contains data structures and functions used in xspi_commands.h
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 19.02.2026 1.00     First Release
 *********************************************************************************************************************/

#ifndef XSPI_COMMANDS_H_
#define XSPI_COMMANDS_H_

#include "hal_data.h"

/* Macros for XSPI command code */
#define XSPI_COMMAND_WRITE_ENABLE_SPI             (0x06)
#define XSPI_COMMAND_WRITE_ENABLE_OPI             (0x06F9)
#define XSPI_COMMAND_WRITE_REGISTER_SPI           (0x72)
#define XSPI_COMMAND_WRITE_REGISTER_OPI           (0x728D)
#define XSPI_COMMAND_READ_STATUS_SPI              (0x05)
#define XSPI_COMMAND_READ_STATUS_OPI              (0x05FA)
#define XSPI_COMMAND_READ_REGISTER_SPI            (0x71)
#define XSPI_COMMAND_READ_REGISTER_OPI            (0x718E)
#define XSPI_COMMAND_READ_DEVICE_ID_SPI           (0x9F)
#define XSPI_COMMAND_READ_DEVICE_ID_OPI           (0x9F60)

/* Macros for XSPI command length */
#define XSPI_COMMAND_LENGTH_SPI                   (1U)
#define XSPI_COMMAND_LENGTH_OPI                   (2U)

/* Macros for XSPI transfer address */
#define XSPI_ADDRESS_DUMMY                        (0U)
#define XSPI_ADDRESS_LENGTH_ZERO                  (0U)
#define XSPI_ADDRESS_LENGTH_THREE                 (3U)
#define XSPI_ADDRESS_LENGTH_FOUR                  (4U)

/* Macros for XSPI transfer data */
#define XSPI_DATA_DUMMY                           (0U)
#define XSPI_DATA_LENGTH_ZERO                     (0U)
#define XSPI_DATA_LENGTH_ONE                      (1U)
#define XSPI_DATA_LENGTH_TWO                      (2U)
#define XSPI_DATA_LENGTH_THREE                    (3U)
#define XSPI_DATA_LENGTH_FOUR                     (4U)

/* Macros for XSPI transfer dummy cycles */
#define XSPI_DUMMY_CYCLE_WRITE_SPI                (0U)
#define XSPI_DUMMY_CYCLE_WRITE_OPI                (0U)
#define XSPI_DUMMY_CYCLE_READ_STATUS_SPI          (0U)
#define XSPI_DUMMY_CYCLE_READ_STATUS_OPI          (4U)
#define XSPI_DUMMY_CYCLE_READ_REGISTER_SPI        (0U)
#define XSPI_DUMMY_CYCLE_READ_REGISTER_OPI        (4U)

/* Macros for flash device register address */
#define XSPI_ADDRESS_CR2_000H_REGISTER            (0x00000000)
#define XSPI_ADDRESS_CR2_300H_REGISTER            (0x00000300)

/* Macros for configure flash device */
#define XSPI_DATA_SET_SPI_CR2_000H                (0x00)
#define XSPI_DATA_SET_OPI_CR2_000H                (0x02)
#define XSPI_DATA_SET_SPI_CR2_300H                (0x00)
#define XSPI_DATA_SET_OPI_CR2_300H                (0x00)
#define XSPI_DATA_SR_WRITE_ENABLE                 (0x02)

#define XSPI_MASK_CR2_300H (0x07U)

typedef enum e_xspi_transfer
{
    XSPI_TRANSFER_WRITE_ENABLE_SPI = 0,
    XSPI_TRANSFER_WRITE_CR2_000H_SPI,
    XSPI_TRANSFER_WRITE_CR2_300H_SPI,
    XSPI_TRANSFER_READ_STATUS_SPI,
    XSPI_TRANSFER_READ_CR2_000H_SPI,
    XSPI_TRANSFER_READ_CR2_300H_SPI,
    XSPI_TRANSFER_READ_DEVICE_ID_SPI,
    XSPI_TRANSFER_WRITE_ENABLE_OPI,
    XSPI_TRANSFER_WRITE_CR2_000H_OPI,
    XSPI_TRANSFER_WRITE_CR2_300H_OPI,
    XSPI_TRANSFER_READ_STATUS_OPI,
    XSPI_TRANSFER_READ_CR2_000H_OPI,
    XSPI_TRANSFER_READ_CR2_300H_OPI,
    XSPI_TRANSFER_READ_DEVICE_ID_OPI,
    XSPI_TRANSFER_MAX,
} xspi_transfer_t;

#endif /* XSPI_COMMANDS_H_ */
