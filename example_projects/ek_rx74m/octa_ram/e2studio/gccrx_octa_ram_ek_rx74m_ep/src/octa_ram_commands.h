/***********************************************************************************************************************
 * File Name    : octa_ram_commands.h
 * Description  : Contains data structures and functions used in octa_ram_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OCTA_RAM_COMMANDS_H_
#define OCTA_RAM_COMMANDS_H_

#include "hal_data.h"

/* Macros for OSPI command length */
#define OCTA_RAM_COMMAND_WRITE_ENABLE_SPI             (0x06)
#define OCTA_RAM_COMMAND_WRITE_ENABLE_OPI             (0x06F9)
#define OCTA_RAM_COMMAND_READ_STATUS_SPI              (0x05)
#define OCTA_RAM_COMMAND_READ_STATUS_OPI              (0x05FA)

#define OCTA_RAM_DUMMY_CYCLES_WRITE_SPI                (0U)
#define OCTA_RAM_DUMMY_CYCLES_WRITE_OPI                (0U)
#define OCTA_RAM_DUMMY_CYCLES_READ_STATUS_SPI          (0U)
#define OCTA_RAM_DUMMY_CYCLES_READ_STATUS_OPI          (4U)

#define OCTA_RAM_COMMAND_LENGTH_SPI                   (1U)
#define OCTA_RAM_COMMAND_LENGTH_OPI                   (1U)

/* Macros for OSPI transfer address */
#define OCTA_RAM_ADDRESS_DUMMY                        (0U)
#define OCTA_RAM_ADDRESS_LENGTH_ZERO                  (0U)
#define OCTA_RAM_ADDRESS_LENGTH_THREE                 (3U)
#define OCTA_RAM_ADDRESS_LENGTH_FOUR                  (4U)

/* Macros for OSPI transfer data */
#define OCTA_RAM_DATA_DUMMY                           (0U)
#define OCTA_RAM_DATA_LENGTH_ZERO                     (0U)
#define OCTA_RAM_DATA_LENGTH_ONE                      (1U)
#define OCTA_RAM_DATA_LENGTH_TWO                      (2U)
#define OCTA_RAM_DATA_LENGTH_THREE                    (3U)
#define OCTA_RAM_DATA_LENGTH_FOUR                     (4U)


/* Macros for OctalRAM device registers */
#define OCTA_RAM_COMMAND_WRITE_OCTALRAM_REGISTER      (0x20)       /* OctalRAM Write Register command */
#define OCTA_RAM_COMMAND_READ_OCTALRAM_REGISTER       (0xA0)       /* OctalRAM Read Register command */
#define OCTA_RAM_COMMAND_READ_OCTALRAM_ID             (0xC0)       /* OctalRAM Read ID command */
#define OCTA_RAM_ADDRESS_OCTALRAM_ID_REGISTER         (0x00000000) /* OctalRAM ID Register address */
#define OCTA_RAM_ADDRESS_OCTALRAM_CONFIG_REGISTER     (0x00000400) /* OctalRAM Configuration Register address */
#define OCTA_RAM_DUMMY_CYCLES_OCTALRAM_REGISTER_ACCESS (5U)        /* Dummy cycles for OctalRAM register access */
#define OCTA_RAM_DATA_OCTALRAM_CONFIG_VALUE           (0x00)       /* Config value for OctalRAM (Fixed Latency, 3V) */

typedef enum e_octa_ram_transfer
{
    OCTA_RAM_TRANSFER_WRITE_OCTALRAM_CR_OPI = 0,
    OCTA_RAM_TRANSFER_READ_OCTALRAM_CR_OPI,
    OCTA_RAM_TRANSFER_READ_OCTALRAM_ID_OPI,
    OCTA_RAM_TRANSFER_WRITE_ENABLE_SPI,
    OCTA_RAM_TRANSFER_WRITE_ENABLE_OPI,
    OCTA_RAM_TRANSFER_READ_STATUS_SPI,
    OCTA_RAM_TRANSFER_READ_STATUS_OPI,
    OCTA_RAM_TRANSFER_MAX
} octa_ram_transfer_t;

#endif /* OCTA_RAM_COMMANDS_H_ */
