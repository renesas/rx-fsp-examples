/***********************************************************************************************************************
 * File Name    : xspi_controller_ep.h
 * Description  : Contains data structures and functions used in xspi_controller_ep.h.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef XSPI_CONTROLLER_EP_H_
#define XSPI_CONTROLLER_EP_H_

/* Macros for flash device */
#define XSPI_CONTROLLER_DEVICE_ID                    (0x3A86C2)      /* For MX25LW51245G flash */

/* Flash device sector size */
#define XSPI_CONTROLLER_SECTOR_SIZE_4K               (0x1000)
#define XSPI_CONTROLLER_SECTOR_SIZE_256K             (0x40000)
#define XSPI_CONTROLLER_SECTOR_4K_END_ADDRESS        (0x3001FFFF)

/* Flash device timing */
#define XSPI_CONTROLLER_TIME_UNIT                    (BSP_DELAY_UNITS_MICROSECONDS)
#define XSPI_CONTROLLER_TIME_RESET_SETUP             (2U)             /*  Type 50ns */
#define XSPI_CONTROLLER_TIME_RESET_PULSE             (1000U)          /*  Type 500us */
#define XSPI_CONTROLLER_TIME_ERASE_256K              (1500000U)       /*  Type 256KB sector is 331 KBps -> Type 0.773s */
#define XSPI_CONTROLLER_TIME_ERASE_4K                (100000U)        /*  Type 4KB sector is 95 KBps -> Type 0.042s */
#define XSPI_CONTROLLER_TIME_WRITE                   (10000U)         /*  Type 256B page (4KB/256KB) is 595/533 KBps -> Type */

/* Flash device status bit */
#define XSPI_CONTROLLER_WEN_BIT_MASK                 (0x00000002)
#define XSPI_CONTROLLER_BUSY_BIT_MASK                (0x00000001)

/* Flash device address space mapping */
#define XSPI_CONTROLLER_CS0_START_ADDRESS            (0x30000000)
#define XSPI_CONTROLLER_CS1_START_ADDRESS            (0x20000000)
#define XSPI_CONTROLLER_APP_ADDRESS(sector_no)    ((uint8_t *)(XSPI_CONTROLLER_CS0_START_ADDRESS + ((sector_no) * XSPI_CONTROLLER_SECTOR_SIZE_4K)))
#define XSPI_CONTROLLER_SECTOR_FIRST                 (0U)
#define XSPI_CONTROLLER_SECTOR_SECOND                (1U)
#define XSPI_CONTROLLER_SECTOR_THREE                 (2U)
#define XSPI_CONTROLLER_SECTOR_FOUR                  (3U)
#define XSPI_CONTROLLER_APP_DATA_SIZE                (64U)

/* Macro for RTT Viewer handle */
#define RTT_SELECT_SPI_MODE_CHAR            ('1')
#define RTT_SELECT_OPI_MODE_CHAR            ('2')
#define RTT_SELECT_DMA_SPI_MODE_CHAR        ('3')
#define RTT_SELECT_DMA_OPI_MODE_CHAR        ('4')
#define RTT_SELECT_WRITE_OPERATION_CHAR     ('1')
#define RTT_SELECT_READ_OPERATION_CHAR      ('2')
#define RTT_SELECT_ERASE_OPERATION_CHAR     ('3')
#define RTT_EXIT_SUB_MENU_CHAR              ('4')
#define RTT_NULL_CHAR                       ('\0')
#define RTT_CHECK_INDEX                     (0U)

#define MAIN_MENU   "\r\nXSPI Controller Main Menu, Protocol Mode Option:" \
                    "\r\n1. SPI 1S-1S-1S Protocol Mode"         \
                    "\r\n2. OPI 8D-8D-8D Protocol Mode"         \
                    "\r\nProtocol Mode Select:  \r\n"

#define SUB_MENU    "\r\nXSPI Controller Sub Menu, %s Operation Option:" \
                    "\r\n1. Write Operation"                  \
                    "\r\n2. Read Operation"                   \
                    "\r\n3. Erase Operation"                  \
                    "\r\n4. Go Back To Main menu"             \
                    "\r\nOperation Select:  \r\n"

#define EP_INFO     "\r\nThis example project demonstrates basic functionalities of XSPI Controller driver\r\n"    \
                    "on Renesas RX MCUs based on Renesas FSP. Based on the User input, EP performs\r\n" \
                    "read/write/erase operation in SPI mode (1S-1S-1S) or DOPI mode (8D-8D-8D).\r\n"    \
                    "On successful completion of each operation, success message will be printed\r\n"   \
                    "on Terminal Application Console. Error and info messages will be printed on \r\n"  \
                    "Terminal Application Console.\r\n\n"

/* Function declarations*/
fsp_err_t xspi_controller_init(void);
fsp_err_t xspi_controller_set_protocol_to_spi(void);
fsp_err_t xspi_controller_set_protocol_to_opi(void);
fsp_err_t xspi_controller_operation(uint8_t *p_address);
fsp_err_t xspi_controller_read_device_id(uint32_t *const p_id);
fsp_err_t timer_init(void);

#endif /* XSPI_CONTROLLER_EP_H_ */
