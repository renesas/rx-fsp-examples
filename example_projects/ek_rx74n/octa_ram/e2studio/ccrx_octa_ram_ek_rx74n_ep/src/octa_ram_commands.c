/***********************************************************************************************************************
 * File Name    : octa_ram_commands.c
 * Description  : Contains data structures and functions used in octa_ram_commands.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "octa_ram_commands.h"
#include "octa_ram_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup octa_ram_commands.c
 * @{
 **********************************************************************************************************************/

spi_flash_direct_transfer_t g_octa_ram_direct_transfer [OCTA_RAM_TRANSFER_MAX] =
{
    [OCTA_RAM_TRANSFER_WRITE_OCTALRAM_CR_OPI] =
    {
        .command        = OCTA_RAM_COMMAND_WRITE_OCTALRAM_REGISTER,
        .address        = OCTA_RAM_ADDRESS_OCTALRAM_CONFIG_REGISTER,
        .data_u.data    = OCTA_RAM_DATA_OCTALRAM_CONFIG_VALUE,
        .command_length = OCTA_RAM_COMMAND_LENGTH_OPI,
        .address_length = OCTA_RAM_ADDRESS_LENGTH_FOUR,
        .data_length    = OCTA_RAM_DATA_LENGTH_TWO,
        .dummy_cycles   = OCTA_RAM_DUMMY_CYCLES_OCTALRAM_REGISTER_ACCESS,
    },
    [OCTA_RAM_TRANSFER_READ_OCTALRAM_CR_OPI] =
    {
        .command        = OCTA_RAM_COMMAND_READ_OCTALRAM_REGISTER,
        .address        = OCTA_RAM_ADDRESS_OCTALRAM_CONFIG_REGISTER,
        .data_u.data    = OCTA_RAM_DATA_DUMMY,
        .command_length = OCTA_RAM_COMMAND_LENGTH_OPI,
        .address_length = OCTA_RAM_ADDRESS_LENGTH_FOUR,
        .data_length    = OCTA_RAM_DATA_LENGTH_TWO,
        .dummy_cycles   = OCTA_RAM_DUMMY_CYCLES_OCTALRAM_REGISTER_ACCESS
    },
    [OCTA_RAM_TRANSFER_READ_OCTALRAM_ID_OPI] =
    {
        .command        = OCTA_RAM_COMMAND_READ_OCTALRAM_ID,
        .address        = OCTA_RAM_ADDRESS_OCTALRAM_ID_REGISTER,
        .data_u.data    = OCTA_RAM_DATA_DUMMY,
        .command_length = OCTA_RAM_COMMAND_LENGTH_OPI,
        .address_length = OCTA_RAM_ADDRESS_LENGTH_FOUR,
        .data_length    = OCTA_RAM_DATA_LENGTH_TWO,
        .dummy_cycles   = OCTA_RAM_DUMMY_CYCLES_OCTALRAM_REGISTER_ACCESS
    },
    [OCTA_RAM_TRANSFER_WRITE_ENABLE_SPI] =
    {
        .command        = OCTA_RAM_COMMAND_WRITE_ENABLE_SPI,
        .address        = OCTA_RAM_ADDRESS_DUMMY,
        .data_u.data    = OCTA_RAM_DATA_DUMMY,
        .command_length = OCTA_RAM_COMMAND_LENGTH_SPI,
        .address_length = OCTA_RAM_ADDRESS_LENGTH_ZERO,
        .data_length    = OCTA_RAM_DATA_LENGTH_ZERO,
        .dummy_cycles   = OCTA_RAM_DUMMY_CYCLES_WRITE_SPI
    },
    [OCTA_RAM_TRANSFER_WRITE_ENABLE_OPI] =
    {
        .command        = OCTA_RAM_COMMAND_WRITE_ENABLE_OPI,
        .address        = OCTA_RAM_ADDRESS_DUMMY,
        .data_u.data    = OCTA_RAM_DATA_DUMMY,
        .command_length = OCTA_RAM_COMMAND_LENGTH_OPI,
        .address_length = OCTA_RAM_ADDRESS_LENGTH_ZERO,
        .data_length    = OCTA_RAM_DATA_LENGTH_ZERO,
        .dummy_cycles   = OCTA_RAM_DUMMY_CYCLES_WRITE_OPI
    },
    [OCTA_RAM_TRANSFER_READ_STATUS_SPI] =
    {
        .command        = OCTA_RAM_COMMAND_READ_STATUS_SPI,
        .address        = OCTA_RAM_ADDRESS_DUMMY,
        .data_u.data    = OCTA_RAM_DATA_DUMMY,
        .command_length = OCTA_RAM_COMMAND_LENGTH_SPI,
        .address_length = OCTA_RAM_ADDRESS_LENGTH_ZERO,
        .data_length    = OCTA_RAM_DATA_LENGTH_ONE,
        .dummy_cycles   = OCTA_RAM_DUMMY_CYCLES_READ_STATUS_SPI
    },

    [OCTA_RAM_TRANSFER_READ_STATUS_OPI] =
     {
         .command        = OCTA_RAM_COMMAND_READ_STATUS_OPI,
         .address        = OCTA_RAM_ADDRESS_DUMMY,
         .data_u.data    = OCTA_RAM_DATA_DUMMY,
         .command_length = OCTA_RAM_COMMAND_LENGTH_OPI,
         .address_length = OCTA_RAM_ADDRESS_LENGTH_FOUR,
         .data_length    = OCTA_RAM_DATA_LENGTH_TWO,
         .dummy_cycles   = OCTA_RAM_DUMMY_CYCLES_READ_STATUS_OPI
     },

};

/*******************************************************************************************************************//**
 * @} (end addtogroup octa_ram_commands.c)
 **********************************************************************************************************************/
