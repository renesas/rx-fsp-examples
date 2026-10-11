/***********************************************************************************************************************
 * File Name    : octa_ram_ep.h
 * Description  : Contains data structures and functions used in octa_ram_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OCTA_RAM_EP_H_
#define OCTA_RAM_EP_H_

/* Macros for OctalRAM device */
#define OCTA_RAM_DEVICE_ID                    (0x03)      /* For IS66WVO32M8DBLL OctalRAM
                                                          bit 0-3 (value: 0011) of Identification (ID) Register */
#define OCTA_RAM_APP_DATA_SIZE                (1024U)

/* OctalRAM device timing */
#define OCTA_RAM_TIME_UNIT                    (BSP_DELAY_UNITS_MICROSECONDS)
#define OCTA_RAM_TIME_RESET_SETUP             (2U)             /*  Type 50ns */
#define OCTA_RAM_TIME_RESET_PULSE             (1000U)          /*  Type 500us */

/* Macro for Main Menu handle */
#define SELECT_WRITE_OPERATION_CHAR     (1U)
#define SELECT_READ_OPERATION_CHAR      (2U)
#define SELECT_RESET_OPERATION_CHAR     (3U)
/* Macro for Sub Menu handle */
#define SUB_MENU_64_BYTES               (1U)
#define SUB_MENU_128_BYTES              (2U)
#define SUB_MENU_256_BYTES              (3U)
#define SUB_MENU_512_BYTES              (4U)
#define SUB_MENU_1024_BYTES             (5U)

#define NULL_CHAR                       ('\0')
#define CHECK_INDEX                     (0U)

#define PROTOCOL_MODE       "8D-8D-8D Protocol Mode"
#define EP_MENU             "\r\nMain Menu: "\
                            "\r\nOperation Options:"\
                            "\r\n1. Write Operation"\
                            "\r\n2. Read Operation"\
                            "\r\n3. Reset the test section"\
                            "\r\nOperation Select: \r\n"

#define EP_SUB_MENU         "\r\n  Select data size: "\
                            "\r\n    1. 64 Bytes"\
                            "\r\n    2. 128 Bytes"\
                            "\r\n    3. 256 Bytes"\
                            "\r\n    4. 512 Bytes"\
                            "\r\n    5. 1024 Bytes\r\n"\

#define EP_INFO             "\r\nThis example project demonstrates basic functionalities of OSPI driver\r\n"\
                            "on Renesas RX MCUs based on Renesas FSP. Based on the User input, EP performs\r\n"\
                            "read/write operation in DOPI mode (8D-8D-8D).\r\n"\
                            "On successful completion of each operation, success message will be printed\r\n"\
                            "on Debug Virtual Console. Error and info messages will be printed on Debug Virtual Console.\r\n\n"
/* Function declarations*/
void octa_ram_ep_entry(void);

#endif /* OCTA_RAM_EP_H_ */
