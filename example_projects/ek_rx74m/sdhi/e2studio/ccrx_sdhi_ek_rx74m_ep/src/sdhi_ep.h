/***********************************************************************************************************************
 * File Name    : sdhi_ep.h
 * Description  : This file contains data structures and functions used in sdhi_ep.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SDHI_EP_H_
#define SDHI_EP_H_

/* Macros used in SHDI Example Project */
#define EP_INFO "\r\nThis example project demonstrates the basic use of SDHI on Renesas RX MCUs.\r\n"                             \
                "The project uses SDHI driver to perform data writing and reading operations directly on the Micro SD card. \r\n" \
                "The example also includes functionality for clearing the buffer (erase) from the Micro SD card when needed.\r\n" \
                "Unlike full file system operations, this example focuses solely on low-level block media operations such as\r\n" \
                "write, read, and erase using the SDHI interface.\r\n"

#define SD_INFO "\r\n********************* Information Micro SD *********************\r\n" \
                "\r\nMicro SD available memory %d"

#define EP_MENU "\r\n******** Menu options ********"      \
                "\r\n1. Erase all data in Micro SD Card"  \
                "\r\n2. Write 2 MB data to Micro SD Card" \
                "\r\n3. Read data from Micro SD Card"     \
                "\r\nInput value: "

#define SD_STATUS(status) ((status) ? "\r\nWrite protected is true (Please disable Micro SD Write Protected to continue...)\r\n" \
                                    : "\r\nWrite protected is false\r\n")

#define SET_FLAG                (1u)
#define RANDOM_VALUE            (256)
#define CONVERT_BYTE_TO_MEGABYTE ((float) 1 / (1024 * 1024))
#define SD_ERASE                (1u)
#define SD_WRITE                (2u)
#define SD_READ                 (3u)
#define SDMMC_BUFFER_SIZE       (2048u)
#define INITIAL_VALUE            '\0'
#define SECTOR_COUNT            (4u)
#define SDHI_PE_PIN             (BSP_IO_PORT_07_PIN_01)

/* Function Declaration */
void sdhi_entry(void);

#endif /* SDHI_EP_H_ */
