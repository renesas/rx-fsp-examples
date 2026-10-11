/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
* File Name    : xspi_commands.c
* Description  : Contains xspi function definition.
**********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 19.02.2026 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "xspi_commands.h"
#include "xspi_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup xspi_commands.c
 * @{
 **********************************************************************************************************************/

spi_flash_direct_transfer_t g_xspi_direct_transfer [XSPI_TRANSFER_MAX] =
{
    /* Transfer structure for SPI mode */
    [XSPI_TRANSFER_WRITE_ENABLE_SPI] =
    {
    .command        = XSPI_COMMAND_WRITE_ENABLE_SPI,
    .address        = XSPI_ADDRESS_DUMMY,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_SPI,
    .address_length = XSPI_ADDRESS_LENGTH_ZERO,
    .data_length    = XSPI_DATA_LENGTH_ZERO,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_WRITE_SPI
    },

    [XSPI_TRANSFER_WRITE_CR2_000H_SPI] =
    {
    .command        = XSPI_COMMAND_WRITE_REGISTER_SPI,
    .address        = XSPI_ADDRESS_CR2_000H_REGISTER,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_SPI,
    .address_length = XSPI_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_DATA_LENGTH_ONE,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_WRITE_SPI
    },

    [XSPI_TRANSFER_WRITE_CR2_300H_SPI] =
    {
    .command        = XSPI_COMMAND_WRITE_REGISTER_SPI,
    .address        = XSPI_ADDRESS_CR2_300H_REGISTER,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_SPI,
    .address_length = XSPI_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_DATA_LENGTH_ONE,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_WRITE_SPI
    },

    [XSPI_TRANSFER_READ_STATUS_SPI] =
    {
    .command        = XSPI_COMMAND_READ_STATUS_SPI,
    .address        = XSPI_ADDRESS_DUMMY,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_SPI,
    .address_length = XSPI_ADDRESS_LENGTH_ZERO,
    .data_length    = XSPI_DATA_LENGTH_ONE,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_READ_STATUS_SPI
    },

    [XSPI_TRANSFER_READ_CR2_000H_SPI] =
    {
    .command        = XSPI_COMMAND_READ_REGISTER_SPI,
    .address        = XSPI_ADDRESS_CR2_000H_REGISTER,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_SPI,
    .address_length = XSPI_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_DATA_LENGTH_ONE,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_READ_REGISTER_SPI
    },

    [XSPI_TRANSFER_READ_CR2_300H_SPI] =
    {
    .command        = XSPI_COMMAND_READ_REGISTER_SPI,
    .address        = XSPI_ADDRESS_CR2_300H_REGISTER,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_SPI,
    .address_length = XSPI_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_DATA_LENGTH_ONE,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_READ_REGISTER_SPI
    },

    [XSPI_TRANSFER_READ_DEVICE_ID_SPI] =
    {
    .command        = XSPI_COMMAND_READ_DEVICE_ID_SPI,
    .address        = XSPI_ADDRESS_DUMMY,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_SPI,
    .address_length = XSPI_ADDRESS_LENGTH_ZERO,
    .data_length    = XSPI_DATA_LENGTH_THREE,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_READ_STATUS_SPI
    },

    /* Transfer structure for OPI mode */
    [XSPI_TRANSFER_WRITE_ENABLE_OPI] =
    {
    .command        = XSPI_COMMAND_WRITE_ENABLE_OPI,
    .address        = XSPI_ADDRESS_DUMMY,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_OPI,
    .address_length = XSPI_ADDRESS_LENGTH_ZERO,
    .data_length    = XSPI_DATA_LENGTH_ZERO,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_WRITE_OPI
    },

    [XSPI_TRANSFER_WRITE_CR2_000H_OPI] =
    {
    .command        = XSPI_COMMAND_WRITE_REGISTER_OPI,
    .address        = XSPI_ADDRESS_CR2_000H_REGISTER,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_OPI,
    .address_length = XSPI_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_DATA_LENGTH_TWO,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_WRITE_OPI
    },

    [XSPI_TRANSFER_WRITE_CR2_300H_OPI] =
    {
    .command        = XSPI_COMMAND_WRITE_REGISTER_OPI,
    .address        = XSPI_ADDRESS_CR2_300H_REGISTER,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_OPI,
    .address_length = XSPI_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_DATA_LENGTH_TWO,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_WRITE_OPI
    },

    [XSPI_TRANSFER_READ_STATUS_OPI] =
    {
    .command        = XSPI_COMMAND_READ_STATUS_OPI,
    .address        = XSPI_ADDRESS_DUMMY,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_OPI,
    .address_length = XSPI_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_DATA_LENGTH_TWO,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_READ_STATUS_OPI
    },

    [XSPI_TRANSFER_READ_CR2_000H_OPI] =
    {
    .command        = XSPI_COMMAND_READ_REGISTER_OPI,
    .address        = XSPI_ADDRESS_CR2_000H_REGISTER,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_OPI,
    .address_length = XSPI_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_DATA_LENGTH_TWO,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_READ_REGISTER_OPI
    },
    [XSPI_TRANSFER_READ_CR2_300H_OPI] =
    {
    .command        = XSPI_COMMAND_READ_REGISTER_OPI,
    .address        = XSPI_ADDRESS_CR2_300H_REGISTER,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_OPI,
    .address_length = XSPI_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_DATA_LENGTH_TWO,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_READ_REGISTER_OPI
    },

    [XSPI_TRANSFER_READ_DEVICE_ID_OPI] =
    {
    .command        = XSPI_COMMAND_READ_DEVICE_ID_OPI,
    .address        = XSPI_ADDRESS_DUMMY,
    .data_u.data         = XSPI_DATA_DUMMY,
    .command_length = XSPI_COMMAND_LENGTH_OPI,
    .address_length = XSPI_ADDRESS_LENGTH_FOUR,
    .data_length    = XSPI_DATA_LENGTH_THREE,
    .dummy_cycles   = XSPI_DUMMY_CYCLE_READ_STATUS_OPI
    },
};

/*******************************************************************************************************************//**
 * @} (end addtogroup xspi_commands.c)
 **********************************************************************************************************************/
