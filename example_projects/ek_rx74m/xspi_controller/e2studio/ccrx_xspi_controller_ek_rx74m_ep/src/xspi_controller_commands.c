/***********************************************************************************************************************
 * File Name    : xspi_controller_commands.c
 * Description  : Contains data structures and functions used in xspi_controller_commands.h.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "xspi_controller_commands.h"
#include "xspi_controller_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup xspi_controller_commands.c
 * @{
 **********************************************************************************************************************/

spi_flash_direct_transfer_t g_xspi_controller_direct_transfer [XSPI_CONTROLLER_TRANSFER_MAX] =
{
 /* Transfer structure for SPI mode */
    [XSPI_CONTROLLER_TRANSFER_WRITE_ENABLE_SPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_WRITE_ENABLE_SPI,
    .address        = XSPI_CONTROLLER_ADDRESS_DUMMY,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_SPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_ZERO,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_ZERO,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_WRITE_SPI
    },

    [XSPI_CONTROLLER_TRANSFER_WRITE_CR2_000H_SPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_WRITE_REGISTER_SPI,
    .address        = XSPI_CONTROLLER_ADDRESS_CR2_000H_REGISTER,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_SPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_ONE,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_WRITE_SPI
    },
    [XSPI_CONTROLLER_TRANSFER_WRITE_CR2_300H_SPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_WRITE_REGISTER_SPI,
    .address        = XSPI_CONTROLLER_ADDRESS_CR2_300H_REGISTER,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_SPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_ONE,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_WRITE_SPI
    },

    [XSPI_CONTROLLER_TRANSFER_READ_STATUS_SPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_READ_STATUS_SPI,
    .address        = XSPI_CONTROLLER_ADDRESS_DUMMY,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_SPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_ZERO,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_ONE,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_READ_STATUS_SPI
    },

    [XSPI_CONTROLLER_TRANSFER_READ_CR2_000H_SPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_READ_REGISTER_SPI,
    .address        = XSPI_CONTROLLER_ADDRESS_CR2_000H_REGISTER,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_SPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_ONE,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_READ_REGISTER_SPI
    },
    [XSPI_CONTROLLER_TRANSFER_READ_CR2_300H_SPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_READ_REGISTER_SPI,
    .address        = XSPI_CONTROLLER_ADDRESS_CR2_300H_REGISTER,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_SPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_ONE,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_READ_REGISTER_SPI
    },

    [XSPI_CONTROLLER_TRANSFER_READ_DEVICE_ID_SPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_READ_DEVICE_ID_SPI,
    .address        = XSPI_CONTROLLER_ADDRESS_DUMMY,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_SPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_ZERO,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_THREE,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_READ_STATUS_SPI
    },

    /* Transfer structure for OPI mode */
    [XSPI_CONTROLLER_TRANSFER_WRITE_ENABLE_OPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_WRITE_ENABLE_OPI,
    .address        = XSPI_CONTROLLER_ADDRESS_DUMMY,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_OPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_ZERO,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_ZERO,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_WRITE_OPI
    },

    [XSPI_CONTROLLER_TRANSFER_WRITE_CR2_000H_OPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_WRITE_REGISTER_OPI,
    .address        = XSPI_CONTROLLER_ADDRESS_CR2_000H_REGISTER,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_OPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_TWO,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_WRITE_OPI
    },

    [XSPI_CONTROLLER_TRANSFER_READ_STATUS_OPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_READ_STATUS_OPI,
    .address        = XSPI_CONTROLLER_ADDRESS_DUMMY,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_OPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_TWO,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_READ_STATUS_OPI
    },

    [XSPI_CONTROLLER_TRANSFER_READ_CR2_000H_OPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_READ_REGISTER_OPI,
    .address        = XSPI_CONTROLLER_ADDRESS_CR2_000H_REGISTER,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_OPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_TWO,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_READ_REGISTER_OPI
    },
    [XSPI_CONTROLLER_TRANSFER_READ_CR2_300H_OPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_READ_REGISTER_OPI,
    .address        = XSPI_CONTROLLER_ADDRESS_CR2_300H_REGISTER,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_OPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_TWO,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_READ_REGISTER_OPI
    },

    [XSPI_CONTROLLER_TRANSFER_READ_DEVICE_ID_OPI] =
    {
    .command        = XSPI_CONTROLLER_COMMAND_READ_DEVICE_ID_OPI,
    .address        = XSPI_CONTROLLER_ADDRESS_DUMMY,
    .data_u.data    = XSPI_CONTROLLER_DATA_DUMMY,
    .command_length = XSPI_CONTROLLER_COMMAND_LENGTH_OPI,
    .address_length = XSPI_CONTROLLER_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_CONTROLLER_DATA_LENGTH_THREE,
    .dummy_cycles   = XSPI_CONTROLLER_DUMMY_CYCLE_READ_STATUS_OPI
    },
};

/*******************************************************************************************************************//**
 * @} (end addtogroup xspi_controller_commands.c)
 **********************************************************************************************************************/
