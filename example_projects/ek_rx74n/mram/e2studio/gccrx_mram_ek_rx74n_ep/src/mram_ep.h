/***********************************************************************************************************************
 * File Name    : mram_ep.h
 * Description  : This file contains data structures, macros and functions used in mram_ep.c.
 *                It implements the MRAM functionality for the example project. It handles erase, write, read
 *                operations to code mram and data mram memory
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef MRAM_EP_H_
#define MRAM_EP_H_

/*******************************************************************************************************************//**
 * @defgroup MRAM_EP
 * @{
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * Macro definition Section
 **********************************************************************************************************************/
/* This macro is used to display the EP information */
#define MRAM_EP_INFO            "\r\nThis example project demonstrates the basic functionality of the MRAM driver on" \
                                "\r\nRenesas RX MCUs using the Renesas Flexible Software Package (FSP). The user can" \
                                "\r\ninteract with the application via terminal to execute write, read, and erase"    \
                                "\r\noperations, starting at a fixed MRAM address. The operation status and data"     \
                                "\r\ncontent are displayed in the terminal.\r\n"

/* This macro is used to display the MRAM region selection */
#define EP_MENU_MRAM_REGION     "\r\n------------------------------------------------------------------------" \
                                "\r\n               SELECT MRAM REGION \r\n"                                   \
                                "\r\n       [1] Code MRAM"                                                     \
                                "\r\n       [2] Data MRAM"                                                     \
                                "\r\n       ---------------------------------------"                           \
                                "\r\n       [0] Exit"                                                          \
                                "\r\n------------------------------------------------------------------------" \
                                "\r\nPlease enter your option [0, 1, 2]: "

/* This macro is used to display the MRAM operation selection */
#define EP_MENU_MRAM_OPERATION  "\r\n------------------------------------------------------------------------" \
                                "\r\n               SELECT MRAM OPERATION \r\n"                                \
                                "\r\n        [1] MRAM Write"                                                   \
                                "\r\n        [2] MRAM Erase"                                                   \
                                "\r\n        [3] MRAM Read"                                                    \
                                "\r\n        ---------------------------------------"                          \
                                "\r\n        [0] Go back to select MRAM region"                                \
                                "\r\n------------------------------------------------------------------------" \
                                "\r\nPlease enter your option [0, 1, 2, 3]: "

/* This macro is used to indicate the number of data bytes */
#define NUM_BYTES         (32 * 4)
/* This macro is used to indicate the start address of the code MRAM region */
#define CODE_MRAM_ADDRESS (0xFFF80020)
/* This macro is used to indicate the start address of the data MRAM region */
#define DATA_MRAM_ADDRESS (0x86900000)
/* This macro is used to
 * set the initial values of the corresponding variables */
#define INIT_VALUE        (10U)
/* This macro is used to indicate the exit option in the menu */
#define EXIT              '0'
/* This macro is used to indicate the code MRAM selection in the menu */
#define CODE_MRAM_REGION  '1'
/* This macro is used to indicate the data MRAM selection in the menu */
#define DATA_MRAM_REGION  '2'
/* This macro is used to indicate the option for writing data to MRAM */
#define MRAM_WRITE        '1'
/* This macro is used to indicate the option for erasing data to MRAM */
#define MRAM_ERASE        '2'
/* This macro is used to indicate the option for reading data from MRAM */
#define MRAM_READ         '3'
/* This macro is used to indicate the buffer size */
#define BUFF_SIZE         (15U)

/* The MRAM erase code block size is 32 bytes, so the number of blocks must be properly aligned. */
#define CODE_MRAM_NUM_BLOCKS    ((NUM_BYTES + (BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES - 1)) / \
                                  BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES)

/* The MRAM erase data block size is 16 bytes, so the number of blocks must be properly aligned. */
#define DATA_MRAM_NUM_BLOCKS    ((NUM_BYTES + (BSP_FEATURE_DATA_MRAM_PROGRAMMING_SIZE_BYTES - 1)) / \
                                  BSP_FEATURE_DATA_MRAM_PROGRAMMING_SIZE_BYTES)

/*******************************************************************************************************************//**
* Function Declaration Section
***********************************************************************************************************************/
/* Public function declarations */
/* Main function of EP */
void mram_ep_entry(void);

/*******************************************************************************************************************//**
 * @} (end defgroup MRAM_EP)
 **********************************************************************************************************************/

#endif /* MRAM_EP_H_ */
